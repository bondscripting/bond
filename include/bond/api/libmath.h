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
}

#endif
