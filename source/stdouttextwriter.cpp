#include "bond/stdouttextwriter.h"
#include <stdio.h>
#include <stdarg.h>

namespace Bond
{

void StdoutTextWriter::Write(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	vprintf(format, argList);
	va_end(argList);
}

}
