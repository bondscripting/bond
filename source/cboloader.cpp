#include "bond/algorithm.h"
#include "bond/allocator.h"
#include "bond/cboloader.h"
#include "bond/cboutil.h"
#include "bond/cbovalidator.h"
#include "bond/codesegment.h"
#include "bond/endian.h"
#include "bond/fileloader.h"
#include "bond/hashedstring.h"
#include "bond/math.h"
#include "bond/opcodes.h"
#include "bond/value.h"
#include "bond/vector.h"
#include <string.h>

namespace Bond
{

template<typename T>
inline size_t TallyMemoryRequirement(size_t start, size_t count)
{
	return AlignUp(start, sizeof(T)) + (count * sizeof(T));
}


class CboLoaderCore
{
public:
	struct MemoryResources
	{
		MemoryResources(
				unsigned char *memory,
				size_t constantTablesStart,
				size_t value32TableStart,
				size_t value64TableStart,
				size_t stringTableStart,
				size_t stringBytesStart,
				size_t qualifiedIdElementStart,
				size_t paramSignatureStart,
				size_t functionLookupStart,
				size_t functionsStart,
				size_t codeStart):
			mConstantTables(reinterpret_cast<ConstantTable *>(memory + constantTablesStart)),
			mValue32Table(reinterpret_cast<Value32 *>(memory + value32TableStart)),
			mValue64Table(reinterpret_cast<Value64 *>(memory + value64TableStart)),
			mStringTable(reinterpret_cast<HashedString *>(memory + stringTableStart)),
			mStringBytes(reinterpret_cast<char *>(memory + stringBytesStart)),
			mQualifiedIdElements(reinterpret_cast<const HashedString **>(memory + qualifiedIdElementStart)),
			mParamSignatures(reinterpret_cast<ParamSignature *>(memory + paramSignatureStart)),
			mFunctionLookup(reinterpret_cast<bu32_t *>(memory + functionLookupStart)),
			mFunctions(reinterpret_cast<Function *>(memory + functionsStart)),
			mCode(reinterpret_cast<unsigned char *>(memory + codeStart))
		{}
		ConstantTable *mConstantTables;
		Value32 *mValue32Table;
		Value64 *mValue64Table;
		HashedString *mStringTable;
		char *mStringBytes;
		const HashedString **mQualifiedIdElements;
		ParamSignature *mParamSignatures;
		bu32_t *mFunctionLookup;
		Function *mFunctions;
		unsigned char *mCode;
	};

	CboLoaderCore(
			const CboValidator::Result &validationResult,
			MemoryResources &resources,
			const unsigned char *byteCode):
		mValidationResult(validationResult),
		mResources(resources),
		mConstantTable(resources.mConstantTables),
		mStringTable(resources.mStringTable),
		mByteCode(byteCode),
		mIndex(0)
	{}

