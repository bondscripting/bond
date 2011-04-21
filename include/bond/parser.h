#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parseerror.h"
#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"

namespace Bond
{

class Token;
class TokenStream;

class Parser
{
public:
	Parser(Allocator &allocator);
	~Parser();
	void Dispose();

	void Parse(TokenStream &stream);

	TranslationUnit *GetTranslationUnitList() { return mTranslationUnitList; }

	bool HasErrors() const { return mErrorBuffer.HasErrors(); }
	const ParseErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

private:
	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	ParseErrorBuffer mErrorBuffer;
	ParseNodeFactory mFactory;
	TranslationUnit *mTranslationUnitList;
};

}

#endif
