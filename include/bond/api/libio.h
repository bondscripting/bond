#ifndef BOND_API_LIBIO_H
#define BOND_API_LIBIO_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection IO_BINDING_COLLECTION;
}

namespace Bond
{
void OutputStream__PrintStr(Bond::CalleeStackFrame &frame);
void OutputStream__PrintB(Bond::CalleeStackFrame &frame);
void OutputStream__PrintC(Bond::CalleeStackFrame &frame);
void OutputStream__PrintI(Bond::CalleeStackFrame &frame);
void OutputStream__PrintUI(Bond::CalleeStackFrame &frame);
void OutputStream__PrintL(Bond::CalleeStackFrame &frame);
void OutputStream__PrintUL(Bond::CalleeStackFrame &frame);
void OutputStream__PrintF(Bond::CalleeStackFrame &frame);
void OutputStream__PrintD(Bond::CalleeStackFrame &frame);
void OutputStream__GetPosition(Bond::CalleeStackFrame &frame);
void OutputStream__SetPosition(Bond::CalleeStackFrame &frame);
void OutputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame);
void OutputStream__AddOffset(Bond::CalleeStackFrame &frame);
void StdOut(Bond::CalleeStackFrame &frame);
void StdErr(Bond::CalleeStackFrame &frame);
}

#endif
