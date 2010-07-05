#include "bond/charstream.h"

namespace Bond
{

void CharStream::SetBuffer(const char *buffer, int length)
{
	mBuffer = buffer;
	mLength = length;
	mPos = StreamPos();
}


bool CharStream::HasNext() const
{
	// Use <= instead of < since we're artificially introducing a space at the end to
	// ensure that the end of the last token is properly identified.
	return mPos.index <= mLength;
}


char CharStream::Next()
{
	const char c = Peek();
	Advance();
	return c;
}


char CharStream::Peek() const
{
	// Artificially introduce a space as the last character to ensure that the end
	// of the last token is properly identified.
	return (mPos.index >= mLength) ? ' ' : mBuffer[mPos.index];
}


void CharStream::Advance()
{
	const char c = Peek();
	++mPos.index;
	++mPos.column;

	if (c == '\n')
	{
		++mPos.line;
		mPos.column = 1;
	}
}


void CharStream::Unget()
{
	if (mPos.index > 0)
	{
		--mPos.index;

		if (mPos.column >= 1)
		{
			--mPos.column;
		}
	}
}


void CharStream::Unget(int numChars)
{
	for (int i = 0; i < numChars; ++i)
	{
		Unget();
	}
}

}
