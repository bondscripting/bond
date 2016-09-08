#include "bond/api/libmath.h"
#include "bond/vm/vm.h"
#include <cmath>

namespace Bond
{

void Sin(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = sin(x);
	frame.SetReturnValue(double(y));
}


void Sinf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = sin(x);
	frame.SetReturnValue(float(y));
}


void Cos(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = cos(x);
	frame.SetReturnValue(double(y));
}


void Cosf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = cos(x);
	frame.SetReturnValue(float(y));
}


void Tan(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = tan(x);
	frame.SetReturnValue(double(y));
}


void Tanf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = tan(x);
	frame.SetReturnValue(float(y));
}


void Asin(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = asin(x);
	frame.SetReturnValue(double(y));
}


void Asinf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = asin(x);
	frame.SetReturnValue(float(y));
}


void Acos(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = acos(x);
	frame.SetReturnValue(double(y));
}


void Acosf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = acos(x);
	frame.SetReturnValue(float(y));
}


void Atan(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = atan(x);
	frame.SetReturnValue(double(y));
}


void Atanf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = atan(x);
	frame.SetReturnValue(float(y));
}


void Atan2(Bond::StackFrame &frame)
{
	const double y = double(frame.GetArg<double>(0));
	const double x = double(frame.GetArg<double>(1));
	const double result = atan2(y, x);
	frame.SetReturnValue(double(result));
}


void Atan2f(Bond::StackFrame &frame)
{
	const double y = double(frame.GetArg<float>(0));
	const double x = double(frame.GetArg<float>(1));
	const double result = atan2(y, x);
	frame.SetReturnValue(float(result));
}


void Sinh(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = sinh(x);
	frame.SetReturnValue(double(y));
}


void Sinhf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = sinh(x);
	frame.SetReturnValue(float(y));
}


void Cosh(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = cosh(x);
	frame.SetReturnValue(double(y));
}


void Coshf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = cosh(x);
	frame.SetReturnValue(float(y));
}


void Tanh(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = tanh(x);
	frame.SetReturnValue(double(y));
}


void Tanhf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = tanh(x);
	frame.SetReturnValue(float(y));
}


void Exp(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = exp(x);
	frame.SetReturnValue(double(y));
}


void Expf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = exp(x);
	frame.SetReturnValue(float(y));
}


void Log(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = log(x);
	frame.SetReturnValue(double(y));
}


void Logf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = log(x);
	frame.SetReturnValue(float(y));
}


void Log10(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = log10(x);
	frame.SetReturnValue(double(y));
}


void Log10f(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = log10(x);
	frame.SetReturnValue(float(y));
}


void Pow(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = double(frame.GetArg<double>(1));
	const double result = pow(x, y);
	frame.SetReturnValue(double(result));
}


void Powf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = double(frame.GetArg<float>(1));
	const double result = pow(x, y);
	frame.SetReturnValue(float(result));
}


void Sqrt(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = sqrt(x);
	frame.SetReturnValue(double(y));
}


void Sqrtf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = sqrt(x);
	frame.SetReturnValue(float(y));
}


void Ceil(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = ceil(x);
	frame.SetReturnValue(double(y));
}


void Ceilf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = ceil(x);
	frame.SetReturnValue(float(y));
}


void Floor(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = floor(x);
	frame.SetReturnValue(double(y));
}


void Floorf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = floor(x);
	frame.SetReturnValue(float(y));
}


void Fabs(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const double y = fabs(x);
	frame.SetReturnValue(double(y));
}


void Fabsf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const double y = fabs(x);
	frame.SetReturnValue(float(y));
}


void Ldexp(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	const int n = int(frame.GetArg<int32_t>(1));
	const double result = ldexp(x, n);
	frame.SetReturnValue(double(result));
}


void Ldexpf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	const int n = int(frame.GetArg<int32_t>(1));
	const double result = ldexp(x, n);
	frame.SetReturnValue(float(result));
}


void Frexp(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<double>(0));
	int exp;
	const double result = frexp(x, &exp);
	frame.SetReturnValue(double(result));
	*frame.GetArg<int32_t *>(1) = int32_t(exp);
}


void Frexpf(Bond::StackFrame &frame)
{
	const double x = double(frame.GetArg<float>(0));
	int exp;
	const double result = frexp(x, &exp);
	frame.SetReturnValue(float(result));
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
