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
	mTranslationUnit = ParseTranslationUnit(status, stream);
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(Status &status, TokenStream &stream)
{
	ExternalDeclaration *declarations = ParseExternalDeclarationList(status, stream);
	TranslationUnit *unit = mFactory.CreateTranslationUnit(declarations);
	return unit;
}


ExternalDeclaration *Parser::ParseExternalDeclarationList(Status &status, TokenStream &stream)
{
	ExternalDeclaration *head = ParseExternalDeclaration(status, stream);
	ExternalDeclaration *current = head;

	while (current != 0)
	{
		ExternalDeclaration *next = ParseExternalDeclaration(status, stream);
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
ExternalDeclaration *Parser::ParseExternalDeclaration(Status &status, TokenStream &stream)
{
	ExternalDeclaration *declaration = 0;

	switch (stream.Peek()->GetTokenType())
	{
		case Token::KEY_NAMESPACE:
			declaration = ParseNamespaceDefinition(status, stream);
			break;

		case Token::KEY_ENUM:
			declaration = ParseEnumDeclaration(status, stream);
			break;

		case Token::KEY_STRUCT:
			declaration = ParseStructDeclaration(status, stream);
			break;

		default:
		{
			// TODO: move into a separate function once const_declarative_statement is figured out.
			TypeDescriptor *descriptor = ParseTypeDescriptor(status, stream);

			// Could be a function declaration, function definition or a constant declarative statement.
			if (descriptor != 0)
			{
				const Token *name = ExpectToken(status, stream, Token::IDENTIFIER);
				if (stream.NextIf(Token::OPAREN) != 0)
				{
					Parameter *parameterList = ParseParameterList(status, stream);
					ExpectToken(status, stream, Token::CPAREN);
					FunctionPrototype *prototype = mFactory.CreateFunctionPrototype(name, descriptor, parameterList);
					CompoundStatement *body = ParseCompoundStatement(status, stream);
					if (body == 0)
					{
						ExpectToken(status, stream, Token::SEMICOLON);
					}
					declaration = mFactory.CreateFunctionDefinition(prototype, body);
				}
				else
				{
					// TODO: Do something or we'll memory leak the type descriptor.
				}
			}
		}
		break;
	}

	return declaration;
}


// namespace_definition
//   : NAMESPACE IDENTIFIER '{' external_declaration* '}'
NamespaceDefinition *Parser::ParseNamespaceDefinition(Status &status, TokenStream &stream)
{
	NamespaceDefinition *space = 0;

	if (stream.NextIf(Token::KEY_NAMESPACE) != 0)
	{
		const Token *name = ExpectToken(status, stream, Token::IDENTIFIER);
		ExpectToken(status, stream, Token::OBRACE);
		ExternalDeclaration *declarations = ParseExternalDeclarationList(status, stream);
		ExpectToken(status, stream, Token::CBRACE);
		space = mFactory.CreateNamespaceDefinition(name, declarations);
	}

	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
//
// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
EnumDeclaration *Parser::ParseEnumDeclaration(Status &status, TokenStream &stream)
{
	EnumDeclaration *enumeration = 0;

	if (stream.NextIf(Token::KEY_ENUM) != 0)
	{
		const Token *name = ExpectToken(status, stream, Token::IDENTIFIER);
		ExpectToken(status, stream, Token::OBRACE);
		Enumerator *enumeratorList = 0;
		Enumerator *current = 0;

		while (stream.PeekIf(TokenTypeSet::BLOCK_DELIMITERS) == 0)
		{
			Enumerator *next = ParseEnumerator(status, stream);
			AssertNode(status, stream, next);
			SyncToEnumeratorDelimiter(status, stream);

			// Note that the comma on the last enumerator is optional.
			if (stream.PeekIf(TokenTypeSet::BLOCK_DELIMITERS) == 0)
			{
				ExpectToken(status, stream, Token::COMMA);
			}

			if (next != 0)
			{
				if (enumeratorList == 0)
				{
					enumeratorList = next;
				}
				else
				{
					current->SetNext(next);
				}
				current = next;
			}
		}

		ExpectToken(status, stream, Token::CBRACE);
		ExpectToken(status, stream, Token::SEMICOLON);
		enumeration = mFactory.CreateEnumDeclaration(name, enumeratorList);
	}

	return enumeration;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *Parser::ParseEnumerator(Status &status, TokenStream &stream)
{
	Enumerator *enumerator = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		Expression *value = 0;
		if (stream.NextIf(Token::ASSIGN) != 0)
		{
			value = ParseConstExpression(status, stream);
			AssertNode(status, stream, value);
		}
		enumerator = mFactory.CreateEnumerator(name, value);
	}

	return enumerator;
}


// parameter_list
//   : parameter
//   | parameter_list ',' parameter
Parameter *Parser::ParseParameterList(Status &status, TokenStream &stream)
{
	Parameter *head = ParseParameter(status, stream);
	Parameter *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Parameter *next = ParseParameter(status, stream);
		AssertNode(status, stream, next);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// parameter
//   : type_descriptor IDENTIFIER
Parameter *Parser::ParseParameter(Status &status, TokenStream &stream)
{
	Parameter *parameter = 0;
	TypeDescriptor *descriptor = ParseTypeDescriptor(status, stream);

	if (descriptor != 0)
	{
		const Token *name = ExpectToken(status, stream, Token::IDENTIFIER);
		parameter = mFactory.CreateParameter(name, descriptor);
	}

	return parameter;
}


// struct_declaration
//   : STRUCT IDENTIFIER '{' struct_member_declaration+ '}' ';'
StructDeclaration *Parser::ParseStructDeclaration(Status &status, TokenStream &stream)
{
	// TODO:
	return 0;
}


// type_descriptor
//   : CONST type_specifier
//   | type_specifier [CONST]
//   | type_descriptor '*' [CONST]
//   | type_descriptor '[' [const_expression] ']'
TypeDescriptor *Parser::ParseRelaxedTypeDescriptor(Status &status, TokenStream &stream)
{
	Status overrideStatus(status);
	overrideStatus.ParseRelaxedTypeDescriptors();
	return ParseTypeDescriptor(overrideStatus, stream);
}
TypeDescriptor *Parser::ParseTypeDescriptor(Status &status, TokenStream &stream)
{
	TypeDescriptor *descriptor = 0;
	const int pos = stream.GetPosition();
	const bool isConst1 = stream.NextIf(Token::KEY_CONST) != 0;
	TypeSpecifier *specifier = ParseTypeSpecifier(status, stream);

	if (specifier != 0)
	{
		const Token *const2 = stream.NextIf(Token::KEY_CONST);
		const bool isConst2 = const2 != 0;
		if (isConst1 && isConst2)
		{
			PushError(status, DUPLICATE_CONST, const2);
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
				Expression *length = status.IsParsingRelaxedTypeDescriptors() ?
					ParseExpression(status, stream) : ParseConstExpression(status, stream);
				ExpectToken(status, stream, Token::CBRACKET);
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
TypeSpecifier *Parser::ParseTypeSpecifier(Status &status, TokenStream &stream)
{
	TypeSpecifier *specifier = ParsePrimitiveTypeSpecifier(status, stream);

	if (specifier == 0)
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(status, stream);
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
TypeSpecifier *Parser::ParsePrimitiveTypeSpecifier(Status &status, TokenStream &stream)
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
NamedInitializer *Parser::ParseNamedInitializerList(Status &status, TokenStream &stream)
{
	NamedInitializer *head = ParseNamedInitializer(status, stream);
	NamedInitializer *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		NamedInitializer *next = ParseNamedInitializer(status, stream);
		AssertNode(status, stream, next);
		current->SetNext(next);
		current = next;
	}

	return head;
}


// named_initializer
//   : IDENTIFIER ['=' initializer]
NamedInitializer *Parser::ParseNamedInitializer(Status &status, TokenStream &stream)
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
QualifiedIdentifier *Parser::ParseQualifiedIdentifier(Status &status, TokenStream &stream)
{
	QualifiedIdentifier *id = 0;
	const Token *name = stream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		id = mFactory.CreateQualifiedIdentifier(name);
		if (stream.NextIf(Token::SCOPE))
		{
			QualifiedIdentifier *next = ParseQualifiedIdentifier(status, stream);
			AssertNode(status, stream, next);
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
Statement *Parser::ParseStatement(Status &status, TokenStream &stream)
{
	Statement *statement = 0;

	switch (stream.Peek()->GetTokenType())
	{
		case Token::OBRACE:
			statement = ParseCompoundStatement(status, stream);
			break;

		case Token::KEY_IF:
			statement = ParseIfStatement(status, stream);
			break;

		case Token::KEY_SWITCH:
			statement = ParseSwitchStatement(status, stream);
			break;

		case Token::KEY_WHILE:
			statement = ParseWhileStatement(status, stream);
			break;

		case Token::KEY_DO:
			statement = ParseDoWhileStatement(status, stream);
			break;

		case Token::KEY_BREAK:
		case Token::KEY_CONTINUE:
		case Token::KEY_RETURN:
			statement = ParseJumpStatement(status, stream);
			break;

		default:
			statement = ParseDeclarativeOrExpressionStatement(status, stream);
			break;
	}

	return statement;
}


// compound_statement
//   : '{' statement* '}'
CompoundStatement *Parser::ParseCompoundStatement(Status &status, TokenStream &stream)
{
	CompoundStatement *compoundStatement = 0;

	if (stream.NextIf(Token::OBRACE))
	{
		Statement *statementList = 0;
		Statement *current = 0;

		while (stream.PeekIf(TokenTypeSet::BLOCK_DELIMITERS) == 0)
		{
			Statement *next = ParseStatement(status, stream);
			AssertNode(status, stream, next);
			SyncToStatementDelimiter(status, stream);

			if (next != 0)
			{
				if (statementList == 0)
				{
					statementList = next;
				}
				else
				{
					current->SetNext(next);
				}
				current = next;
			}
		}

		ExpectToken(status, stream, Token::CBRACE);
		compoundStatement = mFactory.CreateCompoundStatement(statementList);
	}

	return compoundStatement;
}


// if_statement
//   : IF '(' expression ')' statement [ELSE statement]
IfStatement *Parser::ParseIfStatement(Status &status, TokenStream &stream)
{
	IfStatement *ifStatement = 0;

	if (stream.NextIf(Token::KEY_IF) != 0)
	{
		ExpectToken(status, stream, Token::OPAREN);
		Expression *condition = ParseExpression(status, stream);
		AssertNode(status, stream, condition);
		ExpectToken(status, stream, Token::CPAREN);
		Statement *thenStatement = ParseStatement(status, stream);
		AssertNode(status, stream, thenStatement);
		Statement *elseStatement = 0;

		if (stream.NextIf(Token::KEY_ELSE))
		{
			elseStatement = ParseStatement(status, stream);
			AssertNode(status, stream, elseStatement);
		}

		ifStatement = mFactory.CreateIfStatement(condition, thenStatement, elseStatement);
	}

	return ifStatement;
}


// switch_statement
//   : SWITCH '(' expression ')' '{' switch_section* '}'
SwitchStatement *Parser::ParseSwitchStatement(Status &status, TokenStream &stream)
{
	SwitchStatement *switchStatement = 0;

	if (stream.NextIf(Token::KEY_SWITCH) != 0)
	{
		ExpectToken(status, stream, Token::OPAREN);
		Expression *control = ParseExpression(status, stream);
		AssertNode(status, stream, control);
		ExpectToken(status, stream, Token::CPAREN);
		ExpectToken(status, stream, Token::OBRACE);

		SwitchSection *sectionList = ParseSwitchSection(status, stream);
		SwitchSection *current = sectionList;
		while (stream.PeekIf(TokenTypeSet::BLOCK_DELIMITERS) == 0)
		{
			SwitchSection *next = ParseSwitchSection(status, stream);
			current->SetNext(next);
			current = next;
		}
		// TODO: Semantic analyser must ensure the list is not empty.

		ExpectToken(status, stream, Token::CBRACE);
		switchStatement = mFactory.CreateSwitchStatement(control, sectionList);
	}

	return switchStatement;
}


// switch_section
//   : switch_label+ statement+
SwitchSection *Parser::ParseSwitchSection(Status &status, TokenStream &stream)
{
	SwitchSection *section = 0;
	SwitchLabel *labelList = ParseSwitchLabel(status, stream);
	SwitchLabel *currentLabel = labelList;
	while (currentLabel != 0)
	{
		SwitchLabel *next = ParseSwitchLabel(status, stream);
		currentLabel->SetNext(next);
		currentLabel = next;
	}
	// TODO: Semantic analyser must ensure the list is not empty.

	Statement *statementList = 0;
	Statement *currentStatement = 0;
	while (stream.PeekIf(TokenTypeSet::SWITCH_SECTION_DELIMITERS) == 0)
	{
		Statement *next = ParseStatement(status, stream);
		AssertNode(status, stream, next);
		SyncToStatementDelimiter(status, stream);

		if (next != 0)
		{
			if (statementList == 0)
			{
				statementList = next;
			}
			else
			{
				currentStatement->SetNext(next);
			}
			currentStatement = next;
		}
	}
	// TODO: Semantic analyser must ensure the list is not empty.

	section = mFactory.CreateSwitchSection(labelList, statementList);;

	return section;
}


// switch_label
//   : CASE const_expression ':'
//   | DEFAULT ':'
SwitchLabel *Parser::ParseSwitchLabel(Status &status, TokenStream &stream)
{
	SwitchLabel *label = 0;
	const Token *labelToken = stream.NextIf(TokenTypeSet::SWITCH_LABELS);

	if (labelToken != 0)
	{
		if (labelToken->GetTokenType() == Token::KEY_CASE)
		{
			Expression *expression = ParseConstExpression(status, stream);
			AssertNode(status, stream, expression);
			label = mFactory.CreateSwitchLabel(labelToken, expression);
		}
		else
		{
			label = mFactory.CreateDefaultLabel(labelToken);
		}

		SyncToLabelTerminator(status, stream);
	}

	return label;
}


// while_statement
//   : WHILE '(' expression ')' statement
WhileStatement *Parser::ParseWhileStatement(Status &status, TokenStream &stream)
{
	WhileStatement *whileStatement = 0;

	if (stream.NextIf(Token::KEY_WHILE) != 0)
	{
		ExpectToken(status, stream, Token::OPAREN);
		Expression *condition = ParseExpression(status, stream);
		AssertNode(status, stream, condition);
		ExpectToken(status, stream, Token::CPAREN);
		Statement *body = ParseStatement(status, stream);
		AssertNode(status, stream, body);
		whileStatement = mFactory.CreateWhileStatement(condition, body);
	}

	return whileStatement;
}


// do_statement
//   : DO statement WHILE '(' expression ')' ';'
WhileStatement *Parser::ParseDoWhileStatement(Status &status, TokenStream &stream)
{
	WhileStatement *whileStatement = 0;

	if (stream.NextIf(Token::KEY_DO) != 0)
	{
		Statement *body = ParseStatement(status, stream);
		AssertNode(status, stream, body);
		ExpectToken(status, stream, Token::KEY_WHILE);
		ExpectToken(status, stream, Token::OPAREN);
		Expression *condition = ParseExpression(status, stream);
		AssertNode(status, stream, condition);
		ExpectToken(status, stream, Token::CPAREN);
		ExpectToken(status, stream, Token::SEMICOLON);
		whileStatement = mFactory.CreateDoWhileStatement(condition, body);
	}

	return whileStatement;
}


// jump_statement
//   : CONTINUE ';'
//   | BREAK ';'
//   | RETURN [expression] ';'
JumpStatement *Parser::ParseJumpStatement(Status &status, TokenStream &stream)
{
	JumpStatement *jumpStatement = 0;
	const Token *op = stream.NextIf(TokenTypeSet::JUMP_OPERATORS);

	if (op != 0)
	{
		Expression *rhs = 0;
		if (op->GetTokenType() == Token::KEY_RETURN)
		{
			rhs = ParseExpression(status, stream);
		}
		SyncToStatementTerminator(status, stream);
		jumpStatement = mFactory.CreateJumpStatement(op, rhs);
	}

	return jumpStatement;
}


// declarative_statement
//   : type_descriptor named_initializer_list ';'
Statement *Parser::ParseDeclarativeOrExpressionStatement(Status &status, TokenStream &stream)
{
	Statement *statement = 0;
	const int startPos = stream.GetPosition();

	// The grammar is somewhat ambiguous. Since a qualified identifier followed by '*' tokens and array
	// index operators can appear like a type descriptor as well as an expression, we'll treat anything
	// that fits the profile of a declaration as such and everything else like an expression statement.
	TypeDescriptor *descriptor = ParseRelaxedTypeDescriptor(status, stream);
	if (descriptor != 0)
	{
		NamedInitializer *initializerList = ParseNamedInitializerList(status, stream);

		if (initializerList != 0)
		{
			statement = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
			SyncToStatementTerminator(status, stream);
		}
		else
		{
			// The tokens that looked like a type descriptor, might actually be part of an expression.
			const int descriptorPos = stream.GetPosition();
			mFactory.DestroyHierarchy(descriptor);
			descriptor = 0;
			stream.SetPosition(startPos);

			statement = ParseExpressionStatement(status, stream);

			if (statement == 0)
			{
				// Uh, oh. Looks like we're even worse off.
				stream.SetPosition(descriptorPos);
				AssertNode(status, stream, statement);
				SyncToStatementTerminator(status, stream);
			}
		}
	}

	else
	{
		statement = ParseExpressionStatement(status, stream);
	}

	return statement;
}


// expression_statement
//   : [expression] ';'
ExpressionStatement *Parser::ParseExpressionStatement(Status &status, TokenStream &stream)
{
	ExpressionStatement *expressionStatement = 0;
	Expression *expression = ParseExpression(status, stream);

	if (expression != 0)
	{
		SyncToStatementTerminator(status, stream);
		expressionStatement = mFactory.CreateExpressionStatement(expression);
	}
	else if (stream.NextIf(Token::SEMICOLON))
	{
		expressionStatement = mFactory.CreateExpressionStatement(0);
	}

	return expressionStatement;
}


// const_expression
//   : conditional_expression
//
// expression
//   : assignment_expression
//   | expression ',' assignment_expression
Expression *Parser::ParseConstExpression(Status &status, TokenStream &stream)
{
	Status overrideStatus(status);
	overrideStatus.ParseConstExpressions();
	return ParseExpression(overrideStatus, stream);
}
Expression *Parser::ParseExpression(Status &status, TokenStream &stream)
{
	Expression *expression = 0;

	if (status.IsParsingConstExpressions())
	{
		expression = ParseConditionalExpression(status, stream);
	}
	else
	{
		expression = ParseAssignmentExpression(status, stream);
 
		if (expression != 0)
 		{
			const Token *token = stream.NextIf(Token::COMMA);
			while (token != 0)
			{
				Expression *rhs = ParseAssignmentExpression(status, stream);
				AssertNode(status, stream, rhs);
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
Expression *Parser::ParseAssignmentExpression(Status &status, TokenStream &stream)
{
	// TODO: This function can produce pretty much any type of expression on the lhs of an assignment.
	// Will need to do further analysis in the semantic analyser to ensure validity.
	Expression *expression = ParseConditionalExpression(status, stream);

	if (expression == 0)
	{
		expression = ParseUnaryExpression(status, stream);
	}

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ASSIGNMENT_OPERATORS);
		if (token != 0)
		{
			AssertNonConstExpression(status, ASSIGNMENT_IN_CONST_EXPRESSION, token);
			Expression *rhs = ParseAssignmentExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
		}
	}

	return expression;
}


// conditional_expression
//   : logical_or_expression
//   | logical_or_expression '?' expression ':' conditional_expression
Expression *Parser::ParseConditionalExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseLogicalOrExpression(status, stream);

	if ((expression != 0) && (stream.NextIf(Token::OP_TERNARY) != 0))
	{
		Expression *trueExpression = ParseExpression(status, stream);
		AssertNode(status, stream, trueExpression);
		ExpectToken(status, stream, Token::COLON);
		Expression *falseExpression = ParseConditionalExpression(status, stream);
		AssertNode(status, stream, falseExpression);
		expression = mFactory.CreateConditionalExpression(expression, trueExpression, falseExpression);
	}

	return expression;
}


// logical_or_expression
//   : logical_and_expression
//   | logical_or_expression '||' logical_and_expression
Expression *Parser::ParseLogicalOrExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseLogicalAndExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_OR);
		while (token != 0)
		{
			Expression *rhs = ParseLogicalAndExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_OR);
		}
	}

	return expression;
}


// logical_and_expression
//   : inclusive_or_expression
//   | logical_and_expression '&&' inclusive_or_expression
Expression *Parser::ParseLogicalAndExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseInclusiveOrExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_AND);
		while (token != 0)
		{
			Expression *rhs = ParseInclusiveOrExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_AND);
		}
	}

	return expression;
}


// inclusive_or_expression
//   : exclusive_or_expression
//   | inclusive_or_expression '|' exclusive_or_expression
Expression *Parser::ParseInclusiveOrExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseExclusiveOrExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_OR);
		while (token != 0)
		{
			Expression *rhs = ParseExclusiveOrExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_BIT_OR);
		}
	}

	return expression;
}


