#ifndef BOND_PRIVATE_VECTOR_H
#define BOND_PRIVATE_VECTOR_H

#include "bond/stl/stlallocator.h"
#include <vector>

namespace Bond
{

template <typename ValueType>
struct Vector
{
	typedef StlAllocator<ValueType> Allocator;
	typedef std::vector<ValueType, Allocator> Type;
};

}

#endif
