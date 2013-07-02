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
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = sin(x);
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
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = cos(x);
	frame.SetReturnValue(bf32_t(y));
}


void Tan(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = tan(x);
	frame.SetReturnValue(bf64_t(y));
}


void Tanf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = tan(x);
	frame.SetReturnValue(bf32_t(y));
}


void Asin(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = asin(x);
	frame.SetReturnValue(bf64_t(y));
}


void Asinf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = asin(x);
	frame.SetReturnValue(bf32_t(y));
}


void Acos(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = acos(x);
	frame.SetReturnValue(bf64_t(y));
}


void Acosf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = acos(x);
	frame.SetReturnValue(bf32_t(y));
}


void Atan(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = atan(x);
	frame.SetReturnValue(bf64_t(y));
}


void Atanf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = atan(x);
	frame.SetReturnValue(bf32_t(y));
}


void Atan2(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double y = double(frame.GetArg<bf64_t>(0));
	const double x = double(frame.GetArg<bf64_t>(1));
	const double result = atan2(y, x);
	frame.SetReturnValue(bf64_t(result));
}


void Atan2f(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double y = double(frame.GetArg<bf32_t>(0));
	const double x = double(frame.GetArg<bf32_t>(1));
	const double result = atan2f(y, x);
	frame.SetReturnValue(bf32_t(result));
}


void Sinh(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = sinh(x);
	frame.SetReturnValue(bf64_t(y));
}


void Sinhf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = sinh(x);
	frame.SetReturnValue(bf32_t(y));
}


void Cosh(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = cosh(x);
	frame.SetReturnValue(bf64_t(y));
}


void Coshf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = cosh(x);
	frame.SetReturnValue(bf32_t(y));
}


void Tanh(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = tanh(x);
	frame.SetReturnValue(bf64_t(y));
}


void Tanhf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = tanh(x);
	frame.SetReturnValue(bf32_t(y));
}


void Exp(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = exp(x);
	frame.SetReturnValue(bf64_t(y));
}


void Expf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = exp(x);
	frame.SetReturnValue(bf32_t(y));
}


void Log(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = log(x);
	frame.SetReturnValue(bf64_t(y));
}


void Logf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = log(x);
	frame.SetReturnValue(bf32_t(y));
}


void Log10(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = log10(x);
	frame.SetReturnValue(bf64_t(y));
}


void Log10f(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = log10(x);
	frame.SetReturnValue(bf32_t(y));
}


void Pow(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = double(frame.GetArg<bf64_t>(1));
	const double result = pow(x, y);
	frame.SetReturnValue(bf64_t(result));
}


void Powf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = double(frame.GetArg<bf32_t>(1));
	const double result = pow(x, y);
	frame.SetReturnValue(bf32_t(result));
}


void Sqrt(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = sqrt(x);
	frame.SetReturnValue(bf64_t(y));
}


void Sqrtf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = sqrt(x);
	frame.SetReturnValue(bf32_t(y));
}


void Ceil(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = ceil(x);
	frame.SetReturnValue(bf64_t(y));
}


void Ceilf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = ceil(x);
	frame.SetReturnValue(bf32_t(y));
}


void Floor(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = floor(x);
	frame.SetReturnValue(bf64_t(y));
}


void Floorf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = floor(x);
	frame.SetReturnValue(bf32_t(y));
}


void Fabs(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = fabs(x);
	frame.SetReturnValue(bf64_t(y));
}


void Fabsf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = fabs(x);
	frame.SetReturnValue(bf32_t(y));
}


void Ldexp(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const int n = int(frame.GetArg<bi32_t>(1));
	const double result = ldexp(x, n);
	frame.SetReturnValue(bf64_t(result));
}


void Ldexpf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const int n = int(frame.GetArg<bi32_t>(1));
	const double result = ldexp(x, n);
	frame.SetReturnValue(bf32_t(result));
}


void Frexp(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	int exp;
	const double result = frexp(x, &exp);
	frame.SetReturnValue(bf64_t(result));
	*frame.GetArg<bi32_t *>(1) = bi32_t(exp);
}


void Frexpf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	int exp;
	const double result = frexp(x, &exp);
	frame.SetReturnValue(bf32_t(result));
	*frame.GetArg<bi32_t *>(1) = bi32_t(exp);
}


void Modf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	double ip;
	const double fp = modf(x, &ip);
	frame.SetReturnValue(bf64_t(fp));
	*frame.GetArg<bf64_t *>(1) = bf64_t(ip);
}


void Modff(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	double ip;
	const double fp = modf(x, &ip);
	frame.SetReturnValue(bf32_t(fp));
	*frame.GetArg<bf32_t *>(1) = bf32_t(ip);
}


void Fmod(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf64_t>(0));
	const double y = double(frame.GetArg<bf64_t>(1));
	const double result = fmod(x, y);
	frame.SetReturnValue(bf64_t(result));
}


void Fmodf(Bond::VM &vm)
{
	VM::CalleeStackFrame &frame = vm.GetTopStackFrame();
	const double x = double(frame.GetArg<bf32_t>(0));
	const double y = double(frame.GetArg<bf32_t>(1));
	const double result = fmod(x, y);
	frame.SetReturnValue(bf32_t(result));
}

}