// exclusive_or_expression
//   : and_expression
//   | exclusive_or_expression '^' and_expression
Expression *Parser::ParseExclusiveOrExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseAndExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_XOR);
		while (token != 0)
		{
			Expression *rhs = ParseAndExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::OP_BIT_XOR);
		}
	}

	return expression;
}


// and_expression
//   : equality_expression
//   | and_expression '&' equality_expression
Expression *Parser::ParseAndExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseEqualityExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(Token::OP_BIT_AND);
		while (token != 0)
		{
			Expression *rhs = ParseEqualityExpression(status, stream);
			AssertNode(status, stream, rhs);
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
Expression *Parser::ParseEqualityExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseRelationalExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::EQUALITY_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseRelationalExpression(status, stream);
			AssertNode(status, stream, rhs);
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
Expression *Parser::ParseRelationalExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseShiftExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::RELATIONAL_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseShiftExpression(status, stream);
			AssertNode(status, stream, rhs);
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
Expression *Parser::ParseShiftExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseAdditiveExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::SHIFT_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseAdditiveExpression(status, stream);
			AssertNode(status, stream, rhs);
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
Expression *Parser::ParseAdditiveExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseMultiplicativeExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::ADDITIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseMultiplicativeExpression(status, stream);
			AssertNode(status, stream, rhs);
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
Expression *Parser::ParseMultiplicativeExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParseCastExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		while (token != 0)
		{
			Expression *rhs = ParseCastExpression(status, stream);
			AssertNode(status, stream, rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = stream.NextIf(TokenTypeSet::MULTIPLICATIVE_OPERATORS);
		}
	}

	return expression;
}


