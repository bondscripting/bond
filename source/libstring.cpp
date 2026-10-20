#include "bond/api/libstring.h"
#include "bond/vm/vm.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Bond
{

void Strlen(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	const uint32_t length = uint32_t(strlen(str));
	frame.SetReturnValue(length);
}


void Strcmp(Bond::StackFrame &frame)
{
	const char *str1 = frame.GetArg<const char *>(0);
	const char *str2 = frame.GetArg<const char *>(1);
	const uint32_t result = uint32_t(strcmp(str1, str2));
	frame.SetReturnValue(result);
}


void TryParseInt(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	int32_t *result = frame.GetArg<int32_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		char *end;
		errno = 0;
		const long long value = strtoll(str, &end, 10);
		success = (end != str) && (errno != ERANGE) && (value >= BOND_INT_MIN) && (value <= BOND_INT_MAX);
		if (success)
		{
			*result = int32_t(value);
		}
	}
	frame.SetReturnValue(success);
}


void TryParseLong(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	int64_t *result = frame.GetArg<int64_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		char *end;
		errno = 0;
		const long long value = strtoll(str, &end, 10);
		success = (end != str) && (errno != ERANGE);
		if (success)
		{
			*result = int64_t(value);
		}
	}
	frame.SetReturnValue(success);
}


void TryParseFloat(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	float *result = frame.GetArg<float *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		char *end;
		const float value = strtof(str, &end);
		success = end != str;
		if (success)
		{
			*result = value;
		}
	}
	frame.SetReturnValue(success);
}


void TryParseDouble(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	double *result = frame.GetArg<double *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		char *end;
		const double value = strtod(str, &end);
		success = end != str;
		if (success)
		{
			*result = value;
		}
	}
	frame.SetReturnValue(success);
}

}