	void Load();

private:
	void LoadBlob();
	void LoadListBlob();
	void LoadFunctionBlob();
	QualifiedId LoadQualifiedIdentifier();
	ReturnSignature LoadReturnSignature();
	ParamListSignature LoadParamListSignature();

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	CboValidator::Result mValidationResult;
	MemoryResources &mResources;
	ConstantTable *mConstantTable;
	HashedString *mStringTable;
	const unsigned char *mByteCode;
	size_t mIndex;
};


const CodeSegment *CboLoader::Load(const FileData *cboFiles, size_t numFiles)
{
	typedef Vector<CboValidator::Result> ResultList;
	ResultList::Type resultList(numFiles, CboValidator::Result(), ResultList::Allocator(&mTempAllocator));

	size_t value32Count = 0;
	size_t value64Count = 0;
	size_t stringCount = 0;
	size_t stringByteCount = 0;
	size_t qualifiedIdElementCount = 0;
	size_t paramSignatureCount = 0;
	size_t functionCount = 0;
	size_t codeByteCount = 0;

	CboValidator validator;
	for (size_t i = 0; i < numFiles; ++i)
	{
		const FileData &file = cboFiles[i];
		CboValidator::Result &result = resultList[i];
		result = validator.Validate(file.mData, file.mLength);
		if (result.mValidity != CboValidator::CBO_VALID)
		{
			// TODO: Report error somehow.
			return NULL;
		}

		value32Count += result.mValue32Count;
		value64Count += result.mValue64Count;
		stringCount += result.mStringCount;
		stringByteCount += result.mStringByteCount;
		qualifiedIdElementCount += result.mQualifiedIdElementCount;
		paramSignatureCount += result.mParamSignatureCount;
		functionCount += result.mFunctionCount;
		codeByteCount += result.mCodeByteCount;
	}

	const size_t codeSegmentStart = 0;
	const size_t constantTablesStart = TallyMemoryRequirement<CodeSegment>(codeSegmentStart, 1);
	const size_t value32TableStart = TallyMemoryRequirement<ConstantTable>(constantTablesStart, numFiles);
	const size_t value64TableStart = TallyMemoryRequirement<Value32>(value32TableStart, value32Count);
	const size_t stringTableStart = TallyMemoryRequirement<Value64>(value64TableStart, value64Count);
	const size_t stringBytesStart = TallyMemoryRequirement<HashedString>(stringTableStart, stringCount);
	const size_t qualifiedIdElementStart = TallyMemoryRequirement<char>(stringBytesStart, stringByteCount);
	const size_t paramSignatureStart = TallyMemoryRequirement<const HashedString *>(qualifiedIdElementStart, qualifiedIdElementCount);
	const size_t functionLookupStart = TallyMemoryRequirement<ParamSignature>(paramSignatureStart, paramSignatureCount);
	const size_t functionsStart = TallyMemoryRequirement<bu32_t>(functionLookupStart, functionCount);
	const size_t codeStart = TallyMemoryRequirement<Function>(functionsStart, functionCount);
	const size_t totalMemSize = TallyMemoryRequirement<const unsigned char>(codeStart, codeByteCount);

	// TODO: Consider alignment.
	unsigned char *mem = mPermAllocator.Alloc<unsigned char>(totalMemSize);
	CboLoaderCore::MemoryResources resources(
		mem,
		constantTablesStart,
		value32TableStart,
		value64TableStart,
		stringTableStart,
		stringBytesStart,
		qualifiedIdElementStart,
		paramSignatureStart,
		functionLookupStart,
		functionsStart,
		codeStart);

	bu32_t *functionLookup = resources.mFunctionLookup;
	Function *functions = resources.mFunctions;
	CodeSegment *codeSegment = new (mem) CodeSegment(functionLookup, functions, functionCount);

	for (size_t i = 0; i < numFiles; ++i)
	{
		CboLoaderCore loader(resultList[i], resources, cboFiles[i].mData);
		loader.Load();
	}

	// TODO: Report error if there is a hash collision.
	Sort(functions, functions + functionCount, FunctionHashComparator());
	for (size_t i = 0; i < functionCount; ++i)
	{
		functionLookup[i] = functions[i].mHash;
	}

	for (size_t i = 0; i < functionCount; ++i)
	{
		ProcessFunction(functions[i], *codeSegment);
	}

	return codeSegment;
}


void CboLoader::Dispose(const CodeSegment *codeSegment)
{
	mPermAllocator.Free(const_cast<void *>(reinterpret_cast<const void *>(codeSegment)));
}


void CboLoader::ProcessFunction(Function &function, const CodeSegment &codeSegment) const
{
	unsigned char *code = const_cast<unsigned char *>(function.mCode);
	const unsigned char *codeEnd = code + function.mCodeSize;
	while (code < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(*code++);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		switch (param)
		{
			case OC_PARAM_NONE:
			case OC_PARAM_CHAR:
			case OC_PARAM_UCHAR:
			case OC_PARAM_UCHAR_CHAR:
				break;
			case OC_PARAM_SHORT:
			case OC_PARAM_USHORT:
			case OC_PARAM_INT:
			case OC_PARAM_VAL32:
			case OC_PARAM_VAL64:
			case OC_PARAM_OFF16:
			case OC_PARAM_OFF32:
				ConvertBigEndian16(code);
				code += sizeof(Value16);
				break;
			case OC_PARAM_HASH:
			{
				ConvertBigEndian32(code);
				const bu32_t hash = Value32(code).mUInt;
				const void *resolvedPointer = NULL;

				switch (opCode)
				{
					case OPCODE_LOADEA:
						break;
					case OPCODE_INVOKE:
					{
						resolvedPointer = codeSegment.GetFunction(hash);
					}
					break;
					case OPCODE_INVOKENATIVE:
						break;
					default:
						break;
				}

				// TODO: report error if resolvedPointer is NULL.
				memcpy(code, resolvedPointer, BOND_NATIVE_POINTER_SIZE);
				code += BOND_NATIVE_POINTER_SIZE;
			}
			break;
		}
	}
}


bool CboLoader::FunctionHashComparator::operator()(const Function &a, const Function &b) const
{
	return a.mHash < b.mHash;
}


void CboLoaderCore::Load()
{
	mConstantTable->mValue32Table = mResources.mValue32Table;
	mConstantTable->mValue64Table = mResources.mValue64Table;
	mConstantTable->mStringTable = mResources.mStringTable;
	++mResources.mConstantTables;

	// Skip some header information.
	mIndex += 20;

	Value32 *value32 = mResources.mValue32Table;
	for (size_t i = 0; i < mValidationResult.mValue32Count; ++i)
	{
		*value32++ = ReadValue32();
	}
	mResources.mValue32Table = value32;

	Value64 *value64 = mResources.mValue64Table;
	for (size_t i = 0; i < mValidationResult.mValue64Count; ++i)
	{
		*value64++ = ReadValue64();
	}
	mResources.mValue64Table = value64;

	HashedString *str = mResources.mStringTable;
	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const size_t length = ReadValue16().mUShort;
		char *buffer = mResources.mStringBytes;
		memcpy(buffer, mByteCode + mIndex, length);
		mResources.mStringBytes += length;
		mIndex += length;
		*str++ = HashedString(buffer, length);
	}
	mResources.mStringTable = str;

