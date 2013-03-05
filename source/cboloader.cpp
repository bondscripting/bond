#include "bond/io/fileloader.h"
#include "bond/io/textwriter.h"
#include "bond/stl/algorithm.h"
#include "bond/stl/vector.h"
#include "bond/systems/allocator.h"
#include "bond/systems/assert.h"
#include "bond/systems/endian.h"
#include "bond/tools/cboutil.h"
#include "bond/tools/cbovalidator.h"
#include "bond/types/opcodes.h"
#include "bond/types/value.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
#include "private/memory.h"
#include <string.h>

namespace Bond
{

class CboLoaderCore
{
public:
	struct MemoryResources
	{
		MemoryResources(
				bu8_t *memory,
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
			mStringTable(reinterpret_cast<SimpleString *>(memory + stringTableStart)),
			mStringBytes(reinterpret_cast<char *>(memory + stringBytesStart)),
			mQualifiedIdElements(reinterpret_cast<const SimpleString **>(memory + qualifiedIdElementStart)),
			mParamSignatures(reinterpret_cast<ParamSignature *>(memory + paramSignatureStart)),
			mFunctionLookup(reinterpret_cast<bu32_t *>(memory + functionLookupStart)),
			mFunctions(reinterpret_cast<Function *>(memory + functionsStart)),
			mCode(reinterpret_cast<bu8_t *>(memory + codeStart))
		{}
		ConstantTable *mConstantTables;
		Value32 *mValue32Table;
		Value64 *mValue64Table;
		SimpleString *mStringTable;
		char *mStringBytes;
		const SimpleString **mQualifiedIdElements;
		ParamSignature *mParamSignatures;
		bu32_t *mFunctionLookup;
		Function *mFunctions;
		bu8_t *mCode;
	};

	CboLoaderCore(
			const CboValidator::Result &validationResult,
			MemoryResources &resources,
			const bu8_t *byteCode):
		mValidationResult(validationResult),
		mResources(resources),
		mConstantTable(resources.mConstantTables),
		mStringTable(resources.mStringTable),
		mByteCode(static_cast<const bu8_t *>(byteCode)),
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
	SimpleString *mStringTable;
	const bu8_t *mByteCode;
	size_t mIndex;
};


CboLoader::Handle CboLoader::Load(const FileData *cboFiles, size_t numFiles)
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
		result = validator.Validate(reinterpret_cast<const bu8_t *>(file.mData), file.mLength);
		value32Count += result.mValue32Count;
		value64Count += result.mValue64Count;
		stringCount += result.mStringCount;
		stringByteCount += result.mStringByteCount + result.mStringCount;
		qualifiedIdElementCount += result.mQualifiedIdElementCount;
		paramSignatureCount += result.mParamSignatureCount;
		functionCount += result.mFunctionCount;
		codeByteCount += result.mCodeByteCount;
	}

	size_t memSize = 0;
	const size_t codeSegmentStart = TallyMemoryRequirements<CodeSegment>(memSize, 1);
	const size_t constantTablesStart = TallyMemoryRequirements<ConstantTable>(memSize, numFiles);
	const size_t value32TableStart = TallyMemoryRequirements<Value32>(memSize, value32Count);
	const size_t value64TableStart = TallyMemoryRequirements<Value64>(memSize, value64Count);
	const size_t stringTableStart = TallyMemoryRequirements<SimpleString>(memSize, stringCount);
	const size_t stringBytesStart = TallyMemoryRequirements<char>(memSize, stringByteCount);
	const size_t qualifiedIdElementStart = TallyMemoryRequirements<const SimpleString *>(memSize, qualifiedIdElementCount);
	const size_t paramSignatureStart = TallyMemoryRequirements<ParamSignature>(memSize, paramSignatureCount);
	const size_t functionLookupStart = TallyMemoryRequirements<bu32_t>(memSize, functionCount);
	const size_t functionsStart = TallyMemoryRequirements<Function>(memSize, functionCount);
	const size_t codeStart = TallyMemoryRequirements<const bu8_t>(memSize, codeByteCount, sizeof(Value32));

