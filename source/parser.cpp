#include "bond/parser.h"
#include "bond/tokenstream.h"

namespace Bond
{

Parser::Parser(Allocator &allocator):
	mNumErrors(0),
	mFactory(allocator),
	mTranslationUnit(0)
{
	for (int i = 0; i < MAX_ERRORS; ++i)
	{
		mErrors[i] = Error();
	}
}


Parser::~Parser()
{
	Dispose();
}


void Parser::Dispose()
{
	mFactory.DestroyHierarchy(mTranslationUnit);
	mTranslationUnit = 0;
}


void Parser::Parse(TokenStream &stream)
{
	Dispose();
	mTranslationUnit = ParseTranslationUnit(stream);
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(TokenStream &stream)
{
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);
	TranslationUnit *unit = mFactory.CreateTranslationUnit(declarations);
	return unit;
}


ExternalDeclaration *Parser::ParseExternalDeclarationList(TokenStream &stream)
{
	ExternalDeclaration *head = ParseExternalDeclaration(stream);
	ExternalDeclaration *current = head;

	while (current != 0)
	{
		ExternalDeclaration *next = ParseExternalDeclaration(stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// external_declaration
//   : namespace_definition
//   | enum_declaration
//   | function_declaration
//   | function_definition
//   | struct_declaration
//   | const_declarative_statement
//
// function_declaration
//   : function_prototype ';'
//
// function_prototype
//   : type_descriptor IDENTIFIER '(' [parameter_list] ')'
ExternalDeclaration *Parser::ParseExternalDeclaration(TokenStream &stream)
{
	ExternalDeclaration *declaration = ParseNamespaceDefinition(stream);
	if (declaration == 0)
	{
		declaration = ParseEnumDeclaration(stream);
	}

	if (declaration == 0)
	{
		declaration = ParseStructDeclaration(stream);
	}

	if (declaration == 0)
	{
		TypeDescriptor *descriptor = ParseTypeDescriptor(stream);

		// Could be a function declaration, function definition or a constant declarative statement.
		if (descriptor != 0)
		{
			const Token *name = ExpectToken(stream, Token::IDENTIFIER);
			if (stream.NextIf(Token::OPAREN) != 0)
			{
				Parameter *parameterList = ParseParameterList(stream);
				ExpectToken(stream, Token::CPAREN);
				FunctionPrototype *prototype = mFactory.CreateFunctionPrototype(name, descriptor, parameterList);
				// TODO: discriminate between declaration and definition.
				ExpectToken(stream, Token::SEMICOLON);
				declaration = mFactory.CreateFunctionDefinition(prototype);
			}
		}
	}

	return declaration;
}


// namespace_definition
//   : NAMESPACE IDENTIFIER '{' external_declaration* '}'
NamespaceDefinition *Parser::ParseNamespaceDefinition(TokenStream &stream)
{
	if (stream.NextIf(Token::KEY_NAMESPACE) == 0)
	{
		return 0;
	}

	const Token *name = ExpectToken(stream, Token::IDENTIFIER);
	ExpectToken(stream, Token::OBRACE);
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);
	ExpectToken(stream, Token::CBRACE);
	NamespaceDefinition *space = mFactory.CreateNamespaceDefinition(name, declarations);
	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
EnumDeclaration *Parser::ParseEnumDeclaration(TokenStream &stream)
{
	if (stream.NextIf(Token::KEY_ENUM) == 0)
	{
		return 0;
	}

	const Token *name = ExpectToken(stream, Token::IDENTIFIER);
	ExpectToken(stream, Token::OBRACE);
	Enumerator *enumerators = ParseEnumeratorList(stream);
	ExpectToken(stream, Token::CBRACE);
	ExpectToken(stream, Token::SEMICOLON);
	EnumDeclaration *enumeration = mFactory.CreateEnumDeclaration(name, enumerators);
	return enumeration;
}


// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
Enumerator *Parser::ParseEnumeratorList(TokenStream &stream)
{
	Enumerator *head = ParseEnumerator(stream);
	Enumerator *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Enumerator *next = ParseEnumerator(stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *Parser::ParseEnumerator(TokenStream &stream)
{
	const Token *name = stream.NextIf(Token::IDENTIFIER);
	if (name == 0)
	{
		return 0;
	}

	Expression *value = 0;
	if (stream.NextIf(Token::ASSIGN) != 0)
	{
		value = ParseConstExpression(stream);
		AssertNode(value, stream);
	}

	Enumerator *enumerator = mFactory.CreateEnumerator(name, value);
	return enumerator;
}


// parameter_list
//   : parameter
// 	| parameter_list ',' parameter
Parameter *Parser::ParseParameterList(TokenStream &stream)
{
	Parameter *head = ParseParameter(stream);
	Parameter *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Parameter *next = ParseParameter(stream);
		AssertNode(next, stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// parameter
//   : type_descriptor IDENTIFIER
Parameter *Parser::ParseParameter(TokenStream &stream)
{
	Parameter *parameter = 0;
	TypeDescriptor *descriptor = ParseTypeDescriptor(stream);
	if (descriptor != 0)
	{
		const Token *name = ExpectToken(stream, Token::IDENTIFIER);
		parameter = mFactory.CreateParameter(name, descriptor);
	}
	return parameter;
}


// struct_declaration
//   : STRUCT IDENTIFIER '{' struct_member_declaration+ '}' ';'
StructDeclaration *Parser::ParseStructDeclaration(TokenStream &stream)
{
	return 0;
}


// type_descriptor
//   : CONST type_specifier
//   | type_specifier [CONST]
//   | type_descriptor '*' [CONST]
//   | type_descriptor '[' [const_expression] ']'
TypeDescriptor *Parser::ParseTypeDescriptor(TokenStream &stream)
{
	TypeDescriptor *descriptor = 0;
	const int pos = stream.GetPosition();
	const bool isConst1 = stream.NextIf(Token::KEY_CONST) != 0;
	TypeSpecifier *specifier = ParseTypeSpecifier(stream);

	if (specifier != 0)
	{
		const Token *const2 = stream.NextIf(Token::KEY_CONST);
		const bool isConst2 = const2 != 0;
		if (isConst1 && isConst2)
		{
			PushError(DUPLICATE_CONST, const2);
		}

		descriptor = mFactory.CreateTypeDescriptor(specifier, isConst1 || isConst2);

		const Token *token = stream.NextIf(TokenTypeSet::TYPE_DESCRIPTORS);
		while (token != 0)
		{
			if (token->GetTokenType() == Token::OP_MULT)
			{
				const bool isConst = stream.NextIf(Token::KEY_CONST) != 0;
				descriptor = mFactory.CreateTypeDescriptor(descriptor, isConst);
			}
			else
			{
				Expression *length = ParseConstExpression(stream);
				ExpectToken(stream, Token::CBRACKET);
				descriptor = mFactory.CreateTypeDescriptor(descriptor, length);
			}
			token = stream.NextIf(TokenTypeSet::TYPE_DESCRIPTORS);
		}
	}
	else
	{
		stream.SetPosition(pos);
	}

	return descriptor;
}


// type_specifier
//   : primitive_type_specifier
//   | qualified_id
TypeSpecifier *Parser::ParseTypeSpecifier(TokenStream &stream)
{
	TypeSpecifier *specifier = ParsePrimitiveTypeSpecifier(stream);
	if (specifier == 0)
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(stream);
		if (identifier != 0)
		{
			specifier = mFactory.CreateTypeSpecifier(identifier);
		}
	}

	return specifier;
}


// primitive_type_specifier
//   : VOID
//   | BOOL
//   | CHAR
//   | INT
//   | UINT
//   | FLOAT
TypeSpecifier *Parser::ParsePrimitiveTypeSpecifier(TokenStream &stream)
{
	TypeSpecifier *specifier = 0;
	const Token *primitiveType = stream.NextIf(TokenTypeSet::PRIMITIVE_TYPE_SPECIFIERS);
	if (primitiveType != 0)
	{
		specifier = mFactory.CreateTypeSpecifier(primitiveType);
	}
	return specifier;
}


// qualified_id
//   : IDENTIFIER
//   | qualified_id '::' IDENTIFIER
QualifiedIdentifier *Parser::ParseQualifiedIdentifier(TokenStream &stream)
{
	QualifiedIdentifier *id = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);
	if (name != 0)
	{
		id = mFactory.CreateQualifiedIdentifier(name);
		if (stream.NextIf(Token::SCOPE))
		{
			QualifiedIdentifier *next = ParseQualifiedIdentifier(stream);
			AssertNode(next, stream);
			id->SetNext(next);
		}
	}
	return id;
}


// const_expression
//   : conditional_expression
Expression *Parser::ParseConstExpression(TokenStream &stream)
{
	return ParseConditionalExpression(stream, EXP_CONST);
}


// expression
//   : assignment_expression
//   | expression ',' assignment_expression
Expression *Parser::ParseExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression;
	if (qualifier == EXP_CONST)
	{
		expression = ParseConstExpression(stream);
	}
	else
	{
		expression = ParseAssignmentExpression(stream, qualifier);
 
		if (expression != 0)
 		{
			const Token *token = stream.NextIf(Token::COMMA);
			while (token != 0)
			{
				Expression *rhs = ParseAssignmentExpression(stream, qualifier);
				AssertNode(rhs, stream);
				expression = mFactory.CreateBinaryExpression(token, expression, rhs);
				token = stream.NextIf(Token::COMMA);
			}
 		}
 	}
 
 	return expression;
}


// assignment_expression
//   : conditional_expression
//   | unary_expression '=' assignment_expression
//   | unary_expression '<<=' assignment_expression
//   | unary_expression '>>=' assignment_expression
//   | unary_expression '+=' assignment_expression
//   | unary_expression '-=' assignment_expression
//   | unary_expression '*=' assignment_expression
//   | unary_expression '/=' assignment_expression
//   | unary_expression '%=' assignment_expression
//   | unary_expression '&=' assignment_expression
//   | unary_expression '^=' assignment_expression
//   | unary_expression '|=' assignment_expression
Expression *Parser::ParseAssignmentExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	// TODO: This function can produce pretty much any type of expression on the lhs of an assignment.
	// Will need to do further analysis in the semantic analyser to ensure validity.
	Expression *expression = ParseConditionalExpression(stream, qualifier);
	if (expression == 0)
	{
		expression = ParseUnaryExpression(stream, qualifier);
	}

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ASSIGNMENT_OPERATORS);
		if (token != 0)
		{
			AssertConstExpression(qualifier, ASSIGNMENT_IN_CONST_EXPRESSION, token);
			Expression *rhs = ParseAssignmentExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
		}
	}
	return expression;
}


// conditional_expression
//   : logical_or_expression
//   | logical_or_expression '?' expression ':' conditional_expression
Expression *Parser::ParseConditionalExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseLogicalOrExpression(stream, qualifier);

	if ((expression != 0) && (stream.NextIf(Token::OP_TERNARY) != 0))
	{
		Expression *trueExpression = ParseExpression(stream, qualifier);
		AssertNode(trueExpression, stream);
		ExpectToken(stream, Token::COLON);
		Expression *falseExpression = ParseConditionalExpression(stream, qualifier);
		AssertNode(falseExpression, stream);
		expression = mFactory.CreateConditionalExpression(expression, trueExpression, falseExpression);
	}

	return expression;
}


// logical_or_expression
//   : logical_and_expression
//   | logical_or_expression '||' logical_and_expression
Expression *Parser::ParseLogicalOrExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseLogicalAndExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_OR);
		while (token != 0)
		{
			Expression *rhs = ParseLogicalAndExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_OR);
		}
	}

	return expression;
}


