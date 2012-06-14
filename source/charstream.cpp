#include "bond/charstream.h"

namespace Bond
{

void CharStream::SetBuffer(const char *buffer, size_t length)
{
	mBuffer = buffer;
	mLength = length;
	mPos = StreamPos();
}


void CharStream::Reset()
{
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

	if (mPos.index <= mLength)
	{
		++mPos.index;
		++mPos.column;

		if (c == '\n')
		{
			++mPos.line;
			mPos.column = 1;
		}
	}

	return c;
}


char CharStream::Peek(size_t index) const
{
	// Artificially introduce a space as the last character to ensure that the end
	// of the last token is properly identified.
	return (index >= mLength) ? ' ' : mBuffer[index];
}


void CharStream::Unget(size_t numChars)
{
	const size_t delta = (numChars < 0) ? 0 : (numChars > mPos.index) ? mPos.index : numChars;
	const size_t oldIndex = mPos.index;
	mPos.index -= delta;
	mPos.column -= delta;

	size_t numNewLines = 0;

	for (size_t i = mPos.index; i < oldIndex; ++i)
	{
		if (Peek(i) == '\n')
		{
			++numNewLines;
		}
	}

	if (numNewLines > 0)
	{
		mPos.line -= numNewLines;
		mPos.column = 1;
		for (size_t i = mPos.index; (i > 0) && (Peek(i - 1) != '\n'); --i)
		{
			++mPos.column;
		}
	}
}

}