	const size_t CBO_ALIGNMENT = 256;
	Allocator::Handle<bu8_t> memHandle(mPermAllocator, mPermAllocator.AllocAligned<bu8_t>(memSize, CBO_ALIGNMENT));
	CboLoaderCore::MemoryResources resources(
		memHandle.Get(),
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
	CodeSegment *codeSegment = new (memHandle.Get() + codeSegmentStart) CodeSegment(functionLookup, functions, functionCount);

	for (size_t i = 0; i < numFiles; ++i)
	{
		CboLoaderCore loader(resultList[i], resources, reinterpret_cast<const bu8_t *>(cboFiles[i].mData));
		loader.Load();
	}

	Sort(functions, functions + functionCount, FunctionHashComparator());
	for (size_t i = 0; i < functionCount; ++i)
	{
		functionLookup[i] = functions[i].mHash;
		if ((i > 0) && (functionLookup[i] == functionLookup[i - 1]))
		{
			HashCollision(functionLookup[i]);
		}
	}

	for (size_t i = 0; i < functionCount; ++i)
	{
		ProcessFunction(functions[i], *codeSegment);
	}

	memHandle.Release();
	return Handle(mPermAllocator, codeSegment);
}


void CboLoader::Dispose(const CodeSegment *codeSegment)
{
	mPermAllocator.FreeAligned(const_cast<void *>(reinterpret_cast<const void *>(codeSegment)));
}


void CboLoader::ProcessFunction(Function &function, const CodeSegment &codeSegment)
{
	bu8_t *code = const_cast<bu8_t *>(function.mCode);
	const bu8_t *codeEnd = code + function.mCodeSize;
	while (code < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(*code++);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
			case OC_PARAM_UCHAR:
				++code;
				break;
			case OC_PARAM_UCHAR_CHAR:
				code += 2;
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

				if (resolvedPointer != NULL)
				{
					const bu32_t pointerSize = GetPointerSize(BOND_NATIVE_POINTER_SIZE);
					memcpy(code, &resolvedPointer, pointerSize);
					code += pointerSize;
				}
				else
				{
					UnresolvedHash(hash);
				}
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				code = static_cast<bu8_t *>(AlignPointerUp(code, sizeof(Value32)));
				ConvertBigEndian32(code);
				code += sizeof(Value32);

				ConvertBigEndian32(code);
				const bu32_t numMatches = *reinterpret_cast<bu32_t *>(code);
				code += sizeof(Value32);

				for (bu32_t i = 0; i < (2 * numMatches); ++i)
				{
					ConvertBigEndian32(code);
					code += sizeof(Value32);
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				code = static_cast<bu8_t *>(AlignPointerUp(code, sizeof(Value32)));
				ConvertBigEndian32(code);
				code += sizeof(Value32);

				ConvertBigEndian32(code);
				const bu32_t minMatch = *reinterpret_cast<bu32_t *>(code);
				code += sizeof(Value32);

				ConvertBigEndian32(code);
				const bu32_t maxMatch = *reinterpret_cast<bu32_t *>(code);
				code += sizeof(Value32);

				const bu32_t numMatches = maxMatch - minMatch + 1;
				for (bu32_t i = 0; i < numMatches; ++i)
				{
					ConvertBigEndian32(code);
					code += sizeof(Value32);
				}
			}
		}
	}
}


void CboLoader::UnresolvedHash(bu32_t hash) const
{
	BOND_FAIL_FORMAT(("Unresolved hash 0x%" BOND_PRIx32 ".", hash));
}


void CboLoader::HashCollision(bu32_t hash) const
{
	BOND_FAIL_FORMAT(("Hash collision 0x%" BOND_PRIx32 ".", hash));
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

	SimpleString *str = mResources.mStringTable;
	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const size_t length = ReadValue16().mUShort;
		char *buffer = mResources.mStringBytes;
		memcpy(buffer, mByteCode + mIndex, length);
		buffer[length] = '\0';
		mResources.mStringBytes += length + 1;
		mIndex += length;
		*str++ = SimpleString(buffer, length);
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
	function->mArgSize = ReadValue32().mUInt;
	function->mPackedArgSize = ReadValue32().mUInt;
	function->mLocalSize = ReadValue32().mUInt;
	function->mStackSize = ReadValue32().mUInt;
	function->mFramePointerAlignment = ReadValue32().mUInt;

	bu8_t *code = mResources.mCode;
	const bu32_t codeSize = ReadValue32().mUInt;
	function->mCode = code;
	function->mCodeSize = codeSize;
	memcpy(code, mByteCode + mIndex, codeSize);
	++mResources.mFunctions;
	mResources.mCode += AlignUp(codeSize, bu32_t(sizeof(Value32)));
	mIndex += codeSize;
}


QualifiedId CboLoaderCore::LoadQualifiedIdentifier()
{
	const SimpleString **elements = mResources.mQualifiedIdElements;
	const SimpleString **element = elements;
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