// logical_and_expression
//   : inclusive_or_expression
//   | logical_and_expression '&&' inclusive_or_expression
Expression *Parser::ParseLogicalAndExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseInclusiveOrExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_AND);
		while (token != 0)
		{
			Expression *rhs = ParseInclusiveOrExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_AND);
		}
	}

	return expression;
}


// inclusive_or_expression
//   : exclusive_or_expression
//   | inclusive_or_expression '|' exclusive_or_expression
Expression *Parser::ParseInclusiveOrExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseExclusiveOrExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_OR);
		while (token != 0)
		{
			Expression *rhs = ParseExclusiveOrExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_BIT_OR);
		}
	}

	return expression;
}


// exclusive_or_expression
//   : and_expression
//   | exclusive_or_expression '^' and_expression
Expression *Parser::ParseExclusiveOrExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseAndExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_XOR);
		while (token != 0)
		{
			Expression *rhs = ParseAndExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_BIT_XOR);
		}
	}

	return expression;
}


// and_expression
//   : equality_expression
//   | and_expression '&' equality_expression
Expression *Parser::ParseAndExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseEqualityExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_AND);
		while (token != 0)
		{
			Expression *rhs = ParseEqualityExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_BIT_AND);
		}
	}

	return expression;
}


