#include "bond/io/stdiotextwriter.h"
#include <stdio.h>
#include <stdarg.h>

namespace Bond
{

void StdioTextWriter::Write(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	vfprintf(mFile, format, argList);
	va_end(argList);
}

}
