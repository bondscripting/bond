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
	bool HasNext() const { return mIndex < mLength; }
	const Token *Next();
	const Token *Peek() const { return Peek(mIndex); }
	void Unget() { Unget(1); }
	void Unget(int numTokens) { mIndex -= (numTokens < 0) ? 0 : (numTokens > mIndex) ? mIndex : numTokens; }
	int GetPosition() const { return mIndex; }
	void SetPosition(int index) { mIndex = (index < 0) ? 0 : (index > mIndex) ? mIndex : index; }
	int GetLength() const { return mLength; }

private:
	const Token *Peek(int index) const { return (index > mLength) ? 0 : mBuffer + index; }

	const Token *mBuffer;
	int mLength;
	int mIndex;
};

}

#endif
