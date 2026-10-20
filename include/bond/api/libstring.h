#ifndef BOND_API_LIBSTRING_H
#define BOND_API_LIBSTRING_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection STRING_BINDING_COLLECTION;
}

namespace Bond
{
BOND_API void Strlen(Bond::StackFrame &frame);
BOND_API void Strcmp(Bond::StackFrame &frame);
BOND_API void TryParseInt(Bond::StackFrame &frame);
BOND_API void TryParseLong(Bond::StackFrame &frame);
BOND_API void TryParseFloat(Bond::StackFrame &frame);
BOND_API void TryParseDouble(Bond::StackFrame &frame);
}

#endif
