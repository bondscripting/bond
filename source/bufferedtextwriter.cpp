#include "bond/bufferedtextwriter.h"
#include <stdarg.h>
#include <stdio.h>

namespace Bond
{

BufferedTextWriter::BufferedTextWriter(char *buffer, size_t length):
	mBuffer(buffer),
	mLength(length),
	mIndex(0)
{
	mBuffer[0] = '\0';
}


void BufferedTextWriter::Write(const char *format, ...)
{
	size_t length = mLength - mIndex;
	char *buffer = mBuffer + mIndex;
	va_list argList;
	va_start(argList, format);
	const int n = vsnprintf(buffer, length, format, argList);
	va_end(argList);

	if (n > 0)
	{
		mIndex += size_t(n);
		if (mIndex >= mLength)
		{
			mIndex = mLength - 1;
			mBuffer[mLength -1] = '\0';
		}
	}
}

}
