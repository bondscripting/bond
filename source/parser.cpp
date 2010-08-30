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
//   | function_definition
//   | function_declaration
//   | struct_declaration
//   | enum_declaration
//   | const_declaration
ExternalDeclaration *Parser::ParseExternalDeclaration(TokenStream &stream)
{
	ExternalDeclaration *declaration = ParseNamespaceDefinition(stream);
	if (declaration == 0)
	{
		declaration = ParseEnumDeclaration(stream);
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

	while (current != 0)
	{
		if (stream.NextIf(Token::COMMA) == 0)
		{
			break;
		}
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
		AssertNode(stream, value);
	}

	Enumerator *enumerator = mFactory.CreateEnumerator(name, value);
	return enumerator;
}


// type_descriptor
//   : CONST type_specifier
//   | type_specifier [CONST]
//   | type_descriptor '*' [CONST]
//   | type_descriptor '[' [const_expression] ']'
TypeDescriptor *Parser::ParseTypeDescriptor(TokenStream &stream)
{
	// TODO
	return 0;
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
				AssertNode(stream, rhs);
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
	// TODO: Handle conditional_expression.
	Expression *lhs = ParseUnaryExpression(stream, qualifier);
	const Token *op = ExpectToken(stream, TokenTypeSet::ASSIGNMENT_OPERATORS);
	Expression *rhs = ParseAssignmentExpression(stream, qualifier);
	AssertNode(stream, rhs);
	Expression *expression = mFactory.CreateBinaryExpression(op, lhs, rhs);
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
		AssertNode(stream, trueExpression);
		ExpectToken(stream, Token::COLON);
		Expression *falseExpression = ParseConditionalExpression(stream, qualifier);
		AssertNode(stream, falseExpression);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
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
			AssertNode(stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		}
	}

	return expression;
}


// cast_expression
//   : unary_expression
//   | '(' type_descriptor ')' cast_expression
Expression *Parser::ParseCastExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	Expression *expression = 0;

	if (stream.NextIf(Token::OPAREN))
	{
		TypeDescriptor *descriptor = ParseTypeDescriptor(stream);
		AssertNode(stream, descriptor);
		ExpectToken(stream, Token::CPAREN);
		Expression *rhs = ParseCastExpression(stream, qualifier);
		AssertNode(stream, rhs);
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

				if (qualifier == EXP_CONST)
				{
					PushError(INCREMENT_IN_CONST_EXPRESSION, op);
				}
				break;

			default:
				rhs = ParseCastExpression(stream, qualifier);
				break;
		}

		AssertNode(stream, rhs);
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
			AssertNode(stream, descriptor);
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
//   | postfix_expression '(' ')'
//   | postfix_expression '(' argument_expression_list ')'
//   | postfix_expression '.' IDENTIFIER
//   | postfix_expression '->' IDENTIFIER
//   | postfix_expression '++'
//   | postfix_expression '--'
Expression *Parser::ParsePostfixExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	// TODO
	Expression *expression = ParsePrimaryExpression(stream, qualifier);
	return expression;
}


// primary_expression
//   : namespaced_identifier
//   | CONSTANT
//   | '(' expression ')'
Expression *Parser::ParsePrimaryExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	// TODO
	Expression *expression = 0;

	const Token *value = stream.NextIf(TokenTypeSet::CONSTANT_VALUES);
	if (value != 0)
	{
		expression = mFactory.CreateConstantValue(value);
	}

	return expression;
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


void Parser::AssertNode(TokenStream &stream, ParseNode *node)
{
	if (node == 0)
	{
		PushError(PARSE_ERROR, stream.Peek());
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
