#ifndef BOND_TOOLS_CBOUTIL_H
#define BOND_TOOLS_CBOUTIL_H

#include "bond/types/types.h"

namespace Bond
{

inline uint32_t EncodeSizeAndType(uint32_t size, uint32_t type)
{
	return (size << 4) | (type & 0xf);
}


inline void DecodeSizeAndType(uint32_t sizeAndType, uint32_t &size, uint32_t &type)
{
	size = sizeAndType >> 4;
	type = sizeAndType & 0xf;
}


inline PointerSize DecodePointerSize(uint16_t flags)
{
	return ((flags & 1) != 0) ? POINTER_64BIT : POINTER_32BIT;
}


inline uint16_t EncodePointerSize(uint16_t flags, PointerSize pointerSize)
{
	return flags | ((pointerSize == POINTER_64BIT) ? 1 : 0);
}

}

#endif
