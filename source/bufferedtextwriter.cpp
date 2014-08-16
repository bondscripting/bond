#include "bond/io/bufferedtextwriter.h"
#include <cstdio>

namespace Bond
{

BufferedTextWriter::BufferedTextWriter(char *buffer, size_t length):
	mBuffer(buffer),
	mLength(length),
	mIndex(0)
{
	mBuffer[0] = '\0';
}


void BufferedTextWriter::VWrite(const char *format, va_list argList)
{
	size_t length = mLength - mIndex;
	char *buffer = mBuffer + mIndex;
	const int n = vsnprintf(buffer, length, format, argList);

	if (n > 0)
	{
		mIndex += size_t(n);
		if (mIndex >= mLength)
		{
			mIndex = mLength - 1;
			mBuffer[mLength - 1] = '\0';
		}
	}
}

}