// cast_expression
//   : unary_expression
//   | CAST '<' type_descriptor '>' '(' cast_expression ')'
Expression *Parser::ParseCastExpression(Status &status, TokenStream &stream)
{
	Expression *expression = 0;

	if (stream.NextIf(Token::KEY_CAST))
	{
		ExpectToken(status, stream, Token::OP_LT);
		TypeDescriptor *descriptor = ParseTypeDescriptor(status, stream);
		AssertNode(status, stream, descriptor);
		ExpectToken(status, stream, Token::OP_GT);
		ExpectToken(status, stream, Token::OPAREN);
		Expression *rhs = ParseCastExpression(status, stream);
		AssertNode(status, stream, rhs);
		ExpectToken(status, stream, Token::CPAREN);
		expression = mFactory.CreateCastExpression(descriptor, rhs);
	}
	else
	{
		expression = ParseUnaryExpression(status, stream);
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
//   | SIZEOF '<' type_descriptor '>'
Expression *Parser::ParseUnaryExpression(Status &status, TokenStream &stream)
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
				rhs = ParseUnaryExpression(status, stream);
				AssertNonConstExpression(status, INCREMENT_IN_CONST_EXPRESSION, op);
				break;

			default:
				rhs = ParseCastExpression(status, stream);
				break;
		}

		AssertNode(status, stream, rhs);
		expression = mFactory.CreateUnaryExpression(op, rhs);
	}

	else if (stream.NextIf(Token::KEY_SIZEOF) != 0)
	{
		if (stream.NextIf(Token::OP_LT) != 0)
		{
			TypeDescriptor *descriptor = ParseTypeDescriptor(status, stream);
			AssertNode(status, stream, descriptor);
			ExpectToken(status, stream, Token::OP_GT);
			expression = mFactory.CreateSizeofExpression(descriptor);
		}
		else
		{
			Expression *unary = ParseUnaryExpression(status, stream);
			AssertNode(status, stream, unary);
			expression = mFactory.CreateSizeofExpression(unary);
		}
	}

	else
	{
		expression = ParsePostfixExpression(status, stream);
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
Expression *Parser::ParsePostfixExpression(Status &status, TokenStream &stream)
{
	Expression *expression = ParsePrimaryExpression(status, stream);

	if (expression != 0)
	{
		const Token *token = stream.NextIf(TokenTypeSet::POSTFIX_OPERATORS);
		while (token != 0)
		{
			switch (token->GetTokenType())
			{
				case Token::OBRACKET:
				{
					Expression *index = ParseExpression(status, stream);
					expression = mFactory.CreateArraySubscriptExpression(expression, index);
					ExpectToken(status, stream, Token::CBRACKET);
				}
				break;

				case Token::OPAREN:
				{
					Expression *argumentList = ParseArgumentList(status, stream);
					expression = mFactory.CreateFunctionCallExpression(expression, argumentList);
					AssertNonConstExpression(status, FUNCTION_CALL_IN_CONST_EXPRESSION, token);
					ExpectToken(status, stream, Token::CPAREN);
				}
				break;

				case Token::OP_ARROW:
				case Token::PERIOD:
				{
					const Token *memberName = ExpectToken(status, stream, Token::IDENTIFIER);
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
Expression *Parser::ParsePrimaryExpression(Status &status, TokenStream &stream)
{
	Expression *expression = 0;
	const Token *value = stream.NextIf(TokenTypeSet::CONSTANT_VALUES);

	if (value != 0)
	{
		expression = mFactory.CreateConstantExpression(value);
	}
	else if (stream.NextIf(Token::OPAREN) != 0)
	{
		expression = ParseExpression(status, stream);
		AssertNode(status, stream, expression);
		ExpectToken(status, stream, Token::CPAREN);
	}
	else
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier(status, stream);
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
Expression *Parser::ParseArgumentList(Status &status, TokenStream &stream)
{
	Expression *head = ParseExpression(status, stream);
	Expression *current = head;

	while ((current != 0) && (stream.NextIf(Token::COMMA) != 0))
	{
		Expression *next = ParseExpression(status, stream);
		AssertNode(status, stream, next);
		current->SetNext(next);
		current = next;
	}

	return head;
}


void Parser::SyncToEnumeratorDelimiter(Status &status, TokenStream &stream)
{
	Recover(status, stream, TokenTypeSet::ENUMERATOR_DELIMITERS);
}


void Parser::SyncToStatementTerminator(Status &status, TokenStream &stream)
{
	SyncToStatementDelimiter(status, stream);
	ExpectToken(status, stream, Token::SEMICOLON);
	SyncToStatementDelimiter(status, stream);
}


void Parser::SyncToStatementDelimiter(Status &status, TokenStream &stream)
{
	Recover(status, stream, TokenTypeSet::STATEMENT_DELIMITERS);
}


void Parser::SyncToLabelTerminator(Status &status, TokenStream &stream)
{
	SyncToLabelDelimiter(status, stream);
	ExpectToken(status, stream, Token::COLON);
	SyncToLabelDelimiter(status, stream);
}


void Parser::SyncToLabelDelimiter(Status &status, TokenStream &stream)
{
	Recover(status, stream, TokenTypeSet::LABEL_DELIMITERS);
}


void Parser::Recover(
	Status &status,
	TokenStream &stream,
	const TokenTypeSet &delimiters) const
{
	if (status.HasUnrecoveredError())
	{
		stream.SkipTo(delimiters);
		status.RecoverFromError();
	}
}


const Token *Parser::ExpectToken(Status &status, TokenStream &stream, Token::TokenType expectedType)
{
	const Token *token = stream.NextIf(expectedType);
	if (token == 0)
	{
		PushError(status, UNEXPECTED_TOKEN, stream.Peek(), Token::GetTokenName(expectedType));
	}
	return token;
}


const Token *Parser::ExpectToken(Status &status, TokenStream &stream, const TokenTypeSet &expectedTypes)
{
	const Token *token = stream.NextIf(expectedTypes);
	if (token == 0)
	{
		PushError(status, UNEXPECTED_TOKEN, stream.Peek(), expectedTypes.typeName);
	}
	return token;
}


void Parser::AssertNode(Status &status, const TokenStream &stream, ParseNode *node)
{
	if (node == 0)
	{
		PushError(status, PARSE_ERROR, stream.Peek());
	}
}


void Parser::AssertNonConstExpression(Status &status, ErrorType errorType, const Token *context)
{
	if (status.IsParsingConstExpressions())
	{
		PushError(status, errorType, context);
	}
}


void Parser::PushError(Status &status, ErrorType type, const Token *context, const char *expected)
{
	if (!status.HasUnrecoveredError() && (mNumErrors < MAX_ERRORS))
	{
		mErrors[mNumErrors] = Error(type, context, expected);
		++mNumErrors;
		status.MarkError();
	}
}

}
