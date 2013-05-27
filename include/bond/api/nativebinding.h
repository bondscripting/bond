#ifndef BOND_NATIVEBINDING_H
#define BOND_NATIVEBINDING_H

#include "bond/types/simplestring.h"

namespace Bond
{

class VM;

typedef void (*NativeFunction)(VM &vm);


struct NativeFunctionBinding
{
	bu32_t mHash;
	NativeFunction mFunction;
};


struct NativeBindingCollection
{
	const NativeFunctionBinding *mFunctionBindings;
	bu32_t mFunctionBindingCount;
};

}

#endif
