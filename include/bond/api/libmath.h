#ifndef BOND_API_LIBMATH_H
#define BOND_API_LIBMATH_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection MATH_BINDING_COLLECTION;
}

namespace Bond
{
void Sin(Bond::StackFrame &frame);
void Sinf(Bond::StackFrame &frame);
void Cos(Bond::StackFrame &frame);
void Cosf(Bond::StackFrame &frame);
void Tan(Bond::StackFrame &frame);
void Tanf(Bond::StackFrame &frame);
void Asin(Bond::StackFrame &frame);
void Asinf(Bond::StackFrame &frame);
void Acos(Bond::StackFrame &frame);
void Acosf(Bond::StackFrame &frame);
void Atan(Bond::StackFrame &frame);
void Atanf(Bond::StackFrame &frame);
void Atan2(Bond::StackFrame &frame);
void Atan2f(Bond::StackFrame &frame);
void Sinh(Bond::StackFrame &frame);
void Sinhf(Bond::StackFrame &frame);
void Cosh(Bond::StackFrame &frame);
void Coshf(Bond::StackFrame &frame);
void Tanh(Bond::StackFrame &frame);
void Tanhf(Bond::StackFrame &frame);
void Asinh(Bond::StackFrame &frame);
void Asinhf(Bond::StackFrame &frame);
void Acosh(Bond::StackFrame &frame);
void Acoshf(Bond::StackFrame &frame);
void Atanh(Bond::StackFrame &frame);
void Atanhf(Bond::StackFrame &frame);
void Exp(Bond::StackFrame &frame);
void Expf(Bond::StackFrame &frame);
void Exp2(Bond::StackFrame &frame);
void Exp2f(Bond::StackFrame &frame);
void Log(Bond::StackFrame &frame);
void Logf(Bond::StackFrame &frame);
void Log2(Bond::StackFrame &frame);
void Log2f(Bond::StackFrame &frame);
void Log10(Bond::StackFrame &frame);
void Log10f(Bond::StackFrame &frame);
void Pow(Bond::StackFrame &frame);
void Powf(Bond::StackFrame &frame);
void Sqrt(Bond::StackFrame &frame);
void Sqrtf(Bond::StackFrame &frame);
void Cbrt(Bond::StackFrame &frame);
void Cbrtf(Bond::StackFrame &frame);
void Hypot(Bond::StackFrame &frame);
void Hypotf(Bond::StackFrame &frame);
void Ceil(Bond::StackFrame &frame);
void Ceilf(Bond::StackFrame &frame);
void Floor(Bond::StackFrame &frame);
void Floorf(Bond::StackFrame &frame);
void Trunc(Bond::StackFrame &frame);
void Truncf(Bond::StackFrame &frame);
void Fabs(Bond::StackFrame &frame);
void Fabsf(Bond::StackFrame &frame);
void Ldexp(Bond::StackFrame &frame);
void Ldexpf(Bond::StackFrame &frame);
void Frexp(Bond::StackFrame &frame);
void Frexpf(Bond::StackFrame &frame);
void Modf(Bond::StackFrame &frame);
void Modff(Bond::StackFrame &frame);
void Fmod(Bond::StackFrame &frame);
void Fmodf(Bond::StackFrame &frame);
void CopySign(Bond::StackFrame &frame);
void CopySignf(Bond::StackFrame &frame);
void NaN(Bond::StackFrame &frame);
void NaNf(Bond::StackFrame &frame);
}

#endif
