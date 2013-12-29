#include "bond/io/stdiotextwriter.h"
#include <stdio.h>

namespace Bond
{

void StdioTextWriter::VWrite(const char *format, va_list argList)
{
	vfprintf(mFile, format, argList);
}

}
