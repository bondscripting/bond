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
	printf("Entering: Parse\n");
	ParseTranslationUnit(stream);
	printf("Leaving: Parse\n");
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(TokenStream &stream)
{
	printf("Entering: ParseTranslationUnit\n");
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);
	TranslationUnit *unit = new TranslationUnit(declarations);
	printf("Leaving: ParseTranslationUnit\n");
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
	printf("Entering: ParseExternalDeclarationList\n");
	ExternalDeclaration *head = ParseExternalDeclaration(stream);
	ExternalDeclaration *current = head;

	while (current != 0)
	{
		ExternalDeclaration *next = ParseExternalDeclaration(stream);
		current->SetNext(next);
		current = next;
	}
	printf("Leaving: ParseExternalDeclarationList\n");

	return head;
}


ExternalDeclaration *Parser::ParseExternalDeclaration(TokenStream &stream)
{
	printf("Entering: ParseExternalDeclaration\n");
	ExternalDeclaration *declaration = ParseNamespaceDefinition(stream);
	if (declaration == 0)
	{
		declaration = ParseEnumDeclaration(stream);
	}
	printf("Leaving: ParseExternalDeclaration\n");
	return declaration;
}


// namespace_definition
//   : NAMESPACE IDENTIFIER '{' external_declaration* '}'
NamespaceDefinition *Parser::ParseNamespaceDefinition(TokenStream &stream)
{
	printf("Entering: ParseNamespaceDefinition\n");
	if (stream.TestNext(Token::KEY_NAMESPACE) == 0)
	{
		return 0;
	}

	const Token *name = ExpectToken(stream, Token::IDENTIFIER);
	ExpectToken(stream, Token::OBRACE);
	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);
	ExpectToken(stream, Token::CBRACE);
	printf("Parsed namespace: '%s'\n", (name == 0) ? "<INVALID>" : name->GetText());
	NamespaceDefinition *space = new NamespaceDefinition(name, declarations);
	printf("Leaving: ParseNamespaceDefinition\n");
	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
EnumDeclaration *Parser::ParseEnumDeclaration(TokenStream &stream)
{
	printf("Entering: ParseEnumDeclaration\n");
	if (stream.TestNext(Token::KEY_ENUM) == 0)
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
	printf("Leaving: ParseEnumDeclaration\n");
	return enumeration;
}


// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
Enumerator *Parser::ParseEnumeratorList(TokenStream &stream)
{
	printf("Entering: ParseEnumeratorList\n");
	Enumerator *head = ParseEnumerator(stream);
	Enumerator *current = head;

	while (current != 0)
	{
		if (stream.TestNext(Token::COMMA) == NULL)
		{
			break;
		}
		Enumerator *next = ParseEnumerator(stream);
		current->SetNext(next);
		current = next;
	}
	printf("Leaving: ParseEnumeratorList\n");

	return head;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *Parser::ParseEnumerator(TokenStream &stream)
{
	printf("Entering: ParseEnumerator\n");
	const Token *name = stream.TestNext(Token::IDENTIFIER);
	if (name == 0)
	{
		return 0;
	}

	int_t value = 0;
	if (stream.TestNext(Token::ASSIGN) != 0)
	{
		// TODO: parse a const_expression.
		if (stream.TestPeek(Token::CONST_INT))
		{
			value = stream.Next()->GetIntValue();
		}
		else if (stream.TestPeek(Token::CONST_UINT))
		{
			value = static_cast<int_t>(stream.Next()->GetUIntValue());
		}
	}
	printf("Parsed enumerator: '%s' = '%d'\n", name->GetText(), value);

	Enumerator *enumerator = new Enumerator(name, value);
	printf("Leaving: ParseEnumerator\n");
	return enumerator;
}


const Token *Parser::ExpectToken(TokenStream &stream, Token::TokenType expectedType)
{
	const Token *token = stream.TestNext(expectedType);
	if (token == 0)
	{
		PushError(UNEXPECTED_TOKEN, expectedType, stream.Peek());
	}
	return token;
}

void Parser::PushError(ErrorType type, Token::TokenType expectedType, const Token *token)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = Error(type, expectedType, token);
		++mNumErrors;
	}
}

}
