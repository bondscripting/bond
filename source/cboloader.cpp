#include "bond/io/memoryoutputstream.h"
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
#include <cstring>

namespace Bond
{

struct CboLoaderResources
{
	CboLoaderResources(
			uint8_t *memory,
			size_t constantTablesStart,
			size_t value32TableStart,
			size_t value64TableStart,
			size_t stringTableStart,
			size_t stringBytesStart,
			size_t qualifiedNameTableStart,
			size_t qualifiedNameElementTableStart,
			size_t paramSignatureStart,
			size_t functionTableStart,
			size_t staticInitializerTableStart,
			size_t codeStart,
			size_t dataTableStart,
			size_t dataStart):
		mConstantTables(reinterpret_cast<ConstantTable *>(memory + constantTablesStart)),
		mValue32Table(reinterpret_cast<Value32 *>(memory + value32TableStart)),
		mValue64Table(reinterpret_cast<Value64 *>(memory + value64TableStart)),
		mStringTable(reinterpret_cast<StringView *>(memory + stringTableStart)),
		mStringBytes(reinterpret_cast<char *>(memory + stringBytesStart)),
		mQualifiedNameTable(reinterpret_cast<QualifiedName *>(memory + qualifiedNameTableStart)),
		mQualifiedNameElementTable(reinterpret_cast<const char **>(memory + qualifiedNameElementTableStart)),
		mParamSignatures(reinterpret_cast<ParamSignature *>(memory + paramSignatureStart)),
		mFunctionTable(reinterpret_cast<Function *>(memory + functionTableStart)),
		mStaticInitializerTable(reinterpret_cast<Function *>(memory + staticInitializerTableStart)),
		mCode(reinterpret_cast<uint8_t *>(memory + codeStart)),
		mDataTable(reinterpret_cast<DataEntry *>(memory + dataTableStart)),
		mData(reinterpret_cast<uint8_t *>(memory + dataStart))
	{}
	ConstantTable *mConstantTables;
	Value32 *mValue32Table;
	Value64 *mValue64Table;
	StringView *mStringTable;
	char *mStringBytes;
	QualifiedName *mQualifiedNameTable;
	const char **mQualifiedNameElementTable;
	ParamSignature *mParamSignatures;
	Function *mFunctionTable;
	Function *mStaticInitializerTable;
	uint8_t *mCode;
	DataEntry *mDataTable;
	uint8_t *mData;
};


class CboLoaderCore
{
public:
	CboLoaderCore(
			const CboValidator::Result &validationResult,
			CboLoaderResources &resources,
			InputStream &stream):
		mValidationResult(validationResult),
		mResources(resources),
		mConstantTable(nullptr),
		mStream(stream)
	{}

	CboLoaderCore(const CboLoaderCore &other) = delete;
	CboLoaderCore &operator=(const CboLoaderCore &other) = delete;

	void Load();

private:
	void LoadBlob();
	void LoadListBlob();
	void LoadFunctionBlob(size_t blobEnd);
	void LoadDataBlob(size_t blobEnd);
	ReturnSignature LoadReturnSignature();
	ParamListSignature LoadParamListSignature();

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	size_t GetPosition() const { return size_t(mStream.GetPosition()); }
	void SetPosition(size_t pos) { mStream.SetPosition(Stream::pos_t(pos)); }
	void Skip(size_t numBytes) { mStream.AddOffset(Stream::off_t(numBytes)); }

