#include "bond/io/inputstream.h"

namespace Bond
{

void InputStream::Scan(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	VScan(format, argList);
	va_end(argList);
}


size_t InputStream::Read(uint8_t *bytes, size_t numBytes)
{
	size_t i = 0;
	while (i < numBytes)
	{
		const int value = Read();
		if (value == Eof)
		{
			break;
		}
		bytes[i++] = uint8_t(value);
	}
	return numBytes;
}

}