	LoadBlob();
}


void CboLoaderCore::LoadBlob()
{
	const size_t blobStart = mIndex;
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	if (idIndex == mValidationResult.mListBlobIdIndex)
	{
		LoadListBlob();
	}
	else if (idIndex == mValidationResult.mFunctionBlobIdIndex)
	{
		LoadFunctionBlob();
	}
	else
	{
		mIndex = blobEnd;
	}
}


void CboLoaderCore::LoadListBlob()
{
	const size_t numBlobs = ReadValue32().mUInt;
	for (size_t i = 0; i < numBlobs; ++i)
	{
		LoadBlob();
	}
}


void CboLoaderCore::LoadFunctionBlob()
{
	Function *function = mResources.mFunctions;
	function->mReturnSignature = LoadReturnSignature();
	function->mName = LoadQualifiedIdentifier();
	function->mParamListSignature = LoadParamListSignature();
	function->mConstantTable = mConstantTable;
	function->mHash = ReadValue32().mUInt;
	function->mFrameSize = ReadValue32().mUInt;
	function->mPackedFrameSize = ReadValue32().mUInt;
	function->mLocalSize = ReadValue32().mUInt;
	function->mFramePointerAlignment = ReadValue32().mUInt;

	unsigned char *code = mResources.mCode;
	const size_t codeSize = ReadValue32().mUInt;
	function->mCode = code;
	function->mCodeSize = codeSize;
	memcpy(code, mByteCode + mIndex, codeSize);
	++mResources.mFunctions;
	mResources.mCode += codeSize;
}


QualifiedId CboLoaderCore::LoadQualifiedIdentifier()
{
	const HashedString **elements = mResources.mQualifiedIdElements;
	const HashedString **element = elements;
	const bu32_t numElements = ReadValue16().mUShort;
	for (bu32_t i = 0; i < numElements; ++i)
	{
		const size_t idIndex = ReadValue16().mUShort;
		*element++ = mStringTable + idIndex;
	}
	mResources.mQualifiedIdElements = element;
	return QualifiedId(elements, numElements);
}


ReturnSignature CboLoaderCore::LoadReturnSignature()
{
	const bu32_t returnSizeAndType = ReadValue32().mUInt;
	bu32_t returnSize;
	bu32_t returnType;
	DecodeSizeAndType(returnSizeAndType, returnSize, returnType);
	return ReturnSignature(returnSize, returnType);
}


ParamListSignature CboLoaderCore::LoadParamListSignature()
{
	ParamSignature *params = mResources.mParamSignatures;
	ParamSignature *param = params;
	const bu32_t numParams = ReadValue16().mUShort;
	for (bu32_t i = 0; i < numParams; ++i)
	{
		const bi32_t offset = ReadValue32().mInt;
		const bu32_t paramSizeAndType = ReadValue32().mUInt;
		bu32_t paramSize;
		bu32_t paramType;
		DecodeSizeAndType(paramSizeAndType, paramSize, paramType);
		*param++ = ParamSignature(offset, paramSize, paramType);
	}
	mResources.mParamSignatures = param;
	return ParamListSignature(params, numParams);
}


Value16 CboLoaderCore::ReadValue16()
{
	const Value16 value(mByteCode + mIndex);
	mIndex += sizeof(Value16);
	return ConvertBigEndian16(value);
}


Value32 CboLoaderCore::ReadValue32()
{
	const Value32 value(mByteCode + mIndex);
	mIndex += sizeof(Value32);
	return ConvertBigEndian32(value);
}


Value64 CboLoaderCore::ReadValue64()
{
	const Value64 value(mByteCode + mIndex);
	mIndex += sizeof(Value64);
	return ConvertBigEndian64(value);
}

}
