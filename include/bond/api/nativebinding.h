#ifndef BOND_API_NATIVEBINDING_H
#define BOND_API_NATIVEBINDING_H

#include "bond/types/stringview.h"

namespace Bond
{

class StackFrame;

typedef void (*NativeFunction)(StackFrame &frame);


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
