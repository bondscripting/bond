#include "bond/api/libmath.h"
#include "bond/vm/vm.h"
#include <math.h>

namespace Bond
{

void Sin(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = sin(x);
	frame.SetReturnValue(bf64_t(y));
}


void Sinf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const float x = float(frame.GetArg<bf32_t>(0));
	const float y = sinf(x);
	frame.SetReturnValue(bf32_t(y));
}


void Cos(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = cos(x);
	frame.SetReturnValue(bf64_t(y));
}


void Cosf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const float x = float(frame.GetArg<bf32_t>(0));
	const float y = cosf(x);
	frame.SetReturnValue(bf32_t(y));
}

}