	CboValidator::Result mValidationResult;
	CboLoaderResources &mResources;
	ConstantTable *mConstantTable;
	InputStream &mStream;
};


CodeSegmentHandle CboLoader::Load()
{
	typedef Vector<CboValidator::Result> ResultList;
	ResultList resultList(mInputStreamList.size(), CboValidator::Result(), ResultList::allocator_type(&mTempAllocator));

	size_t value32Count = 0;
	size_t value64Count = 0;
	size_t stringCount = 0;
	size_t stringByteCount = 0;
	size_t qualifiedNameCount = 0;
	size_t qualifiedNameElementCount = 0;
	size_t paramSignatureCount = 0;
	size_t functionCount = 0;
	size_t staticInitializerCount = 0;
	size_t codeByteCount = 0;
	size_t dataCount = 0;
	size_t dataSize = 0;
	size_t dataAlignment = size_t(BOND_SLOT_SIZE);

	CboValidator validator;
	auto isit = mInputStreamList.begin();
	for (size_t i = 0; isit != mInputStreamList.end(); ++isit, ++i)
	{
		InputStream &stream = **isit;
		const auto pos = stream.GetPosition();
		CboValidator::Result &result = resultList[i];
		result = validator.Validate(stream);
		stream.SetPosition(pos);
		BOND_ASSERT_FORMAT(result.mPointerSize == BOND_NATIVE_POINTER_SIZE, ("CBO compiled with incompatible pointer size. CBO pointer size %" BOND_PRIu32 ". Native pointer size %" BOND_PRIu32 ".", GetPointerSize(result.mPointerSize), GetPointerSize(BOND_NATIVE_POINTER_SIZE)));
		value32Count += result.mValue32Count;
		value64Count += result.mValue64Count;
		stringCount += result.mStringCount;
		stringByteCount += result.mStringByteCount + result.mStringCount;
		qualifiedNameCount += result.mQualifiedNameCount;
		qualifiedNameElementCount += result.mQualifiedNameElementCount + result.mQualifiedNameCount;
		paramSignatureCount += result.mParamSignatureCount;
		functionCount += result.mFunctionCount;
		staticInitializerCount += result.mStaticInitializerCount;
		codeByteCount += result.mCodeByteCount;
		dataCount += result.mDataCount;
		dataSize = AlignUp(dataSize, result.mDataAlignment);
		dataSize += result.mDataSize;
		dataAlignment = Max(dataAlignment, result.mDataAlignment);
	}

	size_t memSize = 0;
	const size_t codeSegmentStart = TallyMemoryRequirements<CodeSegment>(memSize, 1);
	const size_t constantTablesStart = TallyMemoryRequirements<ConstantTable>(memSize, mInputStreamList.size());
	const size_t value32TableStart = TallyMemoryRequirements<Value32>(memSize, value32Count);
	const size_t value64TableStart = TallyMemoryRequirements<Value64>(memSize, value64Count);
	const size_t stringTableStart = TallyMemoryRequirements<StringView>(memSize, stringCount);
	const size_t stringBytesStart = TallyMemoryRequirements<char>(memSize, stringByteCount);
	const size_t qualifiedNameTableStart = TallyMemoryRequirements<QualifiedName>(memSize, qualifiedNameCount);
	const size_t qualifiedNameElementTableStart = TallyMemoryRequirements<const char *>(memSize, qualifiedNameElementCount);
	const size_t paramSignatureStart = TallyMemoryRequirements<ParamSignature>(memSize, paramSignatureCount);
	const size_t functionTableStart = TallyMemoryRequirements<Function>(memSize, functionCount);
	const size_t staticInitializerTableStart = TallyMemoryRequirements<Function>(memSize, staticInitializerCount);
	const size_t codeStart = TallyMemoryRequirements<const uint8_t>(memSize, codeByteCount, sizeof(Value32));
	const size_t dataTableStart = TallyMemoryRequirements<DataEntry>(memSize, dataCount);
	const size_t dataStart = TallyMemoryRequirements<const uint8_t>(memSize, dataSize, dataAlignment);

	const size_t DEFAULT_ALIGNMENT = 256;
	const size_t alignment = Max(DEFAULT_ALIGNMENT, dataAlignment);

	auto memHandle = mPermAllocator.AllocOwnedAligned<uint8_t>(memSize, alignment);
	memset(memHandle.get(), 0, memSize);

	CboLoaderResources resources(
		memHandle.get(),
		constantTablesStart,
		value32TableStart,
		value64TableStart,
		stringTableStart,
		stringBytesStart,
		qualifiedNameTableStart,
		qualifiedNameElementTableStart,
		paramSignatureStart,
		functionTableStart,
		staticInitializerTableStart,
		codeStart,
		dataTableStart,
		dataStart);

	Function *functionTable = resources.mFunctionTable;
	Function *staticInitializerTable = resources.mStaticInitializerTable;
	DataEntry *dataTable = resources.mDataTable;
	CodeSegment *codeSegment = new (memHandle.get() + codeSegmentStart) CodeSegment(
		functionTable,
		functionCount,
		staticInitializerTable,
		staticInitializerCount,
		dataTable,
		dataCount);

	isit = mInputStreamList.begin();
	for (size_t i = 0; isit != mInputStreamList.end(); ++isit, ++i)
	{
		CboLoaderCore loader(resultList[i], resources, **isit);
		loader.Load();
	}

	auto functionComparator = [](const Function &a, const Function &b) { return a.mName < b.mName; };
	sort(functionTable, functionTable + functionCount, functionComparator);

	auto dataComparator = [](const DataEntry &a, const DataEntry &b) { return a.mName < b.mName; };
	sort(dataTable, dataTable + dataCount, dataComparator);

	for (const NativeBindingCollection *bindingCollection: mNativeBindingList)
	{
		for (uint32_t i = 0; i < bindingCollection->mFunctionBindingCount; ++i)
		{
			BindNativeFunction(bindingCollection->mFunctionBindings[i], *codeSegment);
		}
	}

	for (size_t i = 0; i < staticInitializerCount; ++i)
	{
		ProcessFunction(staticInitializerTable[i], *codeSegment);
	}
	for (size_t i = 0; i < functionCount; ++i)
	{
		ProcessFunction(functionTable[i], *codeSegment);
	}

	memHandle.release();
	return CodeSegmentHandle(mPermAllocator, codeSegment);
}


void CboLoader::BindNativeFunction(const NativeFunctionBinding &binding, const CodeSegment &codeSegment)
{
	Function *function = const_cast<Function *>(codeSegment.GetFunction(binding.mName));
	if (function != nullptr)
	{
		if (function->IsNative())
		{
			function->mNativeFunction = binding.mFunction;
		}
		else
		{
			FunctionIsNotNative(*function);
		}
	}
	else
	{
		UnresolvedQualifiedName(binding.mName);
	}
}


void CboLoader::ProcessFunction(Function &function, const CodeSegment &codeSegment)
{
	if (function.IsNative())
	{
		if (function.mNativeFunction == nullptr)
		{
			FunctionIsNotBound(function);
		}
	}
	else
	{
		uint8_t *code = const_cast<uint8_t *>(function.mCode);
		const uint8_t *codeEnd = code + function.mCodeSize;
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
				case OC_PARAM_STRING:
					ConvertBigEndian16(code);
					code += sizeof(Value16);
					break;
				case OC_PARAM_NAME:
				{
					ConvertBigEndian16(code);
					const QualifiedName &name = function.mConstantTable->mQualifiedNameTable[Value16(code).mUShort];
					int32_t resolvedIndex = -1;

					switch (opCode)
					{
						case OPCODE_LOADEA:
						{
							resolvedIndex = codeSegment.GetDataEntryIndex(name);
						}
						break;
						case OPCODE_INVOKE:
						{
							resolvedIndex = codeSegment.GetFunctionIndex(name);
						}
						break;
						default:
							break;
					}

					if (resolvedIndex >= 0)
					{
						Value16 bytes(resolvedIndex);
						code[0] = bytes.mBytes[0];
						code[1] = bytes.mBytes[1];
						code += sizeof(Value16);
					}
					else
					{
						UnresolvedQualifiedName(name);
					}
				}
				break;
				case OC_PARAM_LOOKUPSWITCH:
				{
					code = AlignPointerUp(code, sizeof(Value32));
					ConvertBigEndian32(code);
					code += sizeof(Value32);

					ConvertBigEndian32(code);
					const uint32_t numMatches = *reinterpret_cast<uint32_t *>(code);
					code += sizeof(Value32);

					for (uint32_t i = 0; i < (2 * numMatches); ++i)
					{
						ConvertBigEndian32(code);
						code += sizeof(Value32);
					}
				}
				break;
				case OC_PARAM_TABLESWITCH:
				{
					code = AlignPointerUp(code, sizeof(Value32));
					ConvertBigEndian32(code);
					code += sizeof(Value32);

					ConvertBigEndian32(code);
					const uint32_t minMatch = *reinterpret_cast<uint32_t *>(code);
					code += sizeof(Value32);

					ConvertBigEndian32(code);
					const uint32_t maxMatch = *reinterpret_cast<uint32_t *>(code);
					code += sizeof(Value32);

					const uint32_t numMatches = maxMatch - minMatch + 1;
					for (uint32_t i = 0; i < numMatches; ++i)
					{
						ConvertBigEndian32(code);
						code += sizeof(Value32);
					}
				}
			}
		}
	}
}


