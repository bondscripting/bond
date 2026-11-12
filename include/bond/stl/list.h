#ifndef BOND_STL_LIST_H
#define BOND_STL_LIST_H

/// \file
/// \brief The doubly linked list container used by Bond.
///
/// By default, Bond::List<ValueType> is an alias of std::list whose memory is allocated from a
/// Bond::Allocator via StlAllocator. It can be replaced by defining USER_BOND_LIST_HEADER. A
/// replacement must provide a Bond::List class template with the interface of std::list that is
/// constructible from a StlAllocator, e.g. <code>List<int> list((List<int>::allocator_type(&allocator)));</code>.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_LIST_HEADER

#include USER_BOND_LIST_HEADER

#else

#include "bond/private/list.h"

#endif

#endif
