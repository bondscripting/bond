#include "bond/exception.h"
#include <stdarg.h>
#include <stdio.h>

namespace Bond
{

Exception::Exception(const char *message)
{
	strncpy(mMessage, message, MESSAGE_BUFFER_LENGTH);
	mMessage[MESSAGE_BUFFER_LENGTH - 1] = '\0';
}


Exception::Exception(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	vsnprintf(mMessage, MESSAGE_BUFFER_LENGTH, format, argList);
	va_end(argList);
	mMessage[MESSAGE_BUFFER_LENGTH - 1] = '\0';
}

}

