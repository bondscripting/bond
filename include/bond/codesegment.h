#ifndef BOND_CODESEGMENT_H
#define BOND_CODESEGMENT_H

#include "bond/hashedstring.h"
#include "bond/value.h"

namespace Bond
{

class QualifiedId
{
public:

	QualifiedId():
		mElements(NULL),
		mNumElements(0)
	{}

	QualifiedId(const HashedString **elements, int numElements):
		mElements(elements),
		mNumElements(numElements)
	{}

	const HashedString &GetElement(int index) const { return *mElements[index]; }
	int GetNumElements() const { return mNumElements; }

private:
	const HashedString **mElements;
	int mNumElements;
};


struct ConstantTable
{
	const Value32 *mValue32Table;
	const Value64 *mValue64Table;
	const HashedString *mStringTable;
};


struct Function
{
	QualifiedId mName;
	const ConstantTable *mConstantTable;
	const unsigned char *mCode;
	size_t mCodeSize;
	size_t mFrameSize;
	size_t mPackedFrameSize;
	size_t mLocalSize;
	size_t mFramePointerAlignment;
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
	size_t mFunctionCount;
};

}

#endif
