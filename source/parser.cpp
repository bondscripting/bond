#include "bond/parser.h"
#include "bond/tokenstream.h"

#include <stdio.h>

namespace Bond
{

Parser::Parser():
	mNumErrors(0)
{
	for (int i = 0; i < MAX_ERRORS; ++i)
	{
		mErrors[i] = Error();
	}
}


void Parser::Parse(TokenStream &stream)
{
	ParseTranslationUnit(stream);
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(TokenStream &stream)
{
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);
	TranslationUnit *unit = new TranslationUnit(declarations);
	return unit;
}


// external_declaration
//   : namespace_definition
//   | function_definition
//   | function_declaration
//   | struct_declaration
//   | enum_declaration
//   | const_declaration
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
	printf("Parsed namespace: '%s'\n", (name == 0) ? "<INVALID>" : name->GetText());
	NamespaceDefinition *space = new NamespaceDefinition(name, declarations);
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

	printf("Parsed enum: '%s'\n", (name == 0) ? "<INVALID>" : name->GetText());
	EnumDeclaration *enumeration = new EnumDeclaration(name, enumerators);
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
		if (stream.NextIf(Token::COMMA) == NULL)
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
	printf("Parsed enumerator: '%s'%s\n", name->GetText(), (value == 0) ? "" : " with initializer");

	Enumerator *enumerator = new Enumerator(name, value);
	return enumerator;
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
				expression = new BinaryExpression(token, expression, rhs);
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
	Expression *expression = new BinaryExpression(op, lhs, rhs);
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
		expression = new ConditionalExpression(expression, trueExpression, falseExpression);
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
			expression = new BinaryExpression(token, expression, rhs);
			token = stream.NextIf(Token::COMMA);
		}
	}
	return expression;
}


// logical_and_expression
//   : inclusive_or_expression
//   | logical_and_expression '&&' inclusive_or_expression
Expression *Parser::ParseLogicalAndExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	return 0;
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
//   | SIZEOF '(' type_name ')'
Expression *Parser::ParseUnaryExpression(TokenStream &stream, ExpressionQualifier qualifier)
{
	return 0;
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
