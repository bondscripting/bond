#ifndef BOND_STL_CMATH_H
#define BOND_STL_CMATH_H

/// \file
/// \brief Math functions used by Bond, such as those backing its math standard library.
///
/// By default, the functions from <cmath> plus abs, min, max and clamp are brought into the Bond
/// namespace from the C++ standard library. They can be replaced, e.g. with a deterministic math
/// library, by defining USER_BOND_CMATH_HEADER. A replacement must provide float and double
/// overloads of each function in the Bond namespace, including isnan, isinf, isfinite and signbit,
/// which C math libraries often define as macros, and integer overloads of abs, min, max and clamp.

#include "bond/conf.h"

#ifdef USER_BOND_CMATH_HEADER

#include USER_BOND_CMATH_HEADER

#else

#include "bond/private/cmath.h"

#endif

#endif
