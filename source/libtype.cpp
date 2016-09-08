#include "bond/api/libtype.h"
#include "bond/vm/vm.h"
#include <cctype>

namespace Bond
{

void IsAlnum(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isalnum(c) != 0;
	frame.SetReturnValue(result);
}


void IsAlpha(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isalpha(c) != 0;
	frame.SetReturnValue(result);
}


void IsCntrl(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = iscntrl(c) != 0;
	frame.SetReturnValue(result);
}


void IsDigit(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isdigit(c) != 0;
	frame.SetReturnValue(result);
}


void IsGraph(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isgraph(c) != 0;
	frame.SetReturnValue(result);
}


void IsLower(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = islower(c) != 0;
	frame.SetReturnValue(result);
}


void IsPrint(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isprint(c) != 0;
	frame.SetReturnValue(result);
}


void IsPunct(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = ispunct(c) != 0;
	frame.SetReturnValue(result);
}


void IsSpace(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isspace(c) != 0;
	frame.SetReturnValue(result);
}


void IsUpper(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isupper(c) != 0;
	frame.SetReturnValue(result);
}


void IsXDigit(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = isxdigit(c) != 0;
	frame.SetReturnValue(result);
}


void ToLower(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = tolower(c);
	frame.SetReturnValue(result);
}


void ToUpper(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = toupper(c);
	frame.SetReturnValue(result);
}

}
