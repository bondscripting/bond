#ifndef BOND_API_LIBIO_H
#define BOND_API_LIBIO_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection IO_BINDING_COLLECTION;
}

namespace Bond
{
void PrintStr(Bond::CalleeStackFrame &frame);
void PrintB(Bond::CalleeStackFrame &frame);
void PrintC(Bond::CalleeStackFrame &frame);
void PrintI(Bond::CalleeStackFrame &frame);
void PrintUI(Bond::CalleeStackFrame &frame);
void PrintL(Bond::CalleeStackFrame &frame);
void PrintUL(Bond::CalleeStackFrame &frame);
void PrintF(Bond::CalleeStackFrame &frame);
void PrintD(Bond::CalleeStackFrame &frame);
}

#endif
