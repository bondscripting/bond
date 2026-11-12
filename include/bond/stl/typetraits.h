#ifndef BOND_STL_TYPETRAITS_H
#define BOND_STL_TYPETRAITS_H

/// \file
/// \brief Type traits used by Bond.
///
/// By default, the type traits used by Bond, such as enable_if, is_integral_v and
/// underlying_type_t, are brought into the Bond namespace from the C++ standard library. They can
/// be replaced by defining USER_BOND_TYPETRAITS_HEADER. A replacement must declare every trait
/// brought in by bond/private/typetraits.h in the Bond namespace with the standard semantics.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_TYPETRAITS_HEADER

#include USER_BOND_TYPETRAITS_HEADER

#else

#include "bond/private/typetraits.h"

#endif

#endif
