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
		mLength(0)
	{
	}

	CharStream(const char *buffer, size_t length):
		mBuffer(buffer),
		mLength(length)
	{
	}

	void SetBuffer(const char *buffer, size_t length);
	void Reset();
	bool HasNext() const;
	char Next();
	char Peek() const { return Peek(mPos.index); }
	void Unget() { Unget(1); }
	void Unget(size_t numChars);

	const StreamPos &GetStreamPos() const { return mPos; }
	const char *GetBuffer() const { return mBuffer; }
	const size_t GetLength() const { return mLength; }

private:
	char Peek(size_t index) const;

	StreamPos mPos;
	const char *mBuffer;
	size_t mLength;
};

}

#endif
