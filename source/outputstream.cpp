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

}
