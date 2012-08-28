#ifndef BOND_CODESEGMENT_H
#define BOND_CODESEGMENT_H

#include "bond/hashedstring.h"
#include "bond/value.h"

namespace Bond
{

struct QualifiedId
{
	QualifiedId(const SimpleString **elements, bu32_t elementCount):
		mElements(elements),
		mElementCount(elementCount)
	{}

	const SimpleString **mElements;
	bu32_t mElementCount;
};


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
	QualifiedId mName;
	ReturnSignature mReturnSignature;
	ParamListSignature mParamListSignature;
	const ConstantTable *mConstantTable;
	const bu8_t *mCode;
	bu32_t mCodeSize;
	bu32_t mFrameSize;
	bu32_t mPackedFrameSize;
	bu32_t mLocalSize;
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

private:
	const bu32_t *mFunctionLookup;
	const Function *mFunctionList;
	bu32_t mFunctionCount;
};

}

#endif
