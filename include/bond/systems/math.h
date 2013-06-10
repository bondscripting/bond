#ifndef BOND_SYSTEMS_MATH_H
#define BOND_SYSTEMS_MATH_H

#include "bond/types/types.h"

namespace Bond
{

template <typename T>
inline T AlignUp(T value, T align) { return (value + (align - 1)) & ~(align - 1); }


template <typename T>
inline T AlignDown(T value, T align) { return value & ~(align - 1); }


inline void *AlignPointerUp(const void *ptr, size_t align)
{
	return reinterpret_cast<void *>(AlignUp(reinterpret_cast<intptr_t>(ptr), intptr_t(align)));
}


inline void *AlignPointerDown(const void *ptr, size_t align)
{
	return reinterpret_cast<void *>(AlignDown(reinterpret_cast<intptr_t>(ptr), intptr_t(align)));
}


template <typename T>
inline T IsPowerOfTwo(T value) { return (value == 1) || ((value & (value - 1)) == 0); }


template <typename T>
inline T RoundUpToPowerOfTwo32(T value)
{
	T v = value - 1;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v + 1;
}


template <typename T>
inline T Min(T a, T b) { return (a < b) ? a : b; }


template <typename T>
inline T Max(T a, T b) { return (a > b) ? a : b; }

}

#endif
