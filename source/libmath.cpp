#include "bond/api/libmath.h"
#include "bond/vm/vm.h"
#include <cmath>

namespace Bond
{

void Sin(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = sin(x);
	frame.SetReturnValue(y);
}


void Sinf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = sinf(x);
	frame.SetReturnValue(y);
}


void Cos(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = cos(x);
	frame.SetReturnValue(y);
}


void Cosf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = cosf(x);
	frame.SetReturnValue(y);
}


void Tan(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = tan(x);
	frame.SetReturnValue(y);
}


void Tanf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = tanf(x);
	frame.SetReturnValue(y);
}


void Asin(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = asin(x);
	frame.SetReturnValue(y);
}


void Asinf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = asinf(x);
	frame.SetReturnValue(y);
}


void Acos(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = acos(x);
	frame.SetReturnValue(y);
}


void Acosf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = acosf(x);
	frame.SetReturnValue(y);
}


void Atan(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = atan(x);
	frame.SetReturnValue(y);
}


void Atanf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = atanf(x);
	frame.SetReturnValue(y);
}


void Atan2(Bond::StackFrame &frame)
{
	const auto y = frame.GetArg<double>(0);
	const auto x = frame.GetArg<double>(1);
	const auto result = atan2(y, x);
	frame.SetReturnValue(result);
}


void Atan2f(Bond::StackFrame &frame)
{
	const auto y = frame.GetArg<float>(0);
	const auto x = frame.GetArg<float>(1);
	const auto result = atan2f(y, x);
	frame.SetReturnValue(result);
}


void Sinh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = sinh(x);
	frame.SetReturnValue(y);
}


void Sinhf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = sinhf(x);
	frame.SetReturnValue(y);
}


void Cosh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = cosh(x);
	frame.SetReturnValue(y);
}


void Coshf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = coshf(x);
	frame.SetReturnValue(y);
}


void Tanh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = tanh(x);
	frame.SetReturnValue(y);
}


void Tanhf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = tanhf(x);
	frame.SetReturnValue(y);
}


void Asinh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = asinh(x);
	frame.SetReturnValue(y);
}


void Asinhf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = asinhf(x);
	frame.SetReturnValue(y);
}


void Acosh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = acosh(x);
	frame.SetReturnValue(y);
}


void Acoshf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = acoshf(x);
	frame.SetReturnValue(y);
}


void Atanh(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = atanh(x);
	frame.SetReturnValue(y);
}


void Atanhf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = atanhf(x);
	frame.SetReturnValue(y);
}


void Exp(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = exp(x);
	frame.SetReturnValue(y);
}


void Expf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = expf(x);
	frame.SetReturnValue(y);
}


void Log(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = log(x);
	frame.SetReturnValue(y);
}


void Logf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = logf(x);
	frame.SetReturnValue(y);
}


void Log10(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = log10(x);
	frame.SetReturnValue(y);
}


void Log10f(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = log10f(x);
	frame.SetReturnValue(y);
}


void Pow(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = pow(x, y);
	frame.SetReturnValue(result);
}


void Powf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = powf(x, y);
	frame.SetReturnValue(result);
}


void Sqrt(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = sqrt(x);
	frame.SetReturnValue(y);
}


void Sqrtf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = sqrtf(x);
	frame.SetReturnValue(y);
}


void Ceil(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = ceil(x);
	frame.SetReturnValue(y);
}


void Ceilf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = ceilf(x);
	frame.SetReturnValue(y);
}


void Floor(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = floor(x);
	frame.SetReturnValue(y);
}


void Floorf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = floorf(x);
	frame.SetReturnValue(y);
}


void Fabs(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = fabs(x);
	frame.SetReturnValue(y);
}


void Fabsf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = fabsf(x);
	frame.SetReturnValue(y);
}


void Ldexp(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = ldexp(x, n);
	frame.SetReturnValue(result);
}


void Ldexpf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = ldexpf(x, n);
	frame.SetReturnValue(result);
}


void Frexp(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	int exp;
	const auto result = frexp(x, &exp);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(1) = int32_t(exp);
}


void Frexpf(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	int exp;
	const auto result = frexpf(x, &exp);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(1) = int32_t(exp);
}


void Modf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	double ip;
	const double fp = modf(x, &ip);
	frame.SetReturnValue(double(fp));
	*frame.GetArg<double *>(1) = double(ip);
}


void Modff(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	double ip;
	const double fp = modf(x, &ip);
	frame.SetReturnValue(float(fp));
	*frame.GetArg<float *>(1) = float(ip);
}


void Fmod(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = double(frame.GetArg<double>(1));
	const double result = fmod(x, y);
	frame.SetReturnValue(double(result));
}


void Fmodf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = double(frame.GetArg<float>(1));
	const double result = fmod(x, y);
	frame.SetReturnValue(float(result));
}

}
