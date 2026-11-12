#ifndef BOND_STL_UTILITY_H
#define BOND_STL_UTILITY_H

/// \file
/// \brief Utility functions used by Bond.
///
/// By default, forward and move are brought into the Bond namespace from the C++ standard
/// library. They can be replaced by defining USER_BOND_UTILITY_HEADER. A replacement must declare
/// these functions in the Bond namespace with the standard semantics.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_UTILITY_HEADER

#include USER_BOND_UTILITY_HEADER

#else

#include "bond/private/utility.h"

#endif

#endif
