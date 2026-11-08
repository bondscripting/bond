#ifndef BOND_TYPES_TYPES_H
#define BOND_TYPES_TYPES_H

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
  BOND_SIGNATURE_TYPE_ITEM(VOID,      "void",                   "void")                   \
  BOND_SIGNATURE_TYPE_ITEM(BOOL,      "bool",                   "bool")                   \
  BOND_SIGNATURE_TYPE_ITEM(CHAR,      "char",                   "int8_t")                 \
  BOND_SIGNATURE_TYPE_ITEM(UCHAR,     "uchar",                  "uint8_t")                \
  BOND_SIGNATURE_TYPE_ITEM(SHORT,     "short",                  "int16_t")                \
  BOND_SIGNATURE_TYPE_ITEM(USHORT,    "ushort",                 "uint16_t")               \
  BOND_SIGNATURE_TYPE_ITEM(INT,       "int",                    "int32_t")                \
  BOND_SIGNATURE_TYPE_ITEM(UINT,      "uint",                   "uint32_t")               \
  BOND_SIGNATURE_TYPE_ITEM(LONG,      "long",                   "int64_t")                \
  BOND_SIGNATURE_TYPE_ITEM(ULONG,     "ulong",                  "uint64_t")               \
  BOND_SIGNATURE_TYPE_ITEM(FLOAT,     "float",                  "float")                  \
  BOND_SIGNATURE_TYPE_ITEM(DOUBLE,    "double",                 "double")                 \
  BOND_SIGNATURE_TYPE_ITEM(POINTER,   "*",                      "pointer")                \
  BOND_SIGNATURE_TYPE_ITEM(AGGREGATE, "aggr<%" BOND_PRIu32 ">", "aggr<%" BOND_PRIu32 ">") \


enum SignatureType
{
#define BOND_SIGNATURE_TYPE_ITEM(type, bondType, apiType) SIG_ ## type,
	BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
};


const uint32_t BOND_VOID_SIZE = 0;
const uint32_t BOND_BOOL_SIZE = 1;
const uint32_t BOND_CHAR_SIZE = 1;
const uint32_t BOND_UCHAR_SIZE = 1;
const uint32_t BOND_SHORT_SIZE = 2;
const uint32_t BOND_USHORT_SIZE = 2;
const uint32_t BOND_INT_SIZE = 4;
const uint32_t BOND_UINT_SIZE = 4;
const uint32_t BOND_LONG_SIZE = 8;
const uint32_t BOND_ULONG_SIZE = 8;
const uint32_t BOND_FLOAT_SIZE = 4;
const uint32_t BOND_DOUBLE_SIZE = 8;
const uint32_t BOND_DEFAULT_STRUCT_SIZE = 4;
const uint32_t BOND_DEFAULT_STRUCT_ALIGN = BOND_DEFAULT_STRUCT_SIZE;
const int32_t BOND_SLOT_SIZE = 8;
const PointerSize BOND_NATIVE_POINTER_SIZE = (sizeof(void *) == 8) ? POINTER_64BIT : POINTER_32BIT;

static_assert(sizeof(int8_t) == 1, "sizeof(int8_t) is not 1");
static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) is not 1");
static_assert(sizeof(int16_t) == 2, "sizeof(int16_t) is not 2");
static_assert(sizeof(uint16_t) == 2, "sizeof(uint16_t) is not 2");
static_assert(sizeof(int32_t) == 4, "sizeof(int32_t) is not 4");
static_assert(sizeof(uint32_t) == 4, "sizeof(uint32_t) is not 4");
static_assert(sizeof(int64_t) == 8, "sizeof(int64_t) is not 8");
static_assert(sizeof(uint64_t) == 8, "sizeof(uint64_t) is not 8");
static_assert(sizeof(float) == 4, "sizeof(float) is not 4");
static_assert(sizeof(double) == 8, "sizeof(double) is not 8");
static_assert(
	((BOND_NATIVE_POINTER_SIZE == POINTER_32BIT) && sizeof(void *) == 4) ||
	((BOND_NATIVE_POINTER_SIZE == POINTER_64BIT) && sizeof(void *) == 8),
	"BOND_NATIVE_POINTER_SIZE is not properly defined");


inline uint32_t GetPointerSize(PointerSize pointerSize) { return (pointerSize == POINTER_64BIT) ? 8 : 4; }

BOND_API const char *GetBondTypeMnemonic(SignatureType signatureType);
BOND_API char *ExpandBondTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

BOND_API const char *GetApiTypeMnemonic(SignatureType signatureType);
BOND_API char *ExpandApiTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

#define BOND_NATIVE_GETTER_SUFFIX "@get"
#define BOND_NATIVE_SETTER_SUFFIX "@set"

}

#endif
