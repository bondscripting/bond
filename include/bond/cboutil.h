#ifndef BOND_CBOUTIL_H
#define BOND_CBOUTIL_H

#include "bond/types.h"

namespace Bond
{

inline PointerSize DecodePointerSize(bu16_t majorVersionAndFlags)
{
	return ((majorVersionAndFlags & 0x8000) != 0) ? POINTER_64BIT : POINTER_32BIT;
}

inline bu16_t DecodeMajorVersion(bu16_t majorVersionAndFlags)
{
	return majorVersionAndFlags & 0x7fff;
}

}

#endif
