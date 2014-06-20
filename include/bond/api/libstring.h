#ifndef BOND_API_LIBSTRING_H
#define BOND_API_LIBSTRING_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection STRING_BINDING_COLLECTION;
}

namespace Bond
{
void Strlen(Bond::CalleeStackFrame &frame);
void TryParseInt(Bond::CalleeStackFrame &frame);
void TryParseLong(Bond::CalleeStackFrame &frame);
void TryParseFloat(Bond::CalleeStackFrame &frame);
void TryParseDouble(Bond::CalleeStackFrame &frame);
}

#endif
