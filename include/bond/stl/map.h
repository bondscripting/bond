#ifndef BOND_STL_MAP_H
#define BOND_STL_MAP_H

/// \file
/// \brief The ordered associative container used by Bond.
///
/// By default, Bond::Map<KeyType, ValueType, CompareType> is an alias of std::map whose memory is
/// allocated from a Bond::Allocator via StlAllocator. CompareType defaults to std::less<KeyType>.
/// It can be replaced by defining USER_BOND_MAP_HEADER. A replacement must provide a Bond::Map
/// class template with the interface of std::map that is constructible from a StlAllocator.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_MAP_HEADER

#include USER_BOND_MAP_HEADER

#else

#include "bond/private/map.h"

#endif

#endif
