#include "bond/api/libstring.h"
#include "bond/vm/vm.h"
#include <string.h>

namespace Bond
{

void Strlen(Bond::CalleeStackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	const bu32_t length = bu32_t(strlen(str));
	frame.SetReturnValue(length);
}

}