#ifndef BOND_CBOUTIL_H
#define BOND_CBOUTIL_H

#include "bond/types.h"

namespace Bond
{

inline PointerSize DecodePointerSize(bu16_t flags)
{
	return ((flags & 1) != 0) ? POINTER_64BIT : POINTER_32BIT;
}


inline bu16_t EncodePointerSize(bu16_t flags, bool is64BitPointer)
{
	return flags | (is64BitPointer ? 1 : 0);
}


inline bu16_t EncodePointerSize(bu16_t flags, PointerSize pointerSize)
{
	return EncodePointerSize(flags, pointerSize == POINTER_64BIT);
}

}

#endif
