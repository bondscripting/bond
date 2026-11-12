#ifndef BOND_STL_VECTOR_H
#define BOND_STL_VECTOR_H

/// \file
/// \brief The dynamic array container used by Bond.
///
/// By default, Bond::Vector<ValueType> is an alias of std::vector whose memory is allocated from a
/// Bond::Allocator via StlAllocator. It can be replaced by defining USER_BOND_VECTOR_HEADER. A
/// replacement must provide a Bond::Vector class template with the interface of std::vector that
/// is constructible from a StlAllocator, e.g.
/// <code>Vector<int> v((Vector<int>::allocator_type(&allocator)));</code>.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_VECTOR_HEADER

#include USER_BOND_VECTOR_HEADER

#else

#include "bond/private/vector.h"

#endif

#endif
