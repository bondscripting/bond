#ifndef BOND_PRIVATE_LIST_H
#define BOND_PRIVATE_LIST_H

#include "bond/stl/stlallocator.h"
#include <list>

namespace Bond
{

template <typename ValueType>
struct List
{
	typedef StlAllocator<ValueType> Allocator;
	typedef std::list<ValueType, Allocator> Type;
};

}

#endif
