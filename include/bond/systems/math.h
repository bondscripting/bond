#ifndef BOND_SYSTEMS_MATH_H
#define BOND_SYSTEMS_MATH_H

#include "bond/stl/cmath.h"
#include "bond/stl/typetraits.h"
#include "bond/types/types.h"
#include <limits>

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


/// \brief Returns whether value is exactly representable by RangeType, i.e. whether converting it to
/// RangeType and back is lossless. NaN and infinity are never representable.
template <typename RangeType, typename ValueType>
bool IsInRange(ValueType value)
{
	static_assert(is_arithmetic<RangeType>::value && is_arithmetic<ValueType>::value, "IsInRange requires arithmetic types");
	using RangeLimits = std::numeric_limits<RangeType>;
	using ValueLimits = std::numeric_limits<ValueType>;

	if constexpr (is_integral<RangeType>::value && is_integral<ValueType>::value)
	{
		// Comparing through intmax_t and uintmax_t avoids mixed signedness comparisons.
		if constexpr (ValueLimits::is_signed)
		{
			if (value < ValueType(0))
			{
				return RangeLimits::is_signed && (intmax_t(value) >= intmax_t(RangeLimits::min()));
			}
		}
		return uintmax_t(value) <= uintmax_t(RangeLimits::max());
	}
	else if constexpr (is_integral<RangeType>::value)
	{
		// 2^digits is exactly representable, so the bounds are exact. NaN fails every comparison.
		const ValueType upperBound = Bond::ldexp(ValueType(1), RangeLimits::digits);
		const ValueType lowerBound = RangeLimits::is_signed ? -upperBound : ValueType(0);
		return (Bond::trunc(value) == value) && (value >= lowerBound) && (value < upperBound);
	}
	else if constexpr (is_integral<ValueType>::value)
	{
		// A value near the top of ValueType's range can round up to 2^digits, which is out of range
		// for ValueType, so the converted value is checked before it is converted back.
		const RangeType convertedValue = RangeType(value);
		return (convertedValue < Bond::ldexp(RangeType(1), ValueLimits::digits)) && (ValueType(convertedValue) == value);
	}
	else
	{
		// Converting a finite value outside RangeType's range is undefined, so check it first.
		return (Bond::fabs(value) <= RangeLimits::max()) && (ValueType(RangeType(value)) == value);
	}
}

/// @}

}

#endif
