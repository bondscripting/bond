#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"

namespace Bond
{

class ParseErrorBuffer;
class Token;
class TokenStream;

class Parser
{
public:
	Parser(Allocator &allocator, ParseErrorBuffer &errorBuffer);
	~Parser();
	void Dispose();

	void Parse(TokenStream &stream);

	TranslationUnit *GetTranslationUnitList() { return mTranslationUnitList; }

private:
	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	ParseNodeFactory mFactory;
	ParseErrorBuffer &mErrorBuffer;
	TranslationUnit *mTranslationUnitList;
};

}

#endif
