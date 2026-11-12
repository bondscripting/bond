#ifndef BOND_TYPES_TYPES_H
#define BOND_TYPES_TYPES_H

/// \file
/// \brief Fixed-size integer types and the sizes, signature types and mnemonics of Bond types.
///
/// The fixed-size integer types are obtained from <cstdint> by default. That can be replaced by
/// defining USER_BOND_TYPES_HEADER. A replacement must provide int8_t through uint64_t and size_t,
/// the BOND_PRI* and BOND_SCN* printf and scanf format macros for 32 and 64-bit integers
/// (e.g. BOND_PRId32, BOND_SCNu64), and the BOND_<TYPE>_MIN and BOND_<TYPE>_MAX constants for the
/// Bond integer types and size_t (e.g. BOND_CHAR_MAX, BOND_SIZE_MAX) in the Bond namespace.
///
/// \ingroup types

#include "bond/conf.h"

#ifdef USER_BOND_TYPES_HEADER

#include USER_BOND_TYPES_HEADER

#else

#include "bond/private/types.h"

#endif

namespace Bond
{

/// \addtogroup types
/// @{

/// \brief The size of pointers on a target platform.
enum PointerSize
{
	/// \brief 4-byte pointers.
	POINTER_32BIT,

	/// \brief 8-byte pointers.
	POINTER_64BIT,
};


/// \brief An X-macro that lists the signature types used to describe the parameters and return
/// values of functions in CBO files.
///
/// To use it, define BOND_SIGNATURE_TYPE_ITEM(type, bondType, apiType), expand
/// BOND_SIGNATURE_TYPE_LIST and then undefine BOND_SIGNATURE_TYPE_ITEM. The arguments of each item
/// are the signature type name without its SIG_ prefix, the printf format string of its mnemonic
/// in Bond and the printf format string of its mnemonic in the C++ API. The aggregate mnemonics
/// expect the size of the aggregate as a uint32_t argument.
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


/// \brief Enumeration of the types of function parameters and return values in CBO files.
///
/// Its values are SIG_VOID, SIG_BOOL, SIG_CHAR, SIG_UCHAR, SIG_SHORT, SIG_USHORT, SIG_INT,
/// SIG_UINT, SIG_LONG, SIG_ULONG, SIG_FLOAT, SIG_DOUBLE, SIG_POINTER and SIG_AGGREGATE, the last of
/// which describes structs passed by value.
enum SignatureType
{
#define BOND_SIGNATURE_TYPE_ITEM(type, bondType, apiType) SIG_ ## type,
	BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
};


/// \brief Size in bytes of the Bond void type.
const uint32_t BOND_VOID_SIZE = 0;

/// \brief Size in bytes of the Bond bool type.
const uint32_t BOND_BOOL_SIZE = 1;

/// \brief Size in bytes of the Bond char type.
const uint32_t BOND_CHAR_SIZE = 1;

/// \brief Size in bytes of the Bond uchar type.
const uint32_t BOND_UCHAR_SIZE = 1;

/// \brief Size in bytes of the Bond short type.
const uint32_t BOND_SHORT_SIZE = 2;

/// \brief Size in bytes of the Bond ushort type.
const uint32_t BOND_USHORT_SIZE = 2;

/// \brief Size in bytes of the Bond int type.
const uint32_t BOND_INT_SIZE = 4;

/// \brief Size in bytes of the Bond uint type.
const uint32_t BOND_UINT_SIZE = 4;

/// \brief Size in bytes of the Bond long type.
const uint32_t BOND_LONG_SIZE = 8;

/// \brief Size in bytes of the Bond ulong type.
const uint32_t BOND_ULONG_SIZE = 8;

/// \brief Size in bytes of the Bond float type.
const uint32_t BOND_FLOAT_SIZE = 4;

/// \brief Size in bytes of the Bond double type.
const uint32_t BOND_DOUBLE_SIZE = 8;

/// \brief Minimum size in bytes of a struct whose layout is computed by the compiler.
const uint32_t BOND_DEFAULT_STRUCT_SIZE = 4;

/// \brief Minimum alignment in bytes of a struct whose layout is computed by the compiler.
const uint32_t BOND_DEFAULT_STRUCT_ALIGN = BOND_DEFAULT_STRUCT_SIZE;

/// \brief Size in bytes of a slot on the operand stack of the VM.
const int32_t BOND_SLOT_SIZE = 8;

/// \brief The size of pointers on the platform for which the code is compiled.
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


/// \brief Returns the number of bytes in a pointer of the given size.
inline uint32_t GetPointerSize(PointerSize pointerSize) { return (pointerSize == POINTER_64BIT) ? 8 : 4; }

/// \brief Returns the mnemonic of the given signature type as it is named in Bond, e.g. "int".
///
/// The mnemonic is a printf format string. That of SIG_AGGREGATE expects the size of the aggregate
/// as a uint32_t argument; ExpandBondTypeMnemonic() can be used to expand it.
BOND_API const char *GetBondTypeMnemonic(SignatureType signatureType);

/// \brief Writes the mnemonic of the given signature type as it is named in Bond to the given
/// buffer, expanding the size of an aggregate, e.g. "aggr<12>".
/// \param buffer The buffer to which the mnemonic is written.
/// \param length The size of the buffer in bytes. The mnemonic is truncated if it does not fit.
/// \param signatureType The signature type whose mnemonic is written.
/// \param size The size in bytes of the type, used only by SIG_AGGREGATE.
/// \returns The given buffer.
BOND_API char *ExpandBondTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

/// \brief Returns the mnemonic of the given signature type as it is named in the C++ API, e.g.
/// "int32_t".
///
/// The mnemonic is a printf format string. That of SIG_AGGREGATE expects the size of the aggregate
/// as a uint32_t argument; ExpandApiTypeMnemonic() can be used to expand it.
BOND_API const char *GetApiTypeMnemonic(SignatureType signatureType);

/// \brief Writes the mnemonic of the given signature type as it is named in the C++ API to the
/// given buffer, expanding the size of an aggregate, e.g. "aggr<12>".
/// \param buffer The buffer to which the mnemonic is written.
/// \param length The size of the buffer in bytes. The mnemonic is truncated if it does not fit.
/// \param signatureType The signature type whose mnemonic is written.
/// \param size The size in bytes of the type, used only by SIG_AGGREGATE.
/// \returns The given buffer.
BOND_API char *ExpandApiTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, uint32_t size);

/// \brief Suffix appended to the qualified name of a native struct member to form the name of
/// the native function that reads its value.
#define BOND_NATIVE_GETTER_SUFFIX "@get"

/// \brief Suffix appended to the qualified name of a native struct member to form the name of
/// the native function that writes its value.
#define BOND_NATIVE_SETTER_SUFFIX "@set"

/// @}

}

#endif