void CboLoader::FunctionIsNotNative(const Function &function) const
{
	char buffer[Exception::MESSAGE_BUFFER_LENGTH];
	MemoryOutputStream stream(buffer, Stream::pos_t(Exception::MESSAGE_BUFFER_LENGTH));
	function.mName.PrintTo(stream);
	BOND_FAIL_FORMAT(("Target function '%s' of native function binding is not native.", buffer));
}


void CboLoader::FunctionIsNotBound(const Function &function) const
{
	char buffer[Exception::MESSAGE_BUFFER_LENGTH];
	MemoryOutputStream stream(buffer, Stream::pos_t(Exception::MESSAGE_BUFFER_LENGTH));
	function.mName.PrintTo(stream);
	BOND_FAIL_FORMAT(("Native function '%s' is not bound.", buffer));
}


void CboLoader::UnresolvedQualifiedName(const QualifiedName &name) const
{
	char buffer[Exception::MESSAGE_BUFFER_LENGTH];
	MemoryOutputStream stream(buffer, Stream::pos_t(Exception::MESSAGE_BUFFER_LENGTH));
	name.PrintTo(stream);
	BOND_FAIL_FORMAT(("Unresolved qualified name '%s'.", buffer));
}


void CboLoader::UnresolvedQualifiedName(const char *name) const
{
	BOND_FAIL_FORMAT(("Unresolved qualified name '%s'.", name));
}


