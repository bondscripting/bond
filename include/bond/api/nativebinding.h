#ifndef BOND_NATIVEBINDING_H
#define BOND_NATIVEBINDING_H

#include "bond/types/simplestring.h"

namespace Bond
{

class VM;

struct NativeTypeBinding
{
	const char *const *mName;
	SignatureType mSignatureType;
	bu32_t mSize;
	bu32_t mAlignment;
};


typedef void (*NativeFunction)(VM &vm);


struct NativeFunctionBinding
{
	const char *const *mName;
	const NativeTypeBinding *mReturnTypeBinding;
	const NativeTypeBinding *mParamBindings;
	const bu32_t mParamCount;
	NativeFunction mFunction;
};


struct NativeBindingCollection
{
	const NativeFunctionBinding *mFunctionBindings;
	const bu32_t mFunctionBindingCount;
};

}

#endif
