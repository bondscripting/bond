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
#define BOND_PRIX32 PRIX32

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


/// \brief Returns whether value is exactly representable by RangeType, i.e. whether converting it to
/// RangeType and back is lossless. NaN and infinity are never representable.
template <typename RangeType, typename ValueType>
bool IsInRange(ValueType value)
{
	static_assert(std::is_arithmetic<RangeType>::value && std::is_arithmetic<ValueType>::value, "IsInRange requires arithmetic types");
	using RangeLimits = std::numeric_limits<RangeType>;
	using ValueLimits = std::numeric_limits<ValueType>;

	if constexpr (std::is_integral<RangeType>::value && std::is_integral<ValueType>::value)
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
	else if constexpr (std::is_integral<RangeType>::value)
	{
		// 2^digits is exactly representable, so the bounds are exact. NaN fails every comparison.
		const ValueType upperBound = std::ldexp(ValueType(1), RangeLimits::digits);
		const ValueType lowerBound = RangeLimits::is_signed ? -upperBound : ValueType(0);
		return (std::trunc(value) == value) && (value >= lowerBound) && (value < upperBound);
	}
	else if constexpr (std::is_integral<ValueType>::value)
	{
		// A value near the top of ValueType's range can round up to 2^digits, which is out of range
		// for ValueType, so the converted value is checked before it is converted back.
		const RangeType convertedValue = RangeType(value);
		return (convertedValue < std::ldexp(RangeType(1), ValueLimits::digits)) && (ValueType(convertedValue) == value);
	}
	else
	{
		// Converting a finite value outside RangeType's range is undefined, so check it first.
		return (std::fabs(value) <= RangeLimits::max()) && (ValueType(RangeType(value)) == value);
	}
}

}

#endif
