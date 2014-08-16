#include "bond/api/libtype.h"
#include "bond/vm/vm.h"
#include <cctype>

namespace Bond
{

void IsAlnum(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isalnum(c) != 0;
	frame.SetReturnValue(result);
}


void IsAlpha(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isalpha(c) != 0;
	frame.SetReturnValue(result);
}


void IsCntrl(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = iscntrl(c) != 0;
	frame.SetReturnValue(result);
}


void IsDigit(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isdigit(c) != 0;
	frame.SetReturnValue(result);
}


void IsGraph(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isgraph(c) != 0;
	frame.SetReturnValue(result);
}


void IsLower(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = islower(c) != 0;
	frame.SetReturnValue(result);
}


void IsPrint(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isprint(c) != 0;
	frame.SetReturnValue(result);
}


void IsPunct(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = ispunct(c) != 0;
	frame.SetReturnValue(result);
}


void IsSpace(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isspace(c) != 0;
	frame.SetReturnValue(result);
}


void IsUpper(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isupper(c) != 0;
	frame.SetReturnValue(result);
}


void IsXDigit(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isxdigit(c) != 0;
	frame.SetReturnValue(result);
}


void ToLower(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = tolower(c);
	frame.SetReturnValue(result);
}


void ToUpper(Bond::CalleeStackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = toupper(c);
	frame.SetReturnValue(result);
}

}
