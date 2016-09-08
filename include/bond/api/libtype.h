#ifndef BOND_API_LIBTYPE_H
#define BOND_API_LIBTYPE_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection TYPE_BINDING_COLLECTION;
}

namespace Bond
{
void IsAlnum(Bond::StackFrame &frame);
void IsAlpha(Bond::StackFrame &frame);
void IsCntrl(Bond::StackFrame &frame);
void IsDigit(Bond::StackFrame &frame);
void IsGraph(Bond::StackFrame &frame);
void IsLower(Bond::StackFrame &frame);
void IsPrint(Bond::StackFrame &frame);
void IsPunct(Bond::StackFrame &frame);
void IsSpace(Bond::StackFrame &frame);
void IsUpper(Bond::StackFrame &frame);
void IsXDigit(Bond::StackFrame &frame);
void ToLower(Bond::StackFrame &frame);
void ToUpper(Bond::StackFrame &frame);
}

#endif
