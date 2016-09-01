#include "bond/compiler/charstream.h"

namespace Bond
{

void CharStream::Reset(const StreamPos &pos)
{
	mPos = pos;
	mStream.SetPosition(Stream::pos_t(pos.index));
}


bool CharStream::HasNext() const
{
	// Use <= instead of < since we're artificially introducing a space at the end to
	// ensure that the end of the last token is properly identified.
	return mPos.index <= mEndPos;
}


char CharStream::Next()
{
	// Artificially introduce a space as the last character to ensure that the end
	// of the last token is properly identified.
	const char c = (mPos.index < mEndPos) ? char(mStream.Read()) : ' ';

	if (mPos.index <= mEndPos)
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

}
