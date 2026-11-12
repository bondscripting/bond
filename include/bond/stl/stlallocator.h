#ifndef BOND_STL_STLALLOCATOR_H
#define BOND_STL_STLALLOCATOR_H

/// \file
/// \brief An adaptor that allows standard library containers to allocate memory from a
/// Bond::Allocator.
///
/// By default, Bond::StlAllocator<T> is a class template that meets the C++ Allocator requirements
/// and forwards allocations to the Bond::Allocator pointer passed to its constructor. Types whose
/// alignment exceeds that of std::max_align_t are allocated with Allocator::AllocAligned. Two
/// StlAllocators compare equal if they use the same Bond::Allocator. Allocation failures throw
/// std::bad_alloc. It can be replaced by defining USER_BOND_STLALLOCATOR_HEADER. A replacement
/// must provide a Bond::StlAllocator class template with an explicit constructor taking an
/// Allocator pointer.
///
/// \ingroup stl

#include "bond/systems/allocator.h"

#ifdef USER_BOND_STLALLOCATOR_HEADER

#include USER_BOND_STLALLOCATOR_HEADER

#else

#include "bond/private/stlallocator.h"

#endif

#endif
