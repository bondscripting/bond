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


#define BOND_SIGNATURE_TYPE_LIST \
  BOND_SIGNATURE_TYPE_ITEM(VOID,    "void")                     \
  BOND_SIGNATURE_TYPE_ITEM(BOOL,    "bool")                     \
  BOND_SIGNATURE_TYPE_ITEM(CHAR,    "char")                     \
  BOND_SIGNATURE_TYPE_ITEM(UCHAR,   "uchar")                    \
  BOND_SIGNATURE_TYPE_ITEM(SHORT,   "short")                    \
  BOND_SIGNATURE_TYPE_ITEM(USHORT,  "ushort")                   \
  BOND_SIGNATURE_TYPE_ITEM(INT,     "int")                      \
  BOND_SIGNATURE_TYPE_ITEM(UINT,    "uint")                     \
  BOND_SIGNATURE_TYPE_ITEM(LONG,    "long")                     \
  BOND_SIGNATURE_TYPE_ITEM(ULONG,   "ulong")                    \
  BOND_SIGNATURE_TYPE_ITEM(FLOAT,   "float")                    \
  BOND_SIGNATURE_TYPE_ITEM(DOUBLE,  "double")                   \
  BOND_SIGNATURE_TYPE_ITEM(POINTER, "*")                        \
  BOND_SIGNATURE_TYPE_ITEM(STRUCT,  "struct<%" BOND_PRIu32 ">") \


enum SignatureType
{
#define BOND_SIGNATURE_TYPE_ITEM(type, mnemonic) SIG_ ## type,
	BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
};


const bu32_t BOND_VOID_SIZE = 0;
const bu32_t BOND_BOOL_SIZE = 1;
const bu32_t BOND_CHAR_SIZE = 1;
const bu32_t BOND_UCHAR_SIZE = 1;
const bu32_t BOND_SHORT_SIZE = 2;
const bu32_t BOND_USHORT_SIZE = 2;
const bu32_t BOND_INT_SIZE = 4;
const bu32_t BOND_UINT_SIZE = 4;
const bu32_t BOND_LONG_SIZE = 8;
const bu32_t BOND_ULONG_SIZE = 8;
const bu32_t BOND_FLOAT_SIZE = 4;
const bu32_t BOND_DOUBLE_SIZE = 8;
const bu32_t BOND_DEFAULT_STRUCT_SIZE = 4;
const bu32_t BOND_DEFAULT_STRUCT_ALIGN = BOND_DEFAULT_STRUCT_SIZE;
const bi32_t BOND_SLOT_SIZE = 8;
const PointerSize BOND_NATIVE_POINTER_SIZE = (sizeof(void *) == 8) ? POINTER_64BIT : POINTER_32BIT;


inline bu32_t GetPointerSize(PointerSize pointerSize) { return (pointerSize == POINTER_64BIT) ? 8 : 4; }


inline bool IsInCharRange(bi32_t value) { return (value >= BOND_CHAR_MIN) && (value <= BOND_CHAR_MAX); }
inline bool IsInCharRange(bu32_t value) { return value <= static_cast<bu32_t>(BOND_CHAR_MAX); }
inline bool IsInCharRange(bi64_t value) { return (value >= BOND_CHAR_MIN) && (value <= BOND_CHAR_MAX); }
inline bool IsInCharRange(bu64_t value) { return value <= static_cast<bu64_t>(BOND_CHAR_MAX); }


inline bool IsInUCharRange(bi32_t value) { return (value >= 0) && (value <= static_cast<bi32_t>(BOND_UCHAR_MAX)); }
inline bool IsInUCharRange(bu32_t value) { return value <= BOND_UCHAR_MAX; }
inline bool IsInUCharRange(bi64_t value) { return (value >= 0) && (value <= static_cast<bi64_t>(BOND_UCHAR_MAX)); }
inline bool IsInUCharRange(bu64_t value) { return value <= BOND_UCHAR_MAX; }


inline bool IsInShortRange(bi32_t value) { return (value >= BOND_SHORT_MIN) && (value <= BOND_SHORT_MAX); }
inline bool IsInShortRange(bu32_t value) { return value <= static_cast<bu32_t>(BOND_SHORT_MAX); }
inline bool IsInShortRange(bi64_t value) { return (value >= BOND_SHORT_MIN) && (value <= BOND_SHORT_MAX); }
inline bool IsInShortRange(bu64_t value) { return value <= static_cast<bu64_t>(BOND_SHORT_MAX); }


inline bool IsInUShortRange(bi32_t value) { return (value >= 0) && (value <= static_cast<bi32_t>(BOND_USHORT_MAX)); }
inline bool IsInUShortRange(bu32_t value) { return value <= BOND_USHORT_MAX; }
inline bool IsInUShortRange(bi64_t value) { return (value >= 0) && (value <= static_cast<bi64_t>(BOND_USHORT_MAX)); }
inline bool IsInUShortRange(bu64_t value) { return value <= BOND_USHORT_MAX; }


inline bool IsInIntRange(bi64_t value) { return (value >= BOND_INT_MIN) && (value <= BOND_INT_MAX); }
inline bool IsInIntRange(bu64_t value) { return value <= static_cast<bu64_t>(BOND_INT_MAX); }


inline bool IsInUIntRange(bi64_t value) { return (value >= 0) && (value <= static_cast<bi64_t>(BOND_UINT_MAX)); }
inline bool IsInUIntRange(bu64_t value) { return value <= BOND_UINT_MAX; }

}

#endif
