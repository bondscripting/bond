#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/compiler/parsenodefactory.h"
#include "bond/compiler/parsenodes.h"

namespace Bond
{

class CompilerErrorBuffer;
class Token;
class TokenStream;

class Parser
{
public:
	Parser(Allocator &allocator, CompilerErrorBuffer &errorBuffer);
	~Parser();
	void Dispose();

	TranslationUnit *Parse(TokenStream &stream);

	TranslationUnit *GetTranslationUnitList() { return mTranslationUnitList; }

	const CompilerErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

private:
	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	ParseNodeFactory mFactory;
	CompilerErrorBuffer &mErrorBuffer;
	TranslationUnit *mTranslationUnitList;
};

}

#endif
