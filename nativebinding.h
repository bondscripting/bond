#ifndef BOND_NATIVEBINDING_H
#define BOND_NATIVEBINDING_H

#include "bond/types.h"

namespace Bond
{

struct TypeBinding
{
	const char *mName;
	SignatureType mSignatureType;
	bu32_t mSize;
	bu32_t mAlignment;
};

struct FunctionBinding
{
	const char *mName;
	const TypeBinding *mReturnTypeBinding;
	const TYpeBInding *mParamBindings;
	const bu32_t mNumParams;
};

}

#endif
