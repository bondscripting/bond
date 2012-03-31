#ifndef BOND_TYPES_H
#define BOND_TYPES_H

#include "bond/conf.h"

#ifdef USER_BOND_TYPES_HEADER

#include USER_BOND_TYPES_HEADER

#else

#include "bond/private/types.h"

#endif

namespace Bond
{

enum PointerSize
{
	POINTER_32BIT,
	POINTER_64BIT,
};

const bu32_t BOND_BOOL_SIZE = 1;
const bu32_t BOND_CHAR_SIZE = 1;
const bu32_t BOND_SHORT_SIZE = 2;
const bu32_t BOND_USHORT_SIZE = 2;
const bu32_t BOND_INT_SIZE = 4;
const bu32_t BOND_UINT_SIZE = 4;
const bu32_t BOND_LONG_SIZE = 8;
const bu32_t BOND_ULONG_SIZE = 8;
const bu32_t BOND_FLOAT_SIZE = 4;
const bu32_t BOND_DOUBLE_SIZE = 8;
const bu32_t BOND_NATIVE_POINTER_SIZE = sizeof(void *);
const bu32_t BOND_DEFAULT_STRUCT_SIZE = 4;
const bu32_t BOND_DEFAULT_STRUCT_ALIGN = BOND_DEFAULT_STRUCT_SIZE;

const bi32_t BOND_CHAR_MAX = 0x7F;
const bi32_t BOND_CHAR_MIN = -BOND_CHAR_MAX - 1;
const bu32_t BOND_UCHAR_MAX = 0xFF;
const bu32_t BOND_UCHAR_MIN = 0x00;
const bi32_t BOND_SHORT_MAX = 0x7FFF;
const bi32_t BOND_SHORT_MIN = -BOND_SHORT_MAX - 1;
const bu32_t BOND_USHORT_MAX = 0xFFFF;
const bu32_t BOND_USHORT_MIN = 0x0000;
const bi32_t BOND_INT_MAX = 0x7FFFFFFF;
const bi32_t BOND_INT_MIN = -BOND_INT_MAX - 1;
const bu32_t BOND_UINT_MAX = 0xFFFFFFFF;
const bu32_t BOND_UINT_MIN = 0x00000000;

inline bool IsInCharRange(bi32_t value) { return (value >= BOND_CHAR_MIN) && (value <= BOND_CHAR_MAX); }
inline bool IsInUCharRange(bu32_t value) { return value <= BOND_UCHAR_MAX; }

inline bool IsInShortRange(bi32_t value) { return (value >= BOND_SHORT_MIN) && (value <= BOND_SHORT_MAX); }
inline bool IsInUShortRange(bu32_t value) { return value <= BOND_USHORT_MAX; }

}

#endif
