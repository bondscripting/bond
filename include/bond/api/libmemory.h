#ifndef BOND_API_LIBMEMORY_H
#define BOND_API_LIBMEMORY_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION;
}

namespace Bond
{
void GetCollector(Bond::StackFrame &frame);
void Allocate(Bond::StackFrame &frame);
void AllocateCollected(Bond::StackFrame &frame);
void AllocateWithCollector(Bond::StackFrame &frame);
void AllocateAligned(Bond::StackFrame &frame);
void AllocateAlignedCollected(Bond::StackFrame &frame);
void AllocateAlignedWithCollector(Bond::StackFrame &frame);
void Free(Bond::StackFrame &frame);
void FreeAligned(Bond::StackFrame &frame);
void Memcpy(Bond::StackFrame &frame);
}

#endif
