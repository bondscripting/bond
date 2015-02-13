#ifndef BOND_PRIVATE_TYPES_H
#define BOND_PRIVATE_TYPES_H

#include <cinttypes>
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

using std::false_type;
using std::true_type;

using std::integral_constant;
using std::numeric_limits;

const int32_t BOND_CHAR_MAX = numeric_limits<int8_t>::max();
const int32_t BOND_CHAR_MIN = numeric_limits<int8_t>::min();
const uint32_t BOND_UCHAR_MAX = numeric_limits<uint8_t>::max();
const uint32_t BOND_UCHAR_MIN = numeric_limits<uint8_t>::min();

const int32_t BOND_SHORT_MAX = numeric_limits<int16_t>::max();
const int32_t BOND_SHORT_MIN = numeric_limits<int16_t>::min();
const uint32_t BOND_USHORT_MAX = numeric_limits<uint16_t>::max();
const uint32_t BOND_USHORT_MIN = numeric_limits<uint16_t>::min();

const int32_t BOND_INT_MAX = numeric_limits<int32_t>::max();
const int32_t BOND_INT_MIN = numeric_limits<int32_t>::min();
const uint32_t BOND_UINT_MAX = numeric_limits<uint32_t>::max();
const uint32_t BOND_UINT_MIN = numeric_limits<uint32_t>::min();

const int64_t BOND_LONG_MAX = numeric_limits<int64_t>::max();
const int64_t BOND_LONG_MIN = numeric_limits<int64_t>::min();
const uint64_t BOND_ULONG_MAX = numeric_limits<uint64_t>::max();
const uint64_t BOND_ULONG_MIN = numeric_limits<uint64_t>::min();

const size_t BOND_SIZE_MAX = numeric_limits<size_t>::max();
const size_t BOND_SIZE_MIN = numeric_limits<size_t>::min();


template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType value, std::false_type, std::false_type, std::false_type)
{
    return value <= ValueType(std::numeric_limits<RangeType>::max());
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType value, std::false_type, std::false_type, std::true_type)
{
    return (value >= 0) && (value <= ValueType(std::numeric_limits<RangeType>::max()));
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType value, std::false_type, std::true_type, std::false_type)
{
    return (value <= ValueType(std::numeric_limits<RangeType>::max()));
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType value, std::false_type, std::true_type, std::true_type)
{
    return (value >= ValueType(std::numeric_limits<RangeType>::min())) &&
        (value <= ValueType(std::numeric_limits<RangeType>::max()));
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType, std::true_type, std::false_type, std::false_type)
{
    return true;
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType value, std::true_type, std::false_type, std::true_type)
{
    return value >= 0;
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType, std::true_type, std::true_type, std::false_type)
{
    return true;
}

template <typename RangeType, typename ValueType>
bool IsInRangeInternal(ValueType, std::true_type, std::true_type, std::true_type)
{
    return true;
}

template <typename RangeType, typename ValueType>
bool IsInRange(ValueType value)
{
    using namespace std;
    return IsInRangeInternal<RangeType>(
        value,
        integral_constant<bool, numeric_limits<ValueType>::digits <= numeric_limits<RangeType>::digits>(),
        integral_constant<bool, numeric_limits<RangeType>::is_signed>(),
        integral_constant<bool, numeric_limits<ValueType>::is_signed>());
}

}

#endif
