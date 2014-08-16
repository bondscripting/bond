#include "bond/api/libio.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"
#include <cstdio>

namespace Bond
{

void PrintStr(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	printf("%s", str);
}


void PrintB(Bond::CalleeStackFrame &frame)
{
	const bool value = frame.GetArg<bool>(0);
	printf("%s", value ? "true" : "false");
}


void PrintC(Bond::CalleeStackFrame &frame)
{
	const bi8_t value = frame.GetArg<bi8_t>(0);
	printf("%c", char(value));
}


void PrintI(Bond::CalleeStackFrame &frame)
{
	const bi32_t value = frame.GetArg<bi32_t>(0);
	printf("%" BOND_PRId32, value);
}


void PrintUI(Bond::CalleeStackFrame &frame)
{
	const bu32_t value = frame.GetArg<bu32_t>(0);
	printf("%" BOND_PRIu32, value);
}


void PrintL(Bond::CalleeStackFrame &frame)
{
	const bi64_t value = frame.GetArg<bi64_t>(0);
	printf("%" BOND_PRId64, value);
}


void PrintUL(Bond::CalleeStackFrame &frame)
{
	const bu64_t value = frame.GetArg<bu64_t>(0);
	printf("%" BOND_PRIu64, value);
}


void PrintF(Bond::CalleeStackFrame &frame)
{
	const bf32_t value = frame.GetArg<bf32_t>(0);
	printf("%" BOND_PRIf32, value);
}


void PrintD(Bond::CalleeStackFrame &frame)
{
	const bf64_t value = frame.GetArg<bf64_t>(0);
	printf("%" BOND_PRIf64, value);
}

}
