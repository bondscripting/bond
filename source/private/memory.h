#ifndef BOND_MEMORY_H
#define BOND_MEMORY_H

#include "bond/math.h"

namespace Bond
{

template<typename T>
inline size_t TallyMemoryRequirements(size_t &size, size_t count)
{
	const size_t start = AlignUp(size, sizeof(T));
	size = start + (count * sizeof(T));
	return start;
}

}

#endif
