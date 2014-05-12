#ifndef BOND_VM_CODESEGMENT_H
#define BOND_VM_CODESEGMENT_H

#include "bond/api/nativebinding.h"
#include "bond/types/hashedstring.h"
#include "bond/types/value.h"

namespace Bond
{

struct ConstantTable
{
	const Value32 *mValue32Table;
	const Value64 *mValue64Table;
	const SimpleString *mStringTable;
};


struct ParamSignature
{
	ParamSignature(bi32_t framePointerOffset, bu32_t size, bu32_t type):
		mFramePointerOffset(framePointerOffset),
		mSize(size),
		mType(type)
	{}

	bi32_t mFramePointerOffset;
	bu32_t mSize:28;
	bu32_t mType:4;
};


struct ParamListSignature
{
	ParamListSignature(const ParamSignature *paramSignatures, bu32_t paramCount):
		mParamSignatures(paramSignatures),
		mParamCount(paramCount)
	{}

	const ParamSignature *mParamSignatures;
	bu32_t mParamCount;
};


struct ReturnSignature
{
	ReturnSignature(bu32_t size, bu32_t type):
		mSize(size),
		mType(type)
	{}

	bu32_t mSize:28;
	bu32_t mType:4;
};


struct Function
{
	bool IsNative() const { return mCodeSize == 0; }

	const char *const *mName;
	ReturnSignature mReturnSignature;
	ParamListSignature mParamListSignature;
	const ConstantTable *mConstantTable;

	union
	{
		const bu8_t *mCode;
		NativeFunction mNativeFunction;
	};

	bu32_t mCodeSize;
	bu32_t mArgSize;
	bu32_t mPackedArgSize;
	bu32_t mLocalSize;
	bu32_t mStackSize;
	bu32_t mFramePointerAlignment;
	bu32_t mHash;
};


class CodeSegment
{
public:
	CodeSegment(const bu32_t *functionLookup, const Function *functionList, size_t functionCount):
		mFunctionLookup(functionLookup),
		mFunctionList(functionList),
		mFunctionCount(functionCount)
	{}

	const Function *GetFunction(const HashedString &functionName) const { return GetFunction(functionName.GetHashCode()); }
	const Function *GetFunction(bu32_t functionHash) const;

	bi32_t GetFunctionIndex(const HashedString &functionName) const { return GetFunctionIndex(functionName.GetHashCode()); }
	bi32_t GetFunctionIndex(bu32_t functionHash) const;
	const Function *GetFunctionAtIndex(bu32_t functionIndex) const { return mFunctionList + functionIndex; }

private:
	const bu32_t *mFunctionLookup;
	const Function *mFunctionList;
	size_t mFunctionCount;
};

}

#endif
