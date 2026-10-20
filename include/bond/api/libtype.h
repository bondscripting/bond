#ifndef BOND_API_LIBTYPE_H
#define BOND_API_LIBTYPE_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection TYPE_BINDING_COLLECTION;
}

namespace Bond
{
BOND_API void IsAlnum(Bond::StackFrame &frame);
BOND_API void IsAlpha(Bond::StackFrame &frame);
BOND_API void IsBlank(Bond::StackFrame &frame);
BOND_API void IsCntrl(Bond::StackFrame &frame);
BOND_API void IsDigit(Bond::StackFrame &frame);
BOND_API void IsGraph(Bond::StackFrame &frame);
BOND_API void IsLower(Bond::StackFrame &frame);
BOND_API void IsPrint(Bond::StackFrame &frame);
BOND_API void IsPunct(Bond::StackFrame &frame);
BOND_API void IsSpace(Bond::StackFrame &frame);
BOND_API void IsUpper(Bond::StackFrame &frame);
BOND_API void IsXDigit(Bond::StackFrame &frame);
BOND_API void ToLower(Bond::StackFrame &frame);
BOND_API void ToUpper(Bond::StackFrame &frame);
}

#endif
