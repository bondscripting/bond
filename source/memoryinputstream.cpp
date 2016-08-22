#include "bond/io/memoryinputstream.h"
#include "bond/systems/assert.h"
#include "bond/systems/math.h"
#include <cstring>
#include <cstdio>

namespace Bond
{

MemoryInputStream::MemoryInputStream(const void *buffer, pos_t size):
	mBuffer(static_cast<const uint8_t *>(buffer)),
	mSize(size),
	mCurrent(0)
{}


void MemoryInputStream::VScan(const char *format, va_list argList)
{
	BOND_FAIL_MESSAGE("MemoryInputStream::VScan is not implemented.");
}


size_t MemoryInputStream::Read(uint8_t *bytes, size_t numBytes)
{
	const size_t n = Min(numBytes, size_t(mSize - mCurrent));
	memcpy(bytes, mBuffer + mCurrent, n);
	mCurrent += pos_t(n);
	return n;
}


int MemoryInputStream::Read()
{
	int value = Eof;
	if (mCurrent < mSize)
	{
		value = int(mBuffer[mCurrent++]);
	}
	return value;
}


void MemoryInputStream::SetPosition(off_t offset)
{
	const pos_t current = Min(Max(pos_t(offset), pos_t(0)), mSize);
	mCurrent = current;
}


void MemoryInputStream::SetPositionFromEnd(off_t offset)
{
	SetPosition(off_t(mSize - offset));
}


void MemoryInputStream::AddOffset(off_t offset)
{
	SetPosition(off_t(mCurrent + offset));
}

}
