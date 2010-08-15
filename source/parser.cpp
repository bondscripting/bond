#include "bond/parser.h"
#include "bond/tokenstream.h"

#include <stdio.h>

namespace Bond
{

void Parser::Parse(TokenStream &stream)
{
	printf("Entering: Parse\n");
	ParseTranslationUnit(stream);
	printf("Leaving: Parse\n");
}


// translation_unit
//  : external_declaration*
TranslationUnit *Parser::ParseTranslationUnit(TokenStream &stream) const
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
ExternalDeclaration *Parser::ParseExternalDeclarationList(TokenStream &stream) const
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


ExternalDeclaration *Parser::ParseExternalDeclaration(TokenStream &stream) const
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
NamespaceDefinition *Parser::ParseNamespaceDefinition(TokenStream &stream) const
{
	printf("Entering: ParseNamespaceDefinition\n");
	const int pos = stream.GetPosition();
	const Token *token = stream.Next();

	if (token->GetTokenType() != Token::KEY_NAMESPACE)
	{
		stream.SetPosition(pos);
		return 0;
	}

	const Token *name = stream.Next();
	if (name->GetTokenType() != Token::IDENTIFIER)
	{
		stream.SetPosition(pos);
		return 0;
	}
	printf("Parsing namespace: '%s'\n", name->GetText());

	token = stream.Next();
	if (token->GetTokenType() != Token::OBRACE)
	{
		stream.SetPosition(pos);
		return 0;
	}

	ExternalDeclaration *declarations = ParseExternalDeclarationList(stream);

	token = stream.Next();
	if (token->GetTokenType() != Token::CBRACE)
	{
		stream.SetPosition(pos);
		return 0;
	}

	printf("Parsed namespace: '%s'\n", name->GetText());
	NamespaceDefinition *space = new NamespaceDefinition(name, declarations);
	printf("Leaving: ParseNamespaceDefinition\n");
	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
EnumDeclaration *Parser::ParseEnumDeclaration(TokenStream &stream) const
{
	printf("Entering: ParseEnumDeclaration\n");
	const int pos = stream.GetPosition();
	const Token *token = stream.Next();

	if (token->GetTokenType() != Token::KEY_ENUM)
	{
		stream.SetPosition(pos);
		return 0;
	}

	const Token *name = stream.Next();
	if (name->GetTokenType() != Token::IDENTIFIER)
	{
		stream.SetPosition(pos);
		return 0;
	}
	printf("Parsing enum: '%s'\n", name->GetText());

	token = stream.Next();
	if (token->GetTokenType() != Token::OBRACE)
	{
		stream.SetPosition(pos);
		return 0;
	}

	Enumerator *enumerators = ParseEnumeratorList(stream);

	token = stream.Next();
	if (token->GetTokenType() != Token::CBRACE)
	{
		stream.SetPosition(pos);
		return 0;
	}

	token = stream.Next();
	if (token->GetTokenType() != Token::SEMICOLON)
	{
		stream.SetPosition(pos);
		return 0;
	}

	printf("Parsed namespace: '%s'\n", name->GetText());
	EnumDeclaration *enumeration = new EnumDeclaration(name, enumerators);
	printf("Leaving: ParseEnumDeclaration\n");
	return enumeration;
}


// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
Enumerator *Parser::ParseEnumeratorList(TokenStream &stream) const
{
	printf("Entering: ParseEnumeratorList\n");
	Enumerator *head = ParseEnumerator(stream);
	Enumerator *current = head;

	while (current != 0)
	{
		const Token *token = stream.Next();
		if (token->GetTokenType() != Token::COMMA)
		{
			// TODO: Do something.
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
Enumerator *Parser::ParseEnumerator(TokenStream &stream) const
{
	printf("Entering: ParseEnumerator\n");
	int pos = stream.GetPosition();
	const Token *name = stream.Next();
	if (name->GetTokenType() != Token::IDENTIFIER)
	{
		stream.SetPosition(pos);
		return 0;
	}

	pos = stream.GetPosition();
	int_t value = 0;
	const Token *token = stream.Next();
	if (token->GetTokenType() == Token::ASSIGN)
	{
		token = stream.Next();
		if (token->GetTokenType() == Token::CONST_INT)
		{
			value = token->GetIntValue();
		}
		else if (token->GetTokenType() == Token::CONST_UINT)
		{
			value = static_cast<int_t>(token->GetUIntValue());
		}
		else
		{
			stream.SetPosition(pos);
		}
	}
	else
	{
		stream.SetPosition(pos);
	}
	printf("Parsed enumerator: '%s' = '%d'\n", name->GetText(), value);

	Enumerator *enumerator = new Enumerator(name, value);
	printf("Leaving: ParseEnumerator\n");
	return enumerator;
}

}
