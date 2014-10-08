#include "bond/api/libstring.h"
#include "bond/vm/vm.h"
#include <cstdio>
#include <cstring>

namespace Bond
{

void Strlen(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	const bu32_t length = bu32_t(strlen(str));
	frame.SetReturnValue(length);
}


void TryParseInt(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	bi32_t *result = frame.GetArg<bi32_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		success = sscanf(str, "%" BOND_SCNd32, result) == 1;
	}
	frame.SetReturnValue(success);
}


void TryParseLong(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	bi64_t *result = frame.GetArg<bi64_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		success = sscanf(str, "%" BOND_SCNd64, result) == 1;
	}
	frame.SetReturnValue(success);
}


void TryParseFloat(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	float *result = frame.GetArg<float *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		success = sscanf(str, "%f", result) == 1;
	}
	frame.SetReturnValue(success);
}


void TryParseDouble(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	double *result = frame.GetArg<double *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		success = sscanf(str, "%lf", result) == 1;
	}
	frame.SetReturnValue(success);
}

}
