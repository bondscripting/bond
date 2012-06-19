#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/tokenstream.h"

namespace Bond
{

class Allocator;
class CharStream;
class CompilerErrorBuffer;
class StringAllocator;

class Lexer
{
public:
	Lexer(Allocator &allocator, CompilerErrorBuffer &errorBuffer):
		mAllocator(allocator),
		mErrorBuffer(errorBuffer),
		mTokenCollectionList(NULL)
	{}

	~Lexer();
	void Dispose();

	TokenCollection *Lex(const char *text, size_t length);
	const TokenCollection *GetTokenCollectionList() const { return mTokenCollectionList; }

private:
	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	TokenCollection *mTokenCollectionList;
};

}

#endif
