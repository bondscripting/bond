#ifndef BOND_PRIVATE_TYPES_H
#define BOND_PRIVATE_TYPES_H

#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#define BOND_SCNd32 SCNd32
#define BOND_PRId32 PRId32

#define BOND_SCNi32 SCNi32
#define BOND_PRIi32 PRIi32

#define BOND_SCNu32 SCNu32
#define BOND_PRIu32 PRIu32

#define BOND_SCNo32 SCNo32
#define BOND_PRIo32 PRIo32

#define BOND_SCNx32 SCNx32
#define BOND_PRIx32 PRIx32

#define BOND_SCNd64 SCNd64
#define BOND_PRId64 PRId64

#define BOND_SCNi64 SCNi64
#define BOND_PRIi64 PRIi64

#define BOND_SCNu64 SCNu64
#define BOND_PRIu64 PRIu64

#define BOND_SCNo64 SCNo64
#define BOND_PRIo64 PRIo64

#define BOND_SCNx64 SCNx64
#define BOND_PRIx64 PRIx64

namespace Bond
{

const int32_t BOND_CHAR_MAX = std::numeric_limits<int8_t>::max();
const int32_t BOND_CHAR_MIN = std::numeric_limits<int8_t>::min();
const uint32_t BOND_UCHAR_MAX = std::numeric_limits<uint8_t>::max();
const uint32_t BOND_UCHAR_MIN = std::numeric_limits<uint8_t>::min();

const int32_t BOND_SHORT_MAX = std::numeric_limits<int16_t>::max();
const int32_t BOND_SHORT_MIN = std::numeric_limits<int16_t>::min();
const uint32_t BOND_USHORT_MAX = std::numeric_limits<uint16_t>::max();
const uint32_t BOND_USHORT_MIN = std::numeric_limits<uint16_t>::min();

const int32_t BOND_INT_MAX = std::numeric_limits<int32_t>::max();
const int32_t BOND_INT_MIN = std::numeric_limits<int32_t>::min();
const uint32_t BOND_UINT_MAX = std::numeric_limits<uint32_t>::max();
const uint32_t BOND_UINT_MIN = std::numeric_limits<uint32_t>::min();

const int64_t BOND_LONG_MAX = std::numeric_limits<int64_t>::max();
const int64_t BOND_LONG_MIN = std::numeric_limits<int64_t>::min();
const uint64_t BOND_ULONG_MAX = std::numeric_limits<uint64_t>::max();
const uint64_t BOND_ULONG_MIN = std::numeric_limits<uint64_t>::min();

const size_t BOND_SIZE_MAX = std::numeric_limits<size_t>::max();
const size_t BOND_SIZE_MIN = std::numeric_limits<size_t>::min();


template <typename FloatType, typename IntegralType, typename Enabled = void>
struct floating_point_to_integral_limits
{};


template <typename FloatType, typename IntegralType>
struct floating_point_to_integral_limits<FloatType, IntegralType, typename std::enable_if<
	std::is_floating_point<FloatType>::value && std::is_integral<IntegralType>::value &&
	(std::numeric_limits<IntegralType>::digits <= std::numeric_limits<FloatType>::digits)
>::type>
{
	static constexpr FloatType max() { return FloatType(std::numeric_limits<IntegralType>::max()); }
	static constexpr FloatType min() { return FloatType(std::numeric_limits<IntegralType>::min()); }
};


template <typename FloatType, typename IntegralType>
struct floating_point_to_integral_limits<FloatType, IntegralType, typename std::enable_if<
	std::is_floating_point<FloatType>::value && std::is_integral<IntegralType>::value &&
	(std::numeric_limits<IntegralType>::digits > std::numeric_limits<FloatType>::digits) &&
	!std::numeric_limits<IntegralType>::is_signed
>::type>
{
	static constexpr FloatType max()
	{
		return FloatType(~((IntegralType(1) << (std::numeric_limits<IntegralType>::digits - std::numeric_limits<FloatType>::digits)) - IntegralType(1)));
	}
	static constexpr FloatType min() { return FloatType(0); }
};


template <typename FloatType, typename IntegralType>
struct floating_point_to_integral_limits<FloatType, IntegralType, typename std::enable_if<
	std::is_floating_point<FloatType>::value && std::is_integral<IntegralType>::value &&
	(std::numeric_limits<IntegralType>::digits > std::numeric_limits<FloatType>::digits) &&
	std::numeric_limits<IntegralType>::is_signed
>::type>
{
	static constexpr FloatType max()
	{
		return FloatType(std::numeric_limits<IntegralType>::max() & ~((IntegralType(1) << (std::numeric_limits<IntegralType>::digits - std::numeric_limits<FloatType>::digits)) - IntegralType(1)));
	}
	static constexpr FloatType min() { return -max(); }
};


// Trivial implementation for when RangeType can encode all values of ValueType.
template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_integral<RangeType>::value && std::is_integral<ValueType>::value &&
	(((std::numeric_limits<ValueType>::digits <= std::numeric_limits<RangeType>::digits) &&
	  (std::numeric_limits<RangeType>::is_signed == std::numeric_limits<ValueType>::is_signed)) ||
	 ((std::numeric_limits<ValueType>::digits < std::numeric_limits<RangeType>::digits) &&
	  (std::numeric_limits<RangeType>::is_signed && !std::numeric_limits<ValueType>::is_signed))),
bool>::type IsInRange(ValueType value)
{
	return true;
}


// RangeType is at least as large as ValueType, but is unsigned, so cannot
// encode negative values.
template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_integral<RangeType>::value && std::is_integral<ValueType>::value &&
	(std::numeric_limits<ValueType>::digits <= std::numeric_limits<RangeType>::digits) &&
	(!std::numeric_limits<RangeType>::is_signed && std::numeric_limits<ValueType>::is_signed),
bool>::type IsInRange(ValueType value)
{
	return value >= ValueType(0);
}


// ValueType is signed and can encode all values of RangeType;
// compare the value against upper and lower bounds of RangeType.
template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_integral<RangeType>::value && std::is_integral<ValueType>::value &&
	(std::numeric_limits<ValueType>::digits > std::numeric_limits<RangeType>::digits) &&
	std::numeric_limits<ValueType>::is_signed,
bool>::type IsInRange(ValueType value)
{
	return (value >= ValueType(std::numeric_limits<RangeType>::min())) &&
		(value <= ValueType(std::numeric_limits<RangeType>::max()));
}


// ValueType is unsigned and can encode all non-negative values of RangeType;
// compare the value against upper bound of RangeType.
template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_integral<RangeType>::value && std::is_integral<ValueType>::value &&
	(std::numeric_limits<ValueType>::digits > std::numeric_limits<RangeType>::digits) &&
	!std::numeric_limits<ValueType>::is_signed,
bool>::type IsInRange(ValueType value)
{
	return value <= ValueType(std::numeric_limits<RangeType>::max());
}


template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_integral<RangeType>::value && std::is_floating_point<ValueType>::value,
bool>::type IsInRange(ValueType value)
{
	ValueType intPart;
	return
		(value == ValueType(0)) ||
		(std::isnormal(value) &&
		 (std::modf(value, &intPart) == ValueType(0)) &&
		 (intPart >= floating_point_to_integral_limits<ValueType, RangeType>::min()) &&
		 (intPart <= floating_point_to_integral_limits<ValueType, RangeType>::max()));
}


template <typename RangeType, typename ValueType>
typename std::enable_if<
	std::is_floating_point<RangeType>::value,
bool>::type IsInRange(ValueType value)
{
	const RangeType convertedValue = RangeType(value);
	const ValueType restoredValue = ValueType(convertedValue);
	return value == restoredValue;
}

}

#endif
