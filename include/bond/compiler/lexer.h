#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/compiler/tokenstream.h"

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

	const CompilerErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

private:
	// Copying disallowed.
	Lexer(const Lexer &other);
	Lexer &operator=(const Lexer &other);

	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	TokenCollection *mTokenCollectionList;
};

}

#endif
