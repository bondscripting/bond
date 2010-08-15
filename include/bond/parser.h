#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parsenodes.h"

namespace Bond
{

class TokenStream;

class Parser
{
public:
	Parser() {}
	~Parser() {}

	void Parse(TokenStream &stream);

private:
	TranslationUnit *ParseTranslationUnit(TokenStream &stream) const;
	ExternalDeclaration *ParseExternalDeclarationList(TokenStream &stream) const;
	ExternalDeclaration *ParseExternalDeclaration(TokenStream &stream) const;
	NamespaceDefinition *ParseNamespaceDefinition(TokenStream &stream) const;
	EnumDeclaration *ParseEnumDeclaration(TokenStream &stream) const;
	Enumerator *ParseEnumeratorList(TokenStream &stream) const;
	Enumerator *ParseEnumerator(TokenStream &stream) const;
};

}

#endif
