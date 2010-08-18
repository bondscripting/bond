#ifndef BOND_TOKENSTREAM_H
#define BOND_TOKENSTREAM_H

#include "bond/token.h"

namespace Bond
{

class TokenStream
{
public:
	TokenStream():
		mBuffer(0),
		mLength(-1),
		mIndex(0)
	{
	}

	TokenStream(const Token *buffer, int length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{
	}

	void SetBuffer(const Token *buffer, int length);
	void Reset() { mIndex = 0; }

	void Advance() { SetPosition(mIndex + 1); }

	const Token *Next();
	const Token *TestNext(Token::TokenType type);

	void SkipTo(Token::TokenType type);
	void SkipTo(const Token::TokenType *types, int numTypes);

	const Token *Peek() const { return mBuffer + mIndex; }
	bool TestPeek(Token::TokenType type) const { return Peek()->GetTokenType() == type; }
	bool TestPeek(const Token::TokenType *types, int numTypes) const;

	int GetPosition() const { return mIndex; }
	void SetPosition(int index) { mIndex = ValidIndex(index); }
	int GetLength() const { return mLength; }

private:
	int ValidIndex(int index) const { return (index < 0) ? 0 : (index > mLength) ? (mLength - 1) : index; }

	const Token *mBuffer;
	int mLength;
	int mIndex;
};

}

#endif
