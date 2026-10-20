#ifndef BOND_API_LIBMEMORY_H
#define BOND_API_LIBMEMORY_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION;
}

namespace Bond
{
BOND_API void GetCollector(Bond::StackFrame &frame);
BOND_API void Allocate(Bond::StackFrame &frame);
BOND_API void AllocateCollected(Bond::StackFrame &frame);
BOND_API void AllocateWithCollector(Bond::StackFrame &frame);
BOND_API void AllocateAligned(Bond::StackFrame &frame);
BOND_API void AllocateAlignedCollected(Bond::StackFrame &frame);
BOND_API void AllocateAlignedWithCollector(Bond::StackFrame &frame);
BOND_API void Free(Bond::StackFrame &frame);
BOND_API void FreeAligned(Bond::StackFrame &frame);
BOND_API void Memcpy(Bond::StackFrame &frame);
}

#endif
