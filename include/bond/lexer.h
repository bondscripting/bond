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
	Lexer(Allocator &allocator):
		mAllocator(allocator),
		mTokenCollectionList(NULL)
	{}

	~Lexer();
	void Dispose();

	TokenCollection *Lex(const char *text, int length);
	TokenCollection *Lex(const unsigned char *text, int length) { return Lex(reinterpret_cast<const char *>(text), length); }
	const TokenCollection *GetTokenCollectionList() const { return mTokenCollectionList; }

private:
	Allocator &mAllocator;
	TokenCollection *mTokenCollectionList;
};

}

#endif
