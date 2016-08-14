#ifndef BOND_PRIVATE_LIST_H
#define BOND_PRIVATE_LIST_H

#include "bond/stl/stlallocator.h"
#include <list>

namespace Bond
{

template <typename ValueType>
using List = std::list<ValueType, StlAllocator<ValueType> >;

}

#endif
