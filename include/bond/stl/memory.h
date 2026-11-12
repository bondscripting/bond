#ifndef BOND_STL_MEMORY_H
#define BOND_STL_MEMORY_H

/// \file
/// \brief Smart pointers used by Bond.
///
/// By default, unique_ptr is brought into the Bond namespace from the C++ standard library. It
/// can be replaced by defining USER_BOND_MEMORY_HEADER. A replacement must declare a
/// Bond::unique_ptr class template with the interface of std::unique_ptr, including support for
/// custom deleters, which the handles of Bond::Allocator rely on.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_MEMORY_HEADER

#include USER_BOND_MEMORY_HEADER

#else

#include "bond/private/memory.h"

#endif

#endif
