#ifndef BOND_PRIVATE_MAP_H
#define BOND_PRIVATE_MAP_H

#include "bond/stl/stlallocator.h"
#include <map>

namespace Bond
{

template <typename KeyType, typename ValueType, typename CompareType = std::less<KeyType> >
using Map = std::map<KeyType, ValueType, CompareType, StlAllocator<ValueType> >;

}

#endif