void CboLoaderCore::Load()
{
	mConstantTable = mResources.mConstantTables++;
	mConstantTable->mValue32Table = mResources.mValue32Table;
	mConstantTable->mValue64Table = mResources.mValue64Table;
	mConstantTable->mStringTable = mResources.mStringTable;
	mConstantTable->mQualifiedNameTable = mResources.mQualifiedNameTable;

	// Skip some header information that is already included in the validation result.
	Skip((2 * sizeof(Value32)) + (7 * sizeof(Value16)));

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

	StringView *str = mResources.mStringTable;
	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const size_t length = ReadValue16().mUShort;
		char *buffer = mResources.mStringBytes;
		mStream.Read(buffer, length);
		buffer[length] = '\0';
		mResources.mStringBytes += length + 1;
		*str++ = StringView(buffer, length);
	}
	mResources.mStringTable = str;

	QualifiedName *name = mResources.mQualifiedNameTable;
	const char **element = mResources.mQualifiedNameElementTable;
	for (size_t i = 0; i < mValidationResult.mQualifiedNameCount; ++i)
	{
		*name++ = QualifiedName(element);
		const size_t numElements = ReadValue16().mUShort;
		for (size_t j = 0; j < numElements; ++j)
		{
			const size_t elementIndex = ReadValue16().mUShort;
			*element++ = mConstantTable->mStringTable[elementIndex].data();
		}
		*element++ = nullptr;
	}
	mResources.mQualifiedNameTable = name;
	mResources.mQualifiedNameElementTable = element;

	mResources.mData = AlignPointerUp(mResources.mData, mValidationResult.mDataAlignment);

	LoadBlob();
}


