#ifndef PRIVATE_BOND_VECTOR_H
#define PRIVATE_BOND_VECTOR_H

#include "bond/stlallocator.h"
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