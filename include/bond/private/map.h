#ifndef PRIVATE_BOND_MAP_H
#define PRIVATE_BOND_MAP_H

#include "bond/stlallocator.h"
#include <map>

namespace Bond
{

template <typename KeyType, typename ValueType, typename CompareType = std::less<KeyType> >
struct Map
{
	typedef CompareType Compare;
	typedef std::pair<KeyType, ValueType> KeyValue;
	typedef StlAllocator<KeyValue> Allocator;
	typedef std::map<KeyType, ValueType, CompareType, Allocator> Type;
	typedef std::pair<typename Type::iterator, bool> InsertResult;
};

}

#endif
