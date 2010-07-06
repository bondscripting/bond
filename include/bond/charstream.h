#ifndef BOND_CHARSTREAM_H
#define BOND_CHARSTREAM_H

#include "bond/streampos.h"

namespace Bond
{

class CharStream
{
public:
	CharStream():
		mBuffer(0),
		mLength(-1)
	{
	}

	CharStream(const char *buffer, int length):
		mBuffer(buffer),
		mLength(length)
	{
	}

	void SetBuffer(const char *buffer, int length);

	bool HasNext() const;
	char Next();
	char Peek() const;
	void Unget();
	void Unget(int numChars);

	const StreamPos &GetStreamPos() const { return mPos; }
	const char *GetBuffer() const { return mBuffer; }
	const int GetLength() const { return mLength; }

private:
	char Peek(int index) const;

	StreamPos mPos;
	const char *mBuffer;
	int mLength;
};

}

#endif