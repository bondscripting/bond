#include "bond/io/outputstream.h"

namespace Bond
{

void OutputStream::Print(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	VPrint(format, argList);
	va_end(argList);
}


void OutputStream::Write(const uint8_t *bytes, size_t numBytes)
{
	for (size_t i = 0; i < numBytes; ++i)
	{
		Write(bytes[i]);
	}
}

}
