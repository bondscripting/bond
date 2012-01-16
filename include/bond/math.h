#ifndef BOND_MATH_H
#define BOND_MATH_H

namespace Bond
{

template <typename T>
inline T AlignUp(T value, T align) { return (value + (align - 1)) & ~(align - 1); }


template <typename T>
inline T AlignDown(T value, T align) { return value & ~(align - 1); }


template <typename T>
inline T IsPowerOfTwo(T value) { return (value == 1) || ((value & (value - 1)) == 0); }


template <typename T>
inline T Min(T a, T b) { return (a < b) ? a : b; }


template <typename T>
inline T Max(T a, T b) { return (a > b) ? a : b; }

}

#endif
