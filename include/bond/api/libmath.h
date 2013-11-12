#ifndef BOND_API_LIBMATH_H
#define BOND_API_LIBMATH_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection MATH_BINDING_COLLECTION;
}

namespace Bond
{
void Sin(Bond::CalleeStackFrame &frame);
void Sinf(Bond::CalleeStackFrame &frame);
void Cos(Bond::CalleeStackFrame &frame);
void Cosf(Bond::CalleeStackFrame &frame);
void Tan(Bond::CalleeStackFrame &frame);
void Tanf(Bond::CalleeStackFrame &frame);
void Asin(Bond::CalleeStackFrame &frame);
void Asinf(Bond::CalleeStackFrame &frame);
void Acos(Bond::CalleeStackFrame &frame);
void Acosf(Bond::CalleeStackFrame &frame);
void Atan(Bond::CalleeStackFrame &frame);
void Atanf(Bond::CalleeStackFrame &frame);
void Atan2(Bond::CalleeStackFrame &frame);
void Atan2f(Bond::CalleeStackFrame &frame);
void Sinh(Bond::CalleeStackFrame &frame);
void Sinhf(Bond::CalleeStackFrame &frame);
void Cosh(Bond::CalleeStackFrame &frame);
void Coshf(Bond::CalleeStackFrame &frame);
void Tanh(Bond::CalleeStackFrame &frame);
void Tanhf(Bond::CalleeStackFrame &frame);
void Exp(Bond::CalleeStackFrame &frame);
void Expf(Bond::CalleeStackFrame &frame);
void Log(Bond::CalleeStackFrame &frame);
void Logf(Bond::CalleeStackFrame &frame);
void Log10(Bond::CalleeStackFrame &frame);
void Log10f(Bond::CalleeStackFrame &frame);
void Pow(Bond::CalleeStackFrame &frame);
void Powf(Bond::CalleeStackFrame &frame);
void Sqrt(Bond::CalleeStackFrame &frame);
void Sqrtf(Bond::CalleeStackFrame &frame);
void Ceil(Bond::CalleeStackFrame &frame);
void Ceilf(Bond::CalleeStackFrame &frame);
void Floor(Bond::CalleeStackFrame &frame);
void Floorf(Bond::CalleeStackFrame &frame);
void Fabs(Bond::CalleeStackFrame &frame);
void Fabsf(Bond::CalleeStackFrame &frame);
void Ldexp(Bond::CalleeStackFrame &frame);
void Ldexpf(Bond::CalleeStackFrame &frame);
void Frexp(Bond::CalleeStackFrame &frame);
void Frexpf(Bond::CalleeStackFrame &frame);
void Modf(Bond::CalleeStackFrame &frame);
void Modff(Bond::CalleeStackFrame &frame);
void Fmod(Bond::CalleeStackFrame &frame);
void Fmodf(Bond::CalleeStackFrame &frame);
}

#endif
