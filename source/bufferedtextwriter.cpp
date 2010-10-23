#include "bond/bufferedtextwriter.h"
#include <stdio.h>
#include <stdarg.h>

namespace Bond
{

BufferedTextWriter::BufferedTextWriter(char *buffer, int length):
	mBuffer(buffer),
	mLength(length),
	mIndex(0)
{
	mBuffer[0] = '\0';
}


void BufferedTextWriter::Write(const char *format, ...)
{
	int length = mLength - mIndex;
	char *buffer = mBuffer + mIndex;
	va_list argList;
	va_start(argList, format);
	int n = vsnprintf(buffer, length, format, argList);
	va_end(argList);

	if (n > 0)
	{
		mIndex += n;
		if (mIndex >= mLength)
		{
			mIndex = mLength - 1;
		}
	}
}

}
