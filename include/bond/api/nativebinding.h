#ifndef BOND_API_NATIVEBINDING_H
#define BOND_API_NATIVEBINDING_H

#include "bond/types/simplestring.h"

namespace Bond
{

class CalleeStackFrame;

typedef void (*NativeFunction)(CalleeStackFrame &frame);


struct NativeFunctionBinding
{
	const char *mName;
	NativeFunction mFunction;
};


struct NativeBindingCollection
{
	const NativeFunctionBinding *mFunctionBindings;
	uint32_t mFunctionBindingCount;
};

}

#endif
