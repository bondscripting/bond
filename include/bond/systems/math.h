#ifndef BOND_SYSTEMS_MATH_H
#define BOND_SYSTEMS_MATH_H

#include "bond/types/types.h"

namespace Bond
{

/// \addtogroup math
/// @{

/// \brief Rounds an integer down to the nearest multiple of a specified power of two.
/// \param value The value to be rounded.
/// \param align The power of two around which the value is rounded.
template <typename T>
inline T AlignDown(T value, T align) { return value & ~(align - 1); }


/// \brief Rounds an integer up to the nearest multiple of a specified power of two.
/// \param value The value to be rounded.
/// \param align The power of two around which the value is rounded.
template <typename T>
inline T AlignUp(T value, T align) { return (value + (align - 1)) & ~(align - 1); }


/// \brief Returns the difference between an integer and the nearest larger multiple of a specified power of two.
/// \param value The value to be rounded.
/// \param align The power of two around which the value is rounded.
template <typename T>
inline T AlignUpDelta(T value, T align) { return AlignUp(value, align) - value; }


/// \brief Rounds a pointer down the nearest multiple of a specified power of two.
/// \param ptr The pointer to be rounded.
/// \param align The power of two around which the pointer is rounded.
template <typename T>
inline T *AlignPointerDown(T *ptr, size_t align)
{
	return reinterpret_cast<T *>(AlignDown(reinterpret_cast<intptr_t>(ptr), intptr_t(align)));
}


/// \brief Rounds a pointer up to the nearest multiple of a specified power of two.
/// \param ptr The pointer to be rounded.
/// \param align The power of two around which the pointer is rounded.
template <typename T>
inline T *AlignPointerUp(T *ptr, size_t align)
{
	return reinterpret_cast<T *>(AlignUp(reinterpret_cast<intptr_t>(ptr), intptr_t(align)));
}


/// \brief Returns whether an integer is a power of two.
/// \param value The value to test.
template <typename T>
inline T IsPowerOfTwo(T value) { return (value == 1) || ((value & (value - 1)) == 0); }


/// \brief Rounds a 32 bit integer up to the nearest power of two.
/// \param value The value to be rounded.
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


/// \brief Returns the absolute value of a number.
/// \param value The number whose absolute value is evaluated.
template <typename T>
inline T Abs(T value) { return (value < T(0)) ? -value : value; }


/// \brief Compares two numbers and returns the smaller of the two.
/// \param a The first of the two compared numbers.
/// \param b The second of the two compared numbers.
template <typename T>
inline T Min(T a, T b) { return (a < b) ? a : b; }


/// \brief Compares two numbers and returns the larger of the two.
/// \param a The first of the two compared numbers.
/// \param b The second of the two compared numbers.
template <typename T>
inline T Max(T a, T b) { return (a > b) ? a : b; }

/// @}

}

#endif