void CboLoaderCore::LoadBlob()
{
	const size_t blobStart = GetPosition();
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	if (idIndex == mValidationResult.mListBlobIdIndex)
	{
		LoadListBlob();
	}
	else if (idIndex == mValidationResult.mFunctionBlobIdIndex)
	{
		LoadFunctionBlob(blobEnd);
	}
	else if (idIndex == mValidationResult.mDataBlobIdIndex)
	{
		LoadDataBlob(blobEnd);
	}
	else
	{
		SetPosition(blobEnd);
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


void CboLoaderCore::LoadFunctionBlob(size_t blobEnd)
{
	const size_t functionNameIndex = ReadValue16().mUShort;
	const bool isStaticInitializer = functionNameIndex == mValidationResult.mStaticInitializerNameIndex;

	Function *function = isStaticInitializer ?
		mResources.mStaticInitializerTable++ : mResources.mFunctionTable++;
	function->mName = mConstantTable->mQualifiedNameTable[functionNameIndex];
	function->mReturnSignature = LoadReturnSignature();
	function->mParamListSignature = LoadParamListSignature();
	function->mConstantTable = mConstantTable;
	function->mArgSize = ReadValue32().mUInt;
	function->mPackedArgSize = ReadValue32().mUInt;
	function->mLocalSize = ReadValue32().mUInt;
	function->mStackSize = ReadValue32().mUInt;
	function->mFramePointerAlignment = ReadValue32().mUInt;

	const uint32_t codeSize = ReadValue32().mUInt;
	function->mCodeSize = codeSize;
	if (codeSize > 0)
	{
		uint8_t *code = mResources.mCode;
		function->mCode = code;
		mStream.Read(code, codeSize);
		mResources.mCode += AlignUp(codeSize, uint32_t(sizeof(Value32)));
	}
	else
	{
		function->mNativeFunction = nullptr;
	}

	bool unpackArguments = function->mFramePointerAlignment > uint32_t(BOND_SLOT_SIZE);
	const uint32_t numParams = function->mParamListSignature.mParamCount;
	const ParamSignature *signatures = function->mParamListSignature.mParamSignatures;
	for (uint32_t i = 0; (i < numParams) && !unpackArguments; ++i)
	{
		const SignatureType type = SignatureType(signatures[i].mType);
		if ((type >= SIG_BOOL) && (type <= SIG_USHORT))
		{
			unpackArguments = true;
		}
	}
	function->mUnpackArguments = unpackArguments;

	// Load the optional metadata blob.
	if (GetPosition() < blobEnd)
	{
		LoadBlob();
	}
}


void CboLoaderCore::LoadDataBlob(size_t blobEnd)
{
	DataEntry *dataEntry = mResources.mDataTable;

	const size_t nameIndex = ReadValue16().mUShort;
	dataEntry->mName = mConstantTable->mQualifiedNameTable[nameIndex];
	const uint32_t sizeAndType = ReadValue32().mUInt;
	uint32_t size;
	SignatureType type;
	DecodeSizeAndType(sizeAndType, size, type);

	const Value32 payload = ReadValue32();

	mResources.mData = AlignPointerUp(mResources.mData, size_t(BOND_SLOT_SIZE));
	dataEntry->mData = mResources.mData;

	switch (type)
	{
		case SIG_BOOL:
		case SIG_CHAR:
		case SIG_UCHAR:
		{
			*reinterpret_cast<uint8_t *>(dataEntry->mData) = uint8_t(payload.mUInt);
		}
		break;

		case SIG_SHORT:
		case SIG_USHORT:
		{
			*reinterpret_cast<uint16_t *>(dataEntry->mData) = uint16_t(payload.mUInt);
		}
		break;

		case SIG_INT:
		case SIG_UINT:
		case SIG_FLOAT:
		{
			*reinterpret_cast<Value32 *>(dataEntry->mData) = payload;
		}
		break;

		case SIG_LONG:
		case SIG_ULONG:
		case SIG_DOUBLE:
		{
			if (payload.mUInt <= 0xffff)
			{
				*reinterpret_cast<Value64 *>(dataEntry->mData) = mConstantTable->mValue64Table[payload.mUInt];
			}
		}
		break;

		case SIG_VOID:
		case SIG_POINTER:
		case SIG_AGGREGATE:
		{
			mResources.mData = AlignPointerUp(mResources.mData, size_t(payload.mUInt));
			dataEntry->mData = mResources.mData;
		}
		break;
	}

	mResources.mData += size;
	++mResources.mDataTable;

	// Load the optional metadata blob.
	if (GetPosition() < blobEnd)
	{
		LoadBlob();
	}
}


ReturnSignature CboLoaderCore::LoadReturnSignature()
{
	const uint32_t returnSizeAndType = ReadValue32().mUInt;
	uint32_t returnSize;
	SignatureType returnType;
	DecodeSizeAndType(returnSizeAndType, returnSize, returnType);
	return ReturnSignature(returnSize, returnType);
}


ParamListSignature CboLoaderCore::LoadParamListSignature()
{
	ParamSignature *params = mResources.mParamSignatures;
	ParamSignature *param = params;
	const uint32_t numParams = ReadValue16().mUShort;
	for (uint32_t i = 0; i < numParams; ++i)
	{
		const int32_t offset = ReadValue32().mInt;
		const uint32_t paramSizeAndType = ReadValue32().mUInt;
		uint32_t paramSize;
		SignatureType paramType;
		DecodeSizeAndType(paramSizeAndType, paramSize, paramType);
		*param++ = ParamSignature(offset, paramSize, paramType);
	}
	mResources.mParamSignatures = param;
	return ParamListSignature(params, numParams);
}


Value16 CboLoaderCore::ReadValue16()
{
	Value16 value;
	mStream.Read(value.mBytes, sizeof(Value16));
	return ConvertBigEndian16(value);
}


Value32 CboLoaderCore::ReadValue32()
{
	Value32 value;
	mStream.Read(value.mBytes, sizeof(Value32));
	return ConvertBigEndian32(value);
}


Value64 CboLoaderCore::ReadValue64()
{
	Value64 value;
	mStream.Read(value.mBytes, sizeof(Value64));
	return ConvertBigEndian64(value);
}

}