// equality_expression
//   : relational_expression
//   | equality_expression '==' relational_expression
//   | equality_expression '!=' relational_expression
Expression *Parser::ParseEqualityExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseRelationalExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::EQUALITY_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseRelationalExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::EQUALITY_OPERATORS);
		}
	}

	return expression;
}


// relational_expression
//   : shift_expression
//   | relational_expression '<' shift_expression
//   | relational_expression '>' shift_expression
//   | relational_expression '<=' shift_expression
//   | relational_expression '>=' shift_expression
Expression *Parser::ParseRelationalExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseShiftExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::RELATIONAL_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseShiftExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::RELATIONAL_OPERATORS);
		}
	}

	return expression;
}


// shift_expression
//   : additive_expression
//   | shift_expression '<<' additive_expression
//   | shift_expression '>>' additive_expression
Expression *Parser::ParseShiftExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseAdditiveExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::SHIFT_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseAdditiveExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::SHIFT_OPERATORS);
		}
	}

	return expression;
}


// additive_expression
//   : multiplicative_expression
//   | additive_expression '+' multiplicative_expression
//   | additive_expression '-' multiplicative_expression
Expression *Parser::ParseAdditiveExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseMultiplicativeExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ADDITIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseMultiplicativeExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::ADDITIVE_OPERATORS);
		}
	}

	return expression;
}


