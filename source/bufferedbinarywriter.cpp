#include "bond/bufferedbinarywriter.h"
#include "bond/math.h"

namespace Bond
{

BufferedBinaryWriter::BufferedBinaryWriter(bu8_t *buffer, int length):
	mBuffer(buffer),
	mLength(length),
	mPosition(0),
	mEnd(0)
{
}


void BufferedBinaryWriter::Write(bu8_t c)
{
	if (mPosition < mLength)
	{
		mBuffer[mPosition++] = c;
		mEnd = Max(mPosition, mEnd);
	}
}


void BufferedBinaryWriter::SetPosition(int offset)
{
	const int position = Min(Max(offset, 0), mLength);
	mPosition = position;
	mEnd = Max(position, mEnd);
}


void BufferedBinaryWriter::SetPositionFromEnd(int offset)
{
	SetPosition(mEnd - offset);
}


void BufferedBinaryWriter::AddOffset(int offset)
{
	SetPosition(mPosition + offset);
}

}
