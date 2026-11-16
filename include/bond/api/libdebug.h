#ifndef BOND_API_LIBDEBUG_H
#define BOND_API_LIBDEBUG_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection DEBUG_BINDING_COLLECTION;
}

namespace Bond
{
namespace Debug
{
BOND_API void Break(Bond::StackFrame &frame);
BOND_API void IsDebuggerPresent(Bond::StackFrame &frame);
}
}

#endif