// multiplicative_expression
//   : cast_expression
//   | multiplicative_expression '*' cast_expression
//   | multiplicative_expression '/' cast_expression
//   | multiplicative_expression '%' cast_expression
Expression *Parser::ParseMultiplicativeExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParseCastExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseCastExpression(stream, qualifier);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		}
	}

	return expression;
}


// cast_expression
//   : unary_expression
//   | CAST '<' type_descriptor '>' '(' cast_expression ')'
Expression *Parser::ParseCastExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = 0;

	if (stream.NextIf(Token::KEY_CAST))
	{
		ExpectToken(stream, Token::OP_LT);
		TypeDescriptor *descriptor = ParseTypeDescriptor(stream);
		AssertNode(descriptor, stream);
		ExpectToken(stream, Token::OP_GT);
		ExpectToken(stream, Token::OPAREN);
		Expression *rhs = ParseCastExpression(stream, qualifier);
		AssertNode(rhs, stream);
		ExpectToken(stream, Token::CPAREN);
		expression = mFactory.CreateCastExpression(descriptor, rhs);
	}
	else
	{
		expression = ParseUnaryExpression(stream, qualifier);
	}

	return expression;
}


// unary_expression
//   : postfix_expression
//   | '++' unary_expression
//   | '--' unary_expression
//   | '&' cast_expression
//   | '*' cast_expression
//   | '+' cast_expression
//   | '-' cast_expression
//   | '~' cast_expression
//   | '!' cast_expression
//   | SIZEOF unary_expression
//   | SIZEOF '(' type_descriptor ')'
Expression *Parser::ParseUnaryExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = 0;
	const Token *op = stream.NextIf(TokenTypeSet::UNARY_OPERATORS);

	if (op != 0)
	{
		Expression *rhs = 0;

		switch (op->GetTokenType())
		{
			case Token::OP_INC:
			case Token::OP_DEC:
				rhs = ParseUnaryExpression(stream, qualifier);
				AssertConstExpression(qualifier, INCREMENT_IN_CONST_EXPRESSION, op);
				break;

			default:
				rhs = ParseCastExpression(stream, qualifier);
				break;
		}

		AssertNode(rhs, stream);
		expression = mFactory.CreateUnaryExpression(op, rhs);
	}

	else if (stream.NextIf(Token::KEY_SIZEOF) != 0)
	{
		Expression *unary = ParseUnaryExpression(stream, qualifier);
		if (unary != 0)
		{
			expression = mFactory.CreateSizeofExpression(unary);
		}
		else
		{
			ExpectToken(stream, Token::OPAREN);
			TypeDescriptor *descriptor = ParseTypeDescriptor(stream);
			AssertNode(descriptor, stream);
			ExpectToken(stream, Token::CPAREN);
			expression = mFactory.CreateSizeofExpression(descriptor);
		}
	}

	else
	{
		expression = ParsePostfixExpression(stream, qualifier);
	}

	return expression;
}


