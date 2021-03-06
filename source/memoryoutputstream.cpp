#include "bond/io/memoryoutputstream.h"
#include "bond/systems/math.h"
#include <cstring>
#include <cstdio>

namespace Bond
{

MemoryOutputStream::MemoryOutputStream(void *buffer, pos_t size):
	mBuffer(static_cast<uint8_t *>(buffer)),
	mSize(size),
	mCurrent(0),
	mEnd(0)
{
	mBuffer[0] = uint8_t('\0');
}


void MemoryOutputStream::VPrint(const char *format, va_list argList)
{
	pos_t size = mSize - mCurrent;
	char *buffer = reinterpret_cast<char *>(mBuffer + mCurrent);
	const int n = vsnprintf(buffer, size, format, argList);

	if (n > 0)
	{
		mCurrent += pos_t(n);
		if (mCurrent >= mSize)
		{
			mCurrent = mSize;
			mBuffer[mSize - 1] = uint8_t('\0');
		}
	}
}


void MemoryOutputStream::Write(uint8_t c)
{
	if (mCurrent < mSize)
	{
		mBuffer[mCurrent++] = c;
		mEnd = Max(mCurrent, mEnd);
	}
}


void MemoryOutputStream::Write(const uint8_t *bytes, size_t numBytes)
{
	const size_t n = Min(numBytes, size_t(mSize - mCurrent));
	memcpy(mBuffer + mCurrent, bytes, n);
	mCurrent += pos_t(n);
	mEnd = Max(mCurrent, mEnd);
}


void MemoryOutputStream::SetPosition(off_t offset)
{
	const pos_t current = Min(Max(pos_t(offset), pos_t(0)), mSize);
	mCurrent = current;
	mEnd = Max(current, mEnd);
}


void MemoryOutputStream::SetPositionFromEnd(off_t offset)
{
	SetPosition(off_t(mEnd - offset));
}


void MemoryOutputStream::AddOffset(off_t offset)
{
	SetPosition(off_t(mCurrent + offset));
}

}
