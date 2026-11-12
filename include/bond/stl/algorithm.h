#ifndef BOND_STL_ALGORITHM_H
#define BOND_STL_ALGORITHM_H

/// \file
/// \brief Algorithms used by Bond.
///
/// By default, find, lower_bound and sort are brought into the Bond namespace from the C++
/// standard library. They can be replaced by defining USER_BOND_ALGORITHM_HEADER. A replacement
/// must declare these functions in the Bond namespace with the standard semantics.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_ALGORITHM_HEADER

#include USER_BOND_ALGORITHM_HEADER

#else

#include "bond/private/algorithm.h"

#endif

#endif
