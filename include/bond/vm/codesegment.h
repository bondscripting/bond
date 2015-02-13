#ifndef BOND_VM_CODESEGMENT_H
#define BOND_VM_CODESEGMENT_H

#include "bond/api/nativebinding.h"
#include "bond/types/qualifiedname.h"
#include "bond/types/value.h"

namespace Bond
{

class VM;

struct ConstantTable
{
	const Value32 *mValue32Table;
	const Value64 *mValue64Table;
	const SimpleString *mStringTable;
	const QualifiedName *mQualifiedNameTable;
};


struct ParamSignature
{
	ParamSignature(int32_t framePointerOffset, uint32_t size, uint32_t type):
		mFramePointerOffset(framePointerOffset),
		mSize(size),
		mType(type)
	{}

	int32_t mFramePointerOffset;
	uint32_t mSize:28;
	uint32_t mType:4;
};


struct ParamListSignature
{
	ParamListSignature(const ParamSignature *paramSignatures, uint32_t paramCount):
		mParamSignatures(paramSignatures),
		mParamCount(paramCount)
	{}

	const ParamSignature *mParamSignatures;
	uint32_t mParamCount;
};


struct ReturnSignature
{
	ReturnSignature(uint32_t size, uint32_t type):
		mSize(size),
		mType(type)
	{}

	uint32_t mSize:28;
	uint32_t mType:4;
};


struct Function
{
	bool IsNative() const { return mCodeSize == 0; }

	QualifiedName mName;
	ReturnSignature mReturnSignature;
	ParamListSignature mParamListSignature;
	const ConstantTable *mConstantTable;

	union
	{
		const uint8_t *mCode;
		NativeFunction mNativeFunction;
	};

	uint32_t mCodeSize;
	uint32_t mArgSize;
	uint32_t mPackedArgSize;
	uint32_t mLocalSize;
	uint32_t mStackSize;
	uint32_t mFramePointerAlignment;
	bool mUnpackArguments;
};


struct DataEntry
{
	QualifiedName mName;
	void *mData;
};


class CodeSegment
{
public:
	CodeSegment(
			const Function *functionTable,
			size_t functionCount,
			const Function *staticInitializerTable,
			size_t staticInitializerCount,
			const DataEntry *dataTable,
			size_t dataCount):
		mFunctionTable(functionTable),
		mFunctionCount(functionCount),
		mStaticInitializerTable(staticInitializerTable),
		mStaticInitializerCount(staticInitializerCount),
		mDataTable(dataTable),
		mDataCount(dataCount)
	{}

	const Function *GetFunction(const QualifiedName &qualifiedName) const;
	const Function *GetFunction(const char *qualifiedName) const;
	int32_t GetFunctionIndex(const QualifiedName &qualifiedName) const;
	int32_t GetFunctionIndex(const char *qualifiedName) const;
	const Function &GetFunctionAtIndex(uint32_t functionIndex) const { return mFunctionTable[functionIndex]; }
	size_t GetFunctionCount() const { return mFunctionCount; }

	const Function &GetStaticInitializerAtIndex(uint32_t initializerIndex) const { return mStaticInitializerTable[initializerIndex]; }
	size_t GetStaticInitializerCount() const { return mStaticInitializerCount; }

	const DataEntry *GetDataEntry(const QualifiedName &qualifiedName) const;
	const DataEntry *GetDataEntry(const char *qualifiedName) const;
	int32_t GetDataEntryIndex(const QualifiedName &qualifiedName) const;
	int32_t GetDataEntryIndex(const char *qualifiedName) const;
	const DataEntry &GetDataEntryAtIndex(uint32_t dataIndex) const { return mDataTable[dataIndex]; }
	size_t GetDataCount() const { return mDataCount; }

	void CallStaticInitializers(VM &vm) const;

private:
	const Function *mFunctionTable;
	size_t mFunctionCount;
	const Function *mStaticInitializerTable;
	size_t mStaticInitializerCount;
	const DataEntry *mDataTable;
	size_t mDataCount;
};

}

#endif
