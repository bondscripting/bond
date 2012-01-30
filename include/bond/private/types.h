#ifndef PRIVATE_BOND_TYPES_H
#define PRIVATE_BOND_TYPES_H

namespace Bond
{

typedef short bi16_t;
typedef unsigned short bu16_t;
typedef int bi32_t;
typedef unsigned int bu32_t;
typedef float bf32_t;

const bu32_t BOND_BOOL_SIZE = 1;
const bu32_t BOND_CHAR_SIZE = 1;
const bu32_t BOND_SHORT_SIZE = 2;
const bu32_t BOND_USHORT_SIZE = 2;
const bu32_t BOND_INT_SIZE = 4;
const bu32_t BOND_UINT_SIZE = 4;
const bu32_t BOND_FLOAT_SIZE = 4;
const bu32_t BOND_NATIVE_POINTER_SIZE = sizeof(void *); 
const bu32_t BOND_DEFAULT_STRUCT_SIZE = 4;
const bu32_t BOND_DEFAULT_STRUCT_ALIGN = BOND_DEFAULT_STRUCT_SIZE;

const bi32_t BOND_CHAR_MIN = -0x80;
const bi32_t BOND_CHAR_MAX = 0x7F;
const bu32_t BOND_UCHAR_MIN = 0x00;
const bu32_t BOND_UCHAR_MAX = 0xFF;
const bi32_t BOND_SHORT_MIN = -0x8000;
const bi32_t BOND_SHORT_MAX = 0x7FFF;
const bu32_t BOND_USHORT_MIN = 0x0000;
const bu32_t BOND_USHORT_MAX = 0xFFFF;
const bi32_t BOND_INT_MIN = -0x80000000;
const bi32_t BOND_INT_MAX = 0x7FFFFFFF;
const bu32_t BOND_UINT_MIN = 0x00000000;
const bu32_t BOND_UINT_MAX = 0xFFFFFFFF;

#define BOND_DECIMAL_FORMAT "%d"
#define BOND_UDECIMAL_FORMAT "%u"
#define BOND_UOCTAL_FORMAT "%o"
#define BOND_UHEX_FORMAT "%x"
#define BOND_FLOAT_FORMAT "%f"

}

#endif
