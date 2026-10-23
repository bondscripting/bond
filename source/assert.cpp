#include "bond/systems/assert.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace Bond
{

void AssertionFailed(const char *file, int line, const char *condition, const char *format, ...)
{
	fprintf(stderr, "%s(%d): Assertion failed", file, line);
	if (condition != nullptr)
	{
		fprintf(stderr, ": %s", condition);
	}
	if (format != nullptr)
	{
		fprintf(stderr, ": ");
		va_list argList;
		va_start(argList, format);
		vfprintf(stderr, format, argList);
		va_end(argList);
	}
	fprintf(stderr, "\n");
	fflush(stderr);
	abort();
}

}
