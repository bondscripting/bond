#ifndef PRIVATE_BOND_LIST_H
#define PRIVATE_BOND_LIST_H

#include "bond/stlallocator.h"
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
