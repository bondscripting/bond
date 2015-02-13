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


inline uint32_t GetPointerSize(PointerSize pointerSize) { return (pointerSize == POINTER_64BIT) ? 8 : 4; }

const char *GetBondTypeMnemonic(SignatureType signatureType);
char *ExpandBondTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

const char *GetApiTypeMnemonic(SignatureType signatureType);
char *ExpandApiTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

#define BOND_NATIVE_GETTER_SUFFIX "@get"
#define BOND_NATIVE_SETTER_SUFFIX "@set"

}

#endif
