#ifndef BOND_CBOUTIL_H
#define BOND_CBOUTIL_H

#include "bond/types/types.h"

namespace Bond
{

inline bu32_t EncodeSizeAndType(bu32_t size, bu32_t type)
{
	return (size << 4) | (type & 0xf);
}


inline void DecodeSizeAndType(bu32_t sizeAndType, bu32_t &size, bu32_t &type)
{
	size = sizeAndType >> 4;
	type = sizeAndType & 0xf;
}


inline PointerSize DecodePointerSize(bu16_t flags)
{
	return ((flags & 1) != 0) ? POINTER_64BIT : POINTER_32BIT;
}


inline bu16_t EncodePointerSize(bu16_t flags, PointerSize pointerSize)
{
	return flags | ((pointerSize == POINTER_64BIT) ? 1 : 0);
}

}

#endif
