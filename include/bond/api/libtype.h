#ifndef BOND_API_LIBTYPE_H
#define BOND_API_LIBTYPE_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection TYPE_BINDING_COLLECTION;
}

namespace Bond
{
void IsAlnum(Bond::CalleeStackFrame &frame);
void IsAlpha(Bond::CalleeStackFrame &frame);
void IsCntrl(Bond::CalleeStackFrame &frame);
void IsDigit(Bond::CalleeStackFrame &frame);
void IsGraph(Bond::CalleeStackFrame &frame);
void IsLower(Bond::CalleeStackFrame &frame);
void IsPrint(Bond::CalleeStackFrame &frame);
void IsPunct(Bond::CalleeStackFrame &frame);
void IsSpace(Bond::CalleeStackFrame &frame);
void IsUpper(Bond::CalleeStackFrame &frame);
void IsXDigit(Bond::CalleeStackFrame &frame);
void ToLower(Bond::CalleeStackFrame &frame);
void ToUpper(Bond::CalleeStackFrame &frame);
}

#endif