// postfix_expression
//   : primary_expression
//   | postfix_expression '[' expression ']'
//   | postfix_expression '(' [argument_expression_list] ')'
//   | postfix_expression '.' IDENTIFIER
//   | postfix_expression '->' IDENTIFIER
//   | postfix_expression '++'
//   | postfix_expression '--'
Expression *Parser::ParsePostfixExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = ParsePrimaryExpression(stream, qualifier);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::POSTFIX_OPERATORS);
		while (token != 0)
		{
			switch (token->GetTokenType())
			{
				case Token::OBRACKET:
				{
					Expression *index = ParseExpression(stream, qualifier);
					expression = mFactory.CreateArraySubscriptExpression(expression, index);
					ExpectToken(stream, Token::CBRACKET);
				}
				break;

				case Token::OPAREN:
				{
					Expression *argumentList = ParseArgumentList(stream);
					expression = mFactory.CreateFunctionCallExpression(expression, argumentList);
					AssertConstExpression(qualifier, FUNCTION_CALL_IN_CONST_EXPRESSION, token);
					ExpectToken(stream, Token::CPAREN);
				}
				break;

				case Token::OP_ARROW:
				case Token::PERIOD:
				{
					const Token *memberName = ExpectToken(stream, Token::IDENTIFIER);
					expression = mFactory.CreateMemberExpression(token, memberName, expression);
				}
				break;

				default:
				{
					expression = mFactory.CreatePostfixExpression(token, expression);
					AssertConstExpression(qualifier, INCREMENT_IN_CONST_EXPRESSION, token);
				}
				break;
			}
			token = stream.NextIf(TokenTypeSet::POSTFIX_OPERATORS);
		}
	}

	return expression;
}


// primary_expression
//   : qualified_id
//   | CONSTANT
//   | '(' expression ')'
Expression *Parser::ParsePrimaryExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = 0;
	const int pos = stream.GetPosition();
	const Token *value = stream.NextIf(TokenTypeSet::CONSTANT_VALUES);

	if (value != 0)
	{
		expression = mFactory.CreateConstantExpression(value);
	}
	else if (stream.NextIf(Token::OPAREN) != 0)
	{
		expression = ParseExpression(stream, qualifier);
		if (expression != 0)
		{
			ExpectToken(stream, Token::CPAREN);
		}
		else
		{
			stream.SetPosition(pos);
		}
	}
	else
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(stream);
		if (identifier != 0)
		{
			expression = mFactory.CreateIdentifierExpression(identifier);
		}
	}

	return expression;
}


// argument_expression_list
//   : assignment_expression
//   | argument_expression_list ',' assignment_expression
Expression *Parser::ParseArgumentList(TokenStream &stream)
{
	Expression *head = ParseExpression(stream);
	Expression *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Expression *next = ParseExpression(stream);
		AssertNode(next, stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


const Token *Parser::ExpectToken(TokenStream &stream, Token::TokenType expectedType)
{
	const Token *token = stream.NextIf(expectedType);
	if (token == 0)
	{
		PushError(UNEXPECTED_TOKEN, stream.Peek(), Token::GetTokenName(expectedType));
	}
	return token;
}


const Token *Parser::ExpectToken(TokenStream &stream, TokenTypeSet &typeSet)
{
	const Token *token = stream.NextIf(typeSet);
	if (token == 0)
	{
		PushError(UNEXPECTED_TOKEN, stream.Peek(), typeSet.typeName);
	}
	return token;
}


void Parser::AssertNode(ParseNode *node, const TokenStream &stream)
{
	if (node == 0)
	{
		PushError(PARSE_ERROR, stream.Peek());
	}
}


void Parser::AssertConstExpression(ExpressionQualifier qualifier, ErrorType errorType, const Token *token)
{
	if (qualifier != EXP_CONST)
	{
		PushError(errorType, token);
	}
}


void Parser::PushError(ErrorType type, const Token *token, const char *expected)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = Error(type, token, expected);
		++mNumErrors;
	}
}

}
