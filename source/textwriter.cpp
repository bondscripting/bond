#include "bond/io/textwriter.h"
#include <stdio.h>

namespace Bond
{

void TextWriter::Write(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	VWrite(format, argList);
	va_end(argList);
}

}
