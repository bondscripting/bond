#ifndef BOND_API_LIBMATH_H
#define BOND_API_LIBMATH_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection MATH_BINDING_COLLECTION;
}

namespace Bond
{
void Sin(Bond::VM &vm);
void Sinf(Bond::VM &vm);
void Cos(Bond::VM &vm);
void Cosf(Bond::VM &vm);
void Tan(Bond::VM &vm);
void Tanf(Bond::VM &vm);
void Asin(Bond::VM &vm);
void Asinf(Bond::VM &vm);
void Acos(Bond::VM &vm);
void Acosf(Bond::VM &vm);
void Atan(Bond::VM &vm);
void Atanf(Bond::VM &vm);
void Atan2(Bond::VM &vm);
void Atan2f(Bond::VM &vm);
void Sinh(Bond::VM &vm);
void Sinhf(Bond::VM &vm);
void Cosh(Bond::VM &vm);
void Coshf(Bond::VM &vm);
void Tanh(Bond::VM &vm);
void Tanhf(Bond::VM &vm);
void Exp(Bond::VM &vm);
void Expf(Bond::VM &vm);
void Log(Bond::VM &vm);
void Logf(Bond::VM &vm);
void Log10(Bond::VM &vm);
void Log10f(Bond::VM &vm);
void Pow(Bond::VM &vm);
void Powf(Bond::VM &vm);
void Sqrt(Bond::VM &vm);
void Sqrtf(Bond::VM &vm);
void Ceil(Bond::VM &vm);
void Ceilf(Bond::VM &vm);
void Floor(Bond::VM &vm);
void Floorf(Bond::VM &vm);
void Fabs(Bond::VM &vm);
void Fabsf(Bond::VM &vm);
void Ldexp(Bond::VM &vm);
void Ldexpf(Bond::VM &vm);
void Frexp(Bond::VM &vm);
void Frexpf(Bond::VM &vm);
void Modf(Bond::VM &vm);
void Modff(Bond::VM &vm);
void Fmod(Bond::VM &vm);
void Fmodf(Bond::VM &vm);
}

#endif
