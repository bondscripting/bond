#ifndef BOND_MEMORY_H
#define BOND_MEMORY_H

#include "bond/systems/math.h"

namespace Bond
{

template<typename T>
inline size_t TallyMemoryRequirements(size_t &size, size_t count, size_t align)
{
	const size_t start = AlignUp(size, align);
	size = start + (count * sizeof(T));
	return start;
}


template<typename T>
inline size_t TallyMemoryRequirements(size_t &size, size_t count)
{
	return TallyMemoryRequirements<T>(size, count, sizeof(T));
}

}

#endif
