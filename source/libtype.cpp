#include "bond/api/libtype.h"
#include "bond/vm/vm.h"
#include <cctype>

namespace Bond
{

// The <cctype> functions have undefined behavior for negative values other than EOF. Bond chars are
// signed, so bytes of multi-byte UTF-8 characters are negative. Classify ASCII only, so that the
// results are also independent of the locale.
static bool IsAscii(int c)
{
	return (c >= 0) && (c <= 0x7F);
}


void IsAlnum(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isalnum(c) != 0);
	frame.SetReturnValue(result);
}


void IsAlpha(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isalpha(c) != 0);
	frame.SetReturnValue(result);
}


void IsBlank(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isblank(c) != 0);
	frame.SetReturnValue(result);
}


void IsCntrl(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (iscntrl(c) != 0);
	frame.SetReturnValue(result);
}


void IsDigit(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isdigit(c) != 0);
	frame.SetReturnValue(result);
}


void IsGraph(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isgraph(c) != 0);
	frame.SetReturnValue(result);
}


void IsLower(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (islower(c) != 0);
	frame.SetReturnValue(result);
}


void IsPrint(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isprint(c) != 0);
	frame.SetReturnValue(result);
}


void IsPunct(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (ispunct(c) != 0);
	frame.SetReturnValue(result);
}


void IsSpace(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isspace(c) != 0);
	frame.SetReturnValue(result);
}


void IsUpper(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isupper(c) != 0);
	frame.SetReturnValue(result);
}


void IsXDigit(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const bool result = IsAscii(c) && (isxdigit(c) != 0);
	frame.SetReturnValue(result);
}


void ToLower(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = IsAscii(c) ? tolower(c) : c;
	frame.SetReturnValue(result);
}


void ToUpper(Bond::StackFrame &frame)
{
	const int c = frame.GetArg<int>(0);
	const int result = IsAscii(c) ? toupper(c) : c;
	frame.SetReturnValue(result);
}

}
