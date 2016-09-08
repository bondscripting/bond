#ifndef BOND_API_LIBSTRING_H
#define BOND_API_LIBSTRING_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection STRING_BINDING_COLLECTION;
}

namespace Bond
{
void Strlen(Bond::StackFrame &frame);
void TryParseInt(Bond::StackFrame &frame);
void TryParseLong(Bond::StackFrame &frame);
void TryParseFloat(Bond::StackFrame &frame);
void TryParseDouble(Bond::StackFrame &frame);
}

#endif
