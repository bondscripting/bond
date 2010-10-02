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
	Status status;
	mTranslationUnit = ParseTranslationUnit(stream, status);
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(TokenStream &stream, Status &status)
{
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream, status);
	TranslationUnit *unit = mFactory.CreateTranslationUnit(declarations);
	return unit;
}


ExternalDeclaration *Parser::ParseExternalDeclarationList(TokenStream &stream, Status &status)
{
	ExternalDeclaration *head = ParseExternalDeclaration(stream, status);
	ExternalDeclaration *current = head;

	while (current != 0)
	{
		ExternalDeclaration *next = ParseExternalDeclaration(stream, status);
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
// function_definition
//   : function_prototype compound_statement
//
// function_declaration
//   : function_prototype ';'
//
// function_prototype
//   : type_descriptor IDENTIFIER '(' [parameter_list] ')'
ExternalDeclaration *Parser::ParseExternalDeclaration(TokenStream &stream, Status &status)
{
	ExternalDeclaration *declaration = 0;

	switch (stream.Peek()->GetTokenType())
	{
		case Token::KEY_NAMESPACE:
			declaration = ParseNamespaceDefinition(stream, status);
			break;

		case Token::KEY_ENUM:
			declaration = ParseEnumDeclaration(stream, status);
			break;

		case Token::KEY_STRUCT:
			declaration = ParseStructDeclaration(stream, status);
			break;

		default:
		{
			// TODO: move into a separate function once const_declarative_statement is figured out.
			TypeDescriptor *descriptor = ParseTypeDescriptor(stream, status);

			// Could be a function declaration, function definition or a constant declarative statement.
			if (descriptor != 0)
			{
				const Token *name = ExpectToken(stream, Token::IDENTIFIER);
				if (stream.NextIf(Token::OPAREN) != 0)
				{
					Parameter *parameterList = ParseParameterList(stream, status);
					ExpectToken(stream, Token::CPAREN);
					FunctionPrototype *prototype = mFactory.CreateFunctionPrototype(name, descriptor, parameterList);
					CompoundStatement *body = ParseCompoundStatement(stream, status);
					if (body == 0)
					{
						ExpectToken(stream, Token::SEMICOLON);
					}
					declaration = mFactory.CreateFunctionDefinition(prototype, body);
				}
				else
				{
					// TODO: Do something else we'll memory leak the type descriptor.
				}
			}
		}
		break;
	}

	return declaration;
}


// namespace_definition
//   : NAMESPACE IDENTIFIER '{' external_declaration* '}'
NamespaceDefinition *Parser::ParseNamespaceDefinition(TokenStream &stream, Status &status)
{
	NamespaceDefinition *space = 0;

	if (stream.NextIf(Token::KEY_NAMESPACE) != 0)
	{
		const Token *name = ExpectToken(stream, Token::IDENTIFIER);
		ExpectToken(stream, Token::OBRACE);
		ExternalDeclaration *declarations = ParseExternalDeclarationList(stream, status);
		ExpectToken(stream, Token::CBRACE);
		space = mFactory.CreateNamespaceDefinition(name, declarations);
	}

	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
EnumDeclaration *Parser::ParseEnumDeclaration(TokenStream &stream, Status &status)
{
	EnumDeclaration *enumeration = 0;

	if (stream.NextIf(Token::KEY_ENUM) != 0)
	{
		const Token *name = ExpectToken(stream, Token::IDENTIFIER);
		ExpectToken(stream, Token::OBRACE);
		Enumerator *enumerators = ParseEnumeratorList(stream, status);
		ExpectToken(stream, Token::CBRACE);
		ExpectToken(stream, Token::SEMICOLON);
		enumeration = mFactory.CreateEnumDeclaration(name, enumerators);
	}

	return enumeration;
}


// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
Enumerator *Parser::ParseEnumeratorList(TokenStream &stream, Status &status)
{
	Enumerator *head = ParseEnumerator(stream, status);
	Enumerator *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Enumerator *next = ParseEnumerator(stream, status);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *Parser::ParseEnumerator(TokenStream &stream, Status &status)
{
	Enumerator *enumerator = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		Expression *value = 0;
		if (stream.NextIf(Token::ASSIGN) != 0)
		{
			value = ParseConstExpression(stream, status);
			AssertNode(value, stream);
		}
		enumerator = mFactory.CreateEnumerator(name, value);
	}

	return enumerator;
}


// parameter_list
//   : parameter
//   | parameter_list ',' parameter
Parameter *Parser::ParseParameterList(TokenStream &stream, Status &status)
{
	Parameter *head = ParseParameter(stream, status);
	Parameter *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Parameter *next = ParseParameter(stream, status);
		AssertNode(next, stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// parameter
//   : type_descriptor IDENTIFIER
Parameter *Parser::ParseParameter(TokenStream &stream, Status &status)
{
	Parameter *parameter = 0;
	TypeDescriptor *descriptor = ParseTypeDescriptor(stream, status);

	if (descriptor != 0)
	{
		const Token *name = ExpectToken(stream, Token::IDENTIFIER);
		parameter = mFactory.CreateParameter(name, descriptor);
	}

	return parameter;
}


// struct_declaration
//   : STRUCT IDENTIFIER '{' struct_member_declaration+ '}' ';'
StructDeclaration *Parser::ParseStructDeclaration(TokenStream &stream, Status &status)
{
	// TODO:
	return 0;
}


// type_descriptor
//   : CONST type_specifier
//   | type_specifier [CONST]
//   | type_descriptor '*' [CONST]
//   | type_descriptor '[' [const_expression] ']'
TypeDescriptor *Parser::ParseRelaxedTypeDescriptor(TokenStream &stream, Status &status)
{
	Status overrideStatus(status);
	overrideStatus.ParseRelaxedTypeDescriptors();
	return ParseTypeDescriptor(stream, overrideStatus);
}
TypeDescriptor *Parser::ParseTypeDescriptor(TokenStream &stream, Status &status)
{
	TypeDescriptor *descriptor = 0;
	const int pos = stream.GetPosition();
	const bool isConst1 = stream.NextIf(Token::KEY_CONST) != 0;
	TypeSpecifier *specifier = ParseTypeSpecifier(stream, status);

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
				Expression *length = status.ShouldParseRelaxedTypeDescriptors() ?
					ParseExpression(stream, status) : ParseConstExpression(stream, status);
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
TypeSpecifier *Parser::ParseTypeSpecifier(TokenStream &stream, Status &status)
{
	TypeSpecifier *specifier = ParsePrimitiveTypeSpecifier(stream, status);

	if (specifier == 0)
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(stream, status);
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
TypeSpecifier *Parser::ParsePrimitiveTypeSpecifier(TokenStream &stream, Status &status)
{
	TypeSpecifier *specifier = 0;
	const Token *primitiveType = stream.NextIf(TokenTypeSet::PRIMITIVE_TYPE_SPECIFIERS);

	if (primitiveType != 0)
	{
		specifier = mFactory.CreateTypeSpecifier(primitiveType);
	}

	return specifier;
}


// named_initializer_list
//   : named_initializer
//   | named_initializer_list ',' named_initializer
NamedInitializer *Parser::ParseNamedInitializerList(TokenStream &stream, Status &status)
{
	NamedInitializer *head = ParseNamedInitializer(stream, status);
	NamedInitializer *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		NamedInitializer *next = ParseNamedInitializer(stream, status);
		AssertNode(next, stream);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// named_initializer
//   : IDENTIFIER ['=' initializer]
NamedInitializer *Parser::ParseNamedInitializer(TokenStream &stream, Status &status)
{
	NamedInitializer *namedInitializer = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		// TODO: Parse initializer
		namedInitializer = mFactory.CreateNamedInitializer(name);
	}

	return namedInitializer;
}


// qualified_id
//   : IDENTIFIER
//   | qualified_id '::' IDENTIFIER
QualifiedIdentifier *Parser::ParseQualifiedIdentifier(TokenStream &stream, Status &status)
{
	QualifiedIdentifier *id = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		id = mFactory.CreateQualifiedIdentifier(name);
		if (stream.NextIf(Token::SCOPE))
		{
			QualifiedIdentifier *next = ParseQualifiedIdentifier(stream, status);
			AssertNode(next, stream);
			id->SetNext(next);
		}
	}

	return id;
}


// statement
//   : compound_statement
//   | if_statement
//   | switch_statement
//   | while_statement
//   | do_statement
//   | for_statement
//   | jump_statement
//   | labeled_statement
//   | declarative_statement
//   | expression_statement
Statement *Parser::ParseStatement(TokenStream &stream, Status &status)
{
	Statement *statement = 0;

	switch (stream.Peek()->GetTokenType())
	{
		case Token::OBRACE:
			statement = ParseCompoundStatement(stream, status);
			break;

		case Token::KEY_IF:
			statement = ParseIfStatement(stream, status);
			break;

		case Token::KEY_SWITCH:
			statement = ParseSwitchStatement(stream, status);
			break;

		case Token::KEY_WHILE:
			statement = ParseWhileStatement(stream, status);
			break;

		case Token::KEY_DO:
			statement = ParseDoWhileStatement(stream, status);
			break;

		case Token::KEY_BREAK:
		case Token::KEY_CONTINUE:
		case Token::KEY_RETURN:
			statement = ParseJumpStatement(stream, status);
			break;

		default:
			statement = ParseDeclarativeOrExpressionStatement(stream, status);
			break;
	}

	return statement;
}


// compound_statement
//   : '{' statement* '}'
CompoundStatement *Parser::ParseCompoundStatement(TokenStream &stream, Status &status)
{
	CompoundStatement *compoundStatement = 0;

	if (stream.NextIf(Token::OBRACE))
	{
		Statement *statementList = ParseStatement(stream, status);
		Statement *current = statementList;

		while ((current != 0) && (stream.PeekIf(Token::CBRACE) == 0))
		{
			Statement *next = ParseStatement(stream, status);
			current->SetNext(next);
			current = next;
		}

		ExpectToken(stream, Token::CBRACE);
		compoundStatement = mFactory.CreateCompoundStatement(statementList);
	}

	return compoundStatement;
}


// if_statement
//   : IF '(' expression ')' statement [ELSE statement]
IfStatement *Parser::ParseIfStatement(TokenStream &stream, Status &status)
{
	IfStatement *ifStatement = 0;

	if (stream.NextIf(Token::KEY_IF) != 0)
	{
		ExpectToken(stream, Token::OPAREN);
		Expression *condition = ParseExpression(stream, status);
		AssertNode(condition, stream);
		ExpectToken(stream, Token::CPAREN);
		Statement *thenStatement = ParseStatement(stream, status);
		AssertNode(thenStatement, stream);
		Statement *elseStatement = 0;

		if (stream.NextIf(Token::KEY_ELSE))
		{
			elseStatement = ParseStatement(stream, status);
			AssertNode(elseStatement, stream);
		}

		ifStatement = mFactory.CreateIfStatement(condition, thenStatement, elseStatement);
	}

	return ifStatement;
}


// switch_statement
//   : SWITCH '(' expression ')' '{' switch_section* '}'
SwitchStatement *Parser::ParseSwitchStatement(TokenStream &stream, Status &status)
{
	SwitchStatement *switchStatement = 0;

	if (stream.NextIf(Token::KEY_SWITCH) != 0)
	{
		ExpectToken(stream, Token::OPAREN);
		Expression *control = ParseExpression(stream, status);
		AssertNode(control, stream);
		ExpectToken(stream, Token::CPAREN);
		ExpectToken(stream, Token::OBRACE);

		SwitchSection *sectionList = ParseSwitchSection(stream, status);
		SwitchSection *current = sectionList;
		while ((current != 0) && (stream.PeekIf(Token::CBRACE) == 0))
		{
			SwitchSection *next = ParseSwitchSection(stream, status);
			current->SetNext(next);
			current = next;
		}
		// TODO: Ensure list is not empty.

		ExpectToken(stream, Token::CBRACE);
		switchStatement = mFactory.CreateSwitchStatement(control, sectionList);
	}

	return switchStatement;
}


// switch_section
//   : switch_label+ statement+
SwitchSection *Parser::ParseSwitchSection(TokenStream &stream, Status &status)
{
	SwitchSection *section = 0;
	SwitchLabel *labelList = ParseSwitchLabel(stream, status);

	if (labelList != 0)
	{
		SwitchLabel *currentLabel = labelList;
		while (currentLabel != 0)
		{
			SwitchLabel *next = ParseSwitchLabel(stream, status);
			currentLabel->SetNext(next);
			currentLabel = next;
		}

		Statement *statementList = ParseStatement(stream, status);
		Statement *currentStatement = statementList;
		while (currentStatement != 0)
		{
			Statement *next = ParseStatement(stream, status);
			currentStatement->SetNext(next);
			currentStatement = next;
		}
		// TODO: Ensure list is not empty.

		section = mFactory.CreateSwitchSection(labelList, statementList);;
	}

	return section;
}


// switch_label
//   : CASE const_expression ':'
//   | DEFAULT ':'
SwitchLabel *Parser::ParseSwitchLabel(TokenStream &stream, Status &status)
{
	SwitchLabel *label = 0;
	const Token *labelToken = stream.NextIf(TokenTypeSet::SWITCH_LABELS);

	if (labelToken != 0)
	{
		if (labelToken->GetTokenType() == Token::KEY_CASE)
		{
			Expression *expression = ParseConstExpression(stream, status);
			AssertNode(expression, stream);
			label = mFactory.CreateSwitchLabel(labelToken, expression);
		}
		else
		{
			label = mFactory.CreateDefaultLabel(labelToken);
		}

		ExpectToken(stream, Token::COLON);
	}

	return label;
}


// while_statement
//   : WHILE '(' expression ')' statement
WhileStatement *Parser::ParseWhileStatement(TokenStream &stream, Status &status)
{
	WhileStatement *whileStatement = 0;

	if (stream.NextIf(Token::KEY_WHILE) != 0)
	{
		ExpectToken(stream, Token::OPAREN);
		Expression *condition = ParseExpression(stream, status);
		AssertNode(condition, stream);
		ExpectToken(stream, Token::CPAREN);
		Statement *body = ParseStatement(stream, status);
		AssertNode(body, stream);
		whileStatement = mFactory.CreateWhileStatement(condition, body);
	}

	return whileStatement;
}


// do_statement
//   : DO statement WHILE '(' expression ')' ';'
WhileStatement *Parser::ParseDoWhileStatement(TokenStream &stream, Status &status)
{
	WhileStatement *whileStatement = 0;

	if (stream.NextIf(Token::KEY_DO) != 0)
	{
		Statement *body = ParseStatement(stream, status);
		AssertNode(body, stream);
		ExpectToken(stream, Token::KEY_WHILE);
		ExpectToken(stream, Token::OPAREN);
		Expression *condition = ParseExpression(stream, status);
		AssertNode(condition, stream);
		ExpectToken(stream, Token::CPAREN);
		ExpectToken(stream, Token::SEMICOLON);
		whileStatement = mFactory.CreateDoWhileStatement(condition, body);
	}

	return whileStatement;
}


// jump_statement
//   : CONTINUE ';'
//   | BREAK ';'
//   | RETURN [expression] ';'
JumpStatement *Parser::ParseJumpStatement(TokenStream &stream, Status &status)
{
	JumpStatement *jumpStatement = 0;
	const Token *op = stream.NextIf(TokenTypeSet::JUMP_OPERATORS);

	if (op != 0)
	{
		Expression *rhs = 0;
		if (op->GetTokenType() == Token::KEY_RETURN)
		{
			rhs = ParseExpression(stream, status);
		}
		ExpectToken(stream, Token::SEMICOLON);
		jumpStatement = mFactory.CreateJumpStatement(op, rhs);
	}

	return jumpStatement;
}


// declarative_statement
//   : type_descriptor named_initializer_list ';'
Statement *Parser::ParseDeclarativeOrExpressionStatement(TokenStream &stream, Status &status)
{
	Statement *statement = 0;
	const int startPos = stream.GetPosition();

	// The grammar is somewhat ambiguous. Since a qualified identifier followed by '*' tokens and array
	// index operators can appear like a type descriptor as well as an expression, we'll treat anything
	// that fits the profile of a declaration as such and everything else like an expression statement.
	TypeDescriptor *descriptor = ParseRelaxedTypeDescriptor(stream, status);
	if (descriptor != 0)
	{
		NamedInitializer *initializerList = ParseNamedInitializerList(stream, status);

		if (initializerList != 0)
		{
			statement = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
			ExpectToken(stream, Token::SEMICOLON);
		}
		else
		{
			// The tokens that looked like a type descriptor, might actually be part of an expression.
			const int descriptorPos = stream.GetPosition();
			mFactory.DestroyHierarchy(descriptor);
			descriptor = 0;
			stream.SetPosition(startPos);

			statement = ParseExpressionStatement(stream, status);

			if (statement == 0)
			{
				// Uh, oh. Looks like we're even worse off.
				stream.SetPosition(descriptorPos);
				AssertNode(statement, stream);
			}
		}
	}

	else
	{
		statement = ParseExpressionStatement(stream, status);
	}

	return statement;
}


// expression_statement
//   : [expression] ';'
ExpressionStatement *Parser::ParseExpressionStatement(TokenStream &stream, Status &status)
{
	ExpressionStatement *expressionStatement = 0;
	Expression *expression = ParseExpression(stream, status);

	if (expression != 0)
	{
		expressionStatement = mFactory.CreateExpressionStatement(expression);
		ExpectToken(stream, Token::SEMICOLON);
	}

	return expressionStatement;
}


// const_expression
//   : conditional_expression
//
// expression
//   : assignment_expression
//   | expression ',' assignment_expression
Expression *Parser::ParseConstExpression(TokenStream &stream, Status &status)
{
	Status overrideStatus(status);
	overrideStatus.ParseConstExpressions();
	return ParseExpression(stream, overrideStatus);
}
Expression *Parser::ParseExpression(TokenStream &stream, Status &status)
{
	Expression *expression = 0;

	if (status.ShouldParseConstExpressions())
	{
		expression = ParseConditionalExpression(stream, status);
	}
	else
	{
		expression = ParseAssignmentExpression(stream, status);
 
		if (expression != 0)
 		{
			const Token *token = stream.NextIf(Token::COMMA);
			while (token != 0)
			{
				Expression *rhs = ParseAssignmentExpression(stream, status);
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
Expression *Parser::ParseAssignmentExpression(TokenStream &stream, Status &status)
{
	// TODO: This function can produce pretty much any type of expression on the lhs of an assignment.
	// Will need to do further analysis in the semantic analyser to ensure validity.
	Expression *expression = ParseConditionalExpression(stream, status);

	if (expression == 0)
	{
		expression = ParseUnaryExpression(stream, status);
	}

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ASSIGNMENT_OPERATORS);
		if (token != 0)
		{
			AssertNonConstExpression(status, ASSIGNMENT_IN_CONST_EXPRESSION, token);
			Expression *rhs = ParseAssignmentExpression(stream, status);
			AssertNode(rhs, stream);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
		}
	}

	return expression;
}


// conditional_expression
//   : logical_or_expression
//   | logical_or_expression '?' expression ':' conditional_expression
Expression *Parser::ParseConditionalExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseLogicalOrExpression(stream, status);

	if ((expression != 0) && (stream.NextIf(Token::OP_TERNARY) != 0))
	{
		Expression *trueExpression = ParseExpression(stream, status);
		AssertNode(trueExpression, stream);
		ExpectToken(stream, Token::COLON);
		Expression *falseExpression = ParseConditionalExpression(stream, status);
		AssertNode(falseExpression, stream);
		expression = mFactory.CreateConditionalExpression(expression, trueExpression, falseExpression);
	}

	return expression;
}


// logical_or_expression
//   : logical_and_expression
//   | logical_or_expression '||' logical_and_expression
Expression *Parser::ParseLogicalOrExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseLogicalAndExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_OR);
		while (token != 0)
		{
			Expression *rhs = ParseLogicalAndExpression(stream, status);
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
Expression *Parser::ParseLogicalAndExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseInclusiveOrExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_AND);
		while (token != 0)
		{
			Expression *rhs = ParseInclusiveOrExpression(stream, status);
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
Expression *Parser::ParseInclusiveOrExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseExclusiveOrExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_OR);
		while (token != 0)
		{
			Expression *rhs = ParseExclusiveOrExpression(stream, status);
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
Expression *Parser::ParseExclusiveOrExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseAndExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_XOR);
		while (token != 0)
		{
			Expression *rhs = ParseAndExpression(stream, status);
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
Expression *Parser::ParseAndExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseEqualityExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_AND);
		while (token != 0)
		{
			Expression *rhs = ParseEqualityExpression(stream, status);
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
Expression *Parser::ParseEqualityExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseRelationalExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::EQUALITY_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseRelationalExpression(stream, status);
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
Expression *Parser::ParseRelationalExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseShiftExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::RELATIONAL_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseShiftExpression(stream, status);
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
Expression *Parser::ParseShiftExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseAdditiveExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::SHIFT_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseAdditiveExpression(stream, status);
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
Expression *Parser::ParseAdditiveExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseMultiplicativeExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ADDITIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseMultiplicativeExpression(stream, status);
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
Expression *Parser::ParseMultiplicativeExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParseCastExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseCastExpression(stream, status);
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
Expression *Parser::ParseCastExpression(TokenStream &stream, Status &status)
{
	Expression *expression = 0;

	if (stream.NextIf(Token::KEY_CAST))
	{
		ExpectToken(stream, Token::OP_LT);
		TypeDescriptor *descriptor = ParseTypeDescriptor(stream, status);
		AssertNode(descriptor, stream);
		ExpectToken(stream, Token::OP_GT);
		ExpectToken(stream, Token::OPAREN);
		Expression *rhs = ParseCastExpression(stream, status);
		AssertNode(rhs, stream);
		ExpectToken(stream, Token::CPAREN);
		expression = mFactory.CreateCastExpression(descriptor, rhs);
	}
	else
	{
		expression = ParseUnaryExpression(stream, status);
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
Expression *Parser::ParseUnaryExpression(TokenStream &stream, Status &status)
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
				rhs = ParseUnaryExpression(stream, status);
				AssertNonConstExpression(status, INCREMENT_IN_CONST_EXPRESSION, op);
				break;

			default:
				rhs = ParseCastExpression(stream, status);
				break;
		}

		AssertNode(rhs, stream);
		expression = mFactory.CreateUnaryExpression(op, rhs);
	}

	else if (stream.NextIf(Token::KEY_SIZEOF) != 0)
	{
		Expression *unary = ParseUnaryExpression(stream, status);
		if (unary != 0)
		{
			expression = mFactory.CreateSizeofExpression(unary);
		}
		else
		{
			ExpectToken(stream, Token::OPAREN);
			TypeDescriptor *descriptor = ParseTypeDescriptor(stream, status);
			AssertNode(descriptor, stream);
			ExpectToken(stream, Token::CPAREN);
			expression = mFactory.CreateSizeofExpression(descriptor);
		}
	}

	else
	{
		expression = ParsePostfixExpression(stream, status);
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
Expression *Parser::ParsePostfixExpression(TokenStream &stream, Status &status)
{
	Expression *expression = ParsePrimaryExpression(stream, status);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::POSTFIX_OPERATORS);
		while (token != 0)
		{
			switch (token->GetTokenType())
			{
				case Token::OBRACKET:
				{
					Expression *index = ParseExpression(stream, status);
					expression = mFactory.CreateArraySubscriptExpression(expression, index);
					ExpectToken(stream, Token::CBRACKET);
				}
				break;

				case Token::OPAREN:
				{
					Expression *argumentList = ParseArgumentList(stream, status);
					expression = mFactory.CreateFunctionCallExpression(expression, argumentList);
					AssertNonConstExpression(status, FUNCTION_CALL_IN_CONST_EXPRESSION, token);
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
					AssertNonConstExpression(status, INCREMENT_IN_CONST_EXPRESSION, token);
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
Expression *Parser::ParsePrimaryExpression(TokenStream &stream, Status &status)
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
		expression = ParseExpression(stream, status);
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
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(stream, status);
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
Expression *Parser::ParseArgumentList(TokenStream &stream, Status &status)
{
	Expression *head = ParseExpression(stream, status);
	Expression *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Expression *next = ParseExpression(stream, status);
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


void Parser::AssertNonConstExpression(Status &status, ErrorType errorType, const Token *token)
{
	if (status.ShouldParseConstExpressions())
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
