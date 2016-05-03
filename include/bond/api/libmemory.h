#ifndef BOND_API_LIBMEMORY_H
#define BOND_API_LIBMEMORY_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION;
}

namespace Bond
{
void Allocate(Bond::CalleeStackFrame &frame);
void AllocateAligned(Bond::CalleeStackFrame &frame);
void Free(Bond::CalleeStackFrame &frame);
void FreeAligned(Bond::CalleeStackFrame &frame);
void Memcpy(Bond::CalleeStackFrame &frame);
}

#endif
