#include "bond/stl/cstdio.h"
#include "bond/systems/exception.h"
#include <cstdarg>

namespace Bond
{

Exception::Exception(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	Bond::vsnprintf(mMessage, MESSAGE_BUFFER_LENGTH, format, argList);
	va_end(argList);
	mMessage[MESSAGE_BUFFER_LENGTH - 1] = '\0';
}

}
