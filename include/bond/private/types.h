#ifndef PRIVATE_BOND_TYPES_H
#define PRIVATE_BOND_TYPES_H

#include <stdint.h>

#if !defined(PRId32) && !defined(_MSC_VER)
#include <inttypes.h>
#endif

#if defined(PRId32)

#define BOND_SCNd32 SCNd32
#define BOND_PRId32 PRId32

#define BOND_SCNu32 SCNu32
#define BOND_PRIu32 PRIu32

#define BOND_SCNo32 SCNo32
#define BOND_PRIo32 PRIo32

#define BOND_SCNx32 SCNx32
#define BOND_PRIx32 PRIx32

#define BOND_SCNd64 SCNd64
#define BOND_PRId64 PRId64

#define BOND_SCNu64 SCNu64
#define BOND_PRIu64 PRIu64

#define BOND_SCNo64 SCNo64
#define BOND_PRIo64 PRIo64

#define BOND_SCNx64 SCNx64
#define BOND_PRIx64 PRIx64

#else

#define BOND_SCNd32 "d"
#define BOND_PRId32 "d"

#define BOND_SCNu32 "u"
#define BOND_PRIu32 "u"

#define BOND_SCNo32 "o"
#define BOND_PRIo32 "o"

#define BOND_SCNx32 "x"
#define BOND_PRIx32 "x"

#define BOND_SCNd64 "lld"
#define BOND_PRId64 "lld"

#define BOND_SCNu64 "llu"
#define BOND_PRIu64 "llu"

#define BOND_SCNo64 "llo"
#define BOND_PRIo64 "llo"

#define BOND_SCNx64 "llx"
#define BOND_PRIx64 "llx"

#endif

#define BOND_SCNf32 "f"
#define BOND_PRIf32 "f"

#define BOND_SCNf64 "lf"
#define BOND_PRIf64 "lf"

namespace Bond
{

typedef int8_t bi8_t;
typedef uint8_t bu8_t;
typedef int16_t bi16_t;
typedef uint16_t bu16_t;
typedef int32_t bi32_t;
typedef uint32_t bu32_t;
typedef int64_t bi64_t;
typedef uint64_t bu64_t;
typedef float bf32_t;
typedef double bf64_t;

const bi32_t BOND_CHAR_MAX = static_cast<bi32_t>(INT8_MAX);
const bi32_t BOND_CHAR_MIN = static_cast<bi32_t>(INT8_MIN);
const bu32_t BOND_UCHAR_MAX = static_cast<bu32_t>(UINT8_MAX);
const bu32_t BOND_UCHAR_MIN = static_cast<bu32_t>(0);
const bi32_t BOND_SHORT_MAX = static_cast<bi32_t>(INT16_MAX);
const bi32_t BOND_SHORT_MIN = static_cast<bi32_t>(INT16_MIN);
const bu32_t BOND_USHORT_MAX = static_cast<bu32_t>(UINT16_MAX);
const bu32_t BOND_USHORT_MIN = static_cast<bu32_t>(0);
const bi32_t BOND_INT_MAX = static_cast<bi32_t>(INT32_MAX);
const bi32_t BOND_INT_MIN = static_cast<bi32_t>(INT32_MIN);
const bu32_t BOND_UINT_MAX = static_cast<bu32_t>(UINT32_MAX);
const bu32_t BOND_UINT_MIN = static_cast<bu32_t>(0);
const bi64_t BOND_LONG_MAX = static_cast<bi64_t>(INT64_MAX);
const bi64_t BOND_LONG_MIN = static_cast<bi64_t>(INT64_MIN);
const bu64_t BOND_ULONG_MAX = static_cast<bu64_t>(UINT64_MAX);
const bu64_t BOND_ULONG_MIN = static_cast<bu64_t>(0);

}

#endif
