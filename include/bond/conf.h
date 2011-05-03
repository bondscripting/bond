#ifndef BOND_CONF_H
#define BOND_CONF_H

namespace Bond
{

typedef float bf32_t;
typedef int bi32_t;
typedef unsigned int bu32_t;

const bu32_t BOND_BOOL_SIZE = 1;
const bu32_t BOND_CHAR_SIZE = 1;
const bu32_t BOND_FLOAT_SIZE = sizeof(bf32_t);
const bu32_t BOND_INT_SIZE = sizeof(bf32_t);
const bu32_t BOND_UINT_SIZE = sizeof(bf32_t);
const bu32_t BOND_NATIVE_POINTER_SIZE = sizeof(void *); 

#define BOND_FLOAT_FORMAT "%f"
#define BOND_DECIMAL_FORMAT "%d"
#define BOND_UDECIMAL_FORMAT "%u"
#define BOND_UOCTAL_FORMAT "%o"
#define BOND_UHEX_FORMAT "%x"

}

#endif
