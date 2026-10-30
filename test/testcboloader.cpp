#include "framework/testvmframework.h"
#include "bond/api/libruntime.h"
#include "bond/systems/defaultallocator.h"
#include "bond/tools/cboutil.h"
#include "bond/types/opcodes.h"
#include "bond/types/value.h"
#include "bond/version.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
#include <cstring>

// Offsets of fields in the CBO header and constant table. See docs/cbo-file-format.txt.
const size_t MAGIC_NUMBER_OFFSET = 0;
const size_t MAJOR_VERSION_OFFSET = 4;
const size_t FLAGS_OFFSET = 8;
const size_t TABLE_SIZE_OFFSET = 10;
const size_t VALUE32_COUNT_OFFSET = 14;
const size_t VALUE64_COUNT_OFFSET = 16;
const size_t STRING_COUNT_OFFSET = 18;
const size_t QUALIFIED_NAME_COUNT_OFFSET = 20;
const size_t VALUE32_TABLE_OFFSET = 22;

const char *const SCRIPT_NAME = "scripts/vm_CboLoader.bond";
const char *const DATA_SCRIPT_NAME = "scripts/vm_CboLoaderData.bond";


struct Cbo
{
	uint8_t mBytes[TestFramework::MAX_CBO_SIZE];
	size_t mSize;
};


struct FunctionBlob
{
	size_t mBlobOffset;
	size_t mNameOffset;
	size_t mArgSizeOffset;
	size_t mCodeSizeOffset;
	size_t mCodeOffset;
	size_t mCodeSize;
};


uint32_t ReadBE16(const Cbo &cbo, size_t offset)
{
	return (uint32_t(cbo.mBytes[offset]) << 8) | uint32_t(cbo.mBytes[offset + 1]);
}


uint32_t ReadBE32(const Cbo &cbo, size_t offset)
{
	return (ReadBE16(cbo, offset) << 16) | ReadBE16(cbo, offset + 2);
}


void WriteBE16(Cbo &cbo, size_t offset, uint32_t value)
{
	cbo.mBytes[offset] = uint8_t(value >> 8);
	cbo.mBytes[offset + 1] = uint8_t(value);
}


void WriteBE32(Cbo &cbo, size_t offset, uint32_t value)
{
	WriteBE16(cbo, offset, value >> 16);
	WriteBE16(cbo, offset + 2, value);
}


void CompileCbo(const char *assertFile, int assertLine, const char *scriptName, Cbo &cbo)
{
	cbo.mSize = TestFramework::CompileScriptToCbo(assertFile, assertLine, scriptName, cbo.mBytes, sizeof(cbo.mBytes));
}


size_t FindStringIndex(const Cbo &cbo, const Bond::StringView &str)
{
	const size_t stringCount = ReadBE16(cbo, STRING_COUNT_OFFSET);
	size_t offset = VALUE32_TABLE_OFFSET +
		(ReadBE16(cbo, VALUE32_COUNT_OFFSET) * sizeof(Bond::Value32)) +
		(ReadBE16(cbo, VALUE64_COUNT_OFFSET) * sizeof(Bond::Value64));

	for (size_t i = 0; i < stringCount; ++i)
	{
		const size_t length = ReadBE16(cbo, offset);
		offset += sizeof(Bond::Value16);
		if (Bond::StringView(reinterpret_cast<const char *>(cbo.mBytes + offset), length) == str)
		{
			return i;
		}
		offset += length;
	}
	return stringCount;
}


size_t GetListBlobOffset(const Cbo &cbo)
{
	return TABLE_SIZE_OFFSET + ReadBE32(cbo, TABLE_SIZE_OFFSET);
}


/// \brief Locates the index'th function blob in the CBO's top level list blob. Returns false if
/// there are not that many function blobs.
bool GetFunctionBlob(const Cbo &cbo, size_t index, FunctionBlob &blob)
{
	const size_t functionIdIndex = FindStringIndex(cbo, Bond::BOND_FUNCTION_BLOB_ID);
	const size_t listBlobOffset = GetListBlobOffset(cbo);
	const size_t numBlobs = ReadBE32(cbo, listBlobOffset + 6);
	size_t offset = listBlobOffset + 10;

	for (size_t i = 0; i < numBlobs; ++i)
	{
		if (ReadBE16(cbo, offset + 4) == functionIdIndex)
		{
			if (index == 0)
			{
				const size_t numParams = ReadBE16(cbo, offset + 12);
				const size_t argSizeOffset = offset + 14 + (numParams * 2 * sizeof(Bond::Value32));
				blob.mBlobOffset = offset;
				blob.mNameOffset = offset + 6;
				blob.mArgSizeOffset = argSizeOffset;
				blob.mCodeSizeOffset = argSizeOffset + (5 * sizeof(Bond::Value32));
				blob.mCodeOffset = blob.mCodeSizeOffset + sizeof(Bond::Value32);
				blob.mCodeSize = ReadBE32(cbo, blob.mCodeSizeOffset);
				return true;
			}
			--index;
		}
		offset += ReadBE32(cbo, offset);
	}
	return false;
}


/// \brief Returns the first function blob in the CBO.
FunctionBlob GetFirstFunctionBlob(const char *assertFile, int assertLine, const Cbo &cbo)
{
	FunctionBlob blob;
	if (!GetFunctionBlob(cbo, 0, blob))
	{
		throw TestFramework::AssertionFailure(assertFile, assertLine, "CBO contains no function blobs.");
	}
	return blob;
}


/// \brief Returns the first function blob that contains bytecode.
FunctionBlob GetFirstScriptFunctionBlob(const char *assertFile, int assertLine, const Cbo &cbo)
{
	FunctionBlob blob;
	for (size_t i = 0; GetFunctionBlob(cbo, i, blob); ++i)
	{
		if (blob.mCodeSize > 0)
		{
			return blob;
		}
	}
	throw TestFramework::AssertionFailure(assertFile, assertLine, "CBO contains no function with bytecode.");
}


/// \brief Returns the offset of the operand of the first instruction in the CBO whose operand
/// is of the given type.
size_t FindOperand(const char *assertFile, int assertLine, const Cbo &cbo, Bond::OpCodeParam operandType)
{
	FunctionBlob blob;
	for (size_t i = 0; GetFunctionBlob(cbo, i, blob); ++i)
	{
		const size_t codeEnd = blob.mCodeOffset + blob.mCodeSize;
		size_t offset = blob.mCodeOffset;
		while (offset < codeEnd)
		{
			const Bond::OpCodeParam param = Bond::GetOpCodeParamType(Bond::OpCode(cbo.mBytes[offset++]));
			if (param == operandType)
			{
				return offset;
			}

			switch (param)
			{
				case Bond::OC_PARAM_NONE:
					break;
				case Bond::OC_PARAM_CHAR:
				case Bond::OC_PARAM_UCHAR:
					offset += 1;
					break;
				case Bond::OC_PARAM_CHAR_CHAR:
				case Bond::OC_PARAM_SHORT:
				case Bond::OC_PARAM_USHORT:
				case Bond::OC_PARAM_INT:
				case Bond::OC_PARAM_VAL32:
				case Bond::OC_PARAM_VAL64:
				case Bond::OC_PARAM_OFF16:
				case Bond::OC_PARAM_OFF32:
				case Bond::OC_PARAM_STRING:
				case Bond::OC_PARAM_NAME:
					offset += sizeof(Bond::Value16);
					break;
				case Bond::OC_PARAM_LOOKUPSWITCH:
				{
					offset = blob.mCodeOffset + Bond::AlignUp(offset - blob.mCodeOffset, sizeof(Bond::Value32));
					const size_t numMatches = ReadBE32(cbo, offset + sizeof(Bond::Value32));
					offset += (2 + (2 * numMatches)) * sizeof(Bond::Value32);
				}
				break;
				case Bond::OC_PARAM_TABLESWITCH:
				{
					offset = blob.mCodeOffset + Bond::AlignUp(offset - blob.mCodeOffset, sizeof(Bond::Value32));
					const int32_t minMatch = int32_t(ReadBE32(cbo, offset + sizeof(Bond::Value32)));
					const int32_t maxMatch = int32_t(ReadBE32(cbo, offset + (2 * sizeof(Bond::Value32))));
					offset += (3 + size_t(maxMatch - minMatch + 1)) * sizeof(Bond::Value32);
				}
				break;
			}
		}
	}
	throw TestFramework::AssertionFailure(assertFile, assertLine, "CBO contains no operand of type %d.", int(operandType));
}


void UnboundNative(Bond::StackFrame &frame)
{
	frame.SetReturnValue(int32_t(0));
}


const Bond::NativeFunctionBinding UNBOUND_NATIVE_BINDINGS[] =
{
	{"UnboundNative", &UnboundNative},
};

const Bond::NativeBindingCollection UNBOUND_NATIVE_BINDING_COLLECTION =
{
	UNBOUND_NATIVE_BINDINGS,
	1
};


struct LoadOptions
{
	size_t mNumCopies;
	bool mLoadLibs;
	const Bond::NativeBindingCollection *mBinding;
};

// Everything the script needs, so that loading fails only because of the corruption under test.
const LoadOptions DEFAULT_LOAD_OPTIONS = {1, true, &UNBOUND_NATIVE_BINDING_COLLECTION};


void LoadCbo(Bond::Allocator &allocator, const Cbo &cbo, const LoadOptions &options)
{
	Bond::CboLoader cboLoader(allocator);
	if (options.mLoadLibs)
	{
		Bond::LoadAllLibs(cboLoader);
	}
	if (options.mBinding != nullptr)
	{
		cboLoader.AddNativeBinding(*options.mBinding);
	}
	for (size_t i = 0; i < options.mNumCopies; ++i)
	{
		cboLoader.AddCboFile(cbo.mBytes, cbo.mSize);
	}
	Bond::CodeSegmentHandle codeSegmentHandle = cboLoader.Load();
}


void AssertLoadSucceeds(const char *assertFile, int assertLine, const Cbo &cbo, const LoadOptions &options)
{
	Bond::DefaultAllocator allocator;
	TestFramework::AllocationLeakGuard<Bond::DefaultAllocator> leakGuard(allocator, "CBO loader", assertFile, assertLine);
	try
	{
		LoadCbo(allocator, cbo, options);
	}
	catch (const Bond::Exception &e)
	{
		throw TestFramework::AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}


void AssertLoadFails(
	const char *assertFile,
	int assertLine,
	const char *expectedPrefix,
	const Cbo &cbo,
	const LoadOptions &options)
{
	Bond::DefaultAllocator allocator;
	TestFramework::AllocationLeakGuard<Bond::DefaultAllocator> leakGuard(allocator, "CBO loader", assertFile, assertLine);
	TestFramework::AssertThrowsBondException(assertFile, assertLine, expectedPrefix,
		[&]() { LoadCbo(allocator, cbo, options); });
}


// Thin pass-through macros that capture __FILE__/__LINE__.
#define COMPILE_CBO(scriptName, cbo)                                                           \
  CompileCbo(__FILE__, __LINE__, (scriptName), (cbo))

#define ASSERT_LOAD_SUCCEEDS(cbo, options)                                                     \
  AssertLoadSucceeds(__FILE__, __LINE__, (cbo), (options))

#define ASSERT_LOAD_FAILS(expectedPrefix, cbo, options)                                        \
  AssertLoadFails(__FILE__, __LINE__, (expectedPrefix), (cbo), (options))


const char *const MALFORMED_CBO = "CBO file is incomplete or malformed.";
const char *const INVALID_BYTECODE = "CBO file contains invalid bytecode";


DEFINE_TEST(ValidCboLoads)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	ASSERT_LOAD_SUCCEEDS(cbo, DEFAULT_LOAD_OPTIONS);

	Cbo dataCbo;
	COMPILE_CBO(DATA_SCRIPT_NAME, dataCbo);
	const LoadOptions dataOptions = {1, false, nullptr};
	ASSERT_LOAD_SUCCEEDS(dataCbo, dataOptions);
}


DEFINE_TEST(Truncation)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const size_t size = cbo.mSize;

	for (size_t length = 0; length < size; ++length)
	{
		cbo.mSize = length;
		ASSERT_LOAD_FAILS(nullptr, cbo, DEFAULT_LOAD_OPTIONS);
	}
}


DEFINE_TEST(InvalidMagicNumber)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	cbo.mBytes[MAGIC_NUMBER_OFFSET] ^= 0xff;
	ASSERT_LOAD_FAILS("CBO file contains invalid magic number", cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(UnsupportedVersion)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	WriteBE16(cbo, MAJOR_VERSION_OFFSET, uint32_t(Bond::CBO_MAX_SUPPORTED_MAJOR_VERSION) + 1);
	ASSERT_LOAD_FAILS("Unsupported CBO file format version", cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(IncompatiblePointerSize)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	WriteBE16(cbo, FLAGS_OFFSET, ReadBE16(cbo, FLAGS_OFFSET) ^ 1);
	ASSERT_LOAD_FAILS("CBO compiled with incompatible pointer size", cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(BadTableSize)
{
	Cbo original;
	COMPILE_CBO(SCRIPT_NAME, original);
	const uint32_t tableSize = ReadBE32(original, TABLE_SIZE_OFFSET);

	Cbo cbo = original;
	WriteBE32(cbo, TABLE_SIZE_OFFSET, tableSize + 1);
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);

	cbo = original;
	WriteBE32(cbo, TABLE_SIZE_OFFSET, tableSize - 1);
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(BadBlobSize)
{
	Cbo original;
	COMPILE_CBO(SCRIPT_NAME, original);

	Cbo cbo = original;
	const size_t listBlobOffset = GetListBlobOffset(cbo);
	WriteBE32(cbo, listBlobOffset, ReadBE32(cbo, listBlobOffset) + 1);
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);

	cbo = original;
	const FunctionBlob blob = GetFirstFunctionBlob(__FILE__, __LINE__, cbo);
	WriteBE32(cbo, blob.mBlobOffset, ReadBE32(cbo, blob.mBlobOffset) - 1);
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(BadCodeSize)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const FunctionBlob blob = GetFirstScriptFunctionBlob(__FILE__, __LINE__, cbo);
	WriteBE32(cbo, blob.mCodeSizeOffset, uint32_t(blob.mCodeSize) + 1);
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(InvalidFunctionBlob)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const FunctionBlob blob = GetFirstFunctionBlob(__FILE__, __LINE__, cbo);
	WriteBE32(cbo, blob.mArgSizeOffset, ReadBE32(cbo, blob.mArgSizeOffset) + 1);
	ASSERT_LOAD_FAILS("CBO file contains an invalid function blob", cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(ConstantIndexOutOfRange)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const size_t operandOffset = FindOperand(__FILE__, __LINE__, cbo, Bond::OC_PARAM_VAL32);
	WriteBE16(cbo, operandOffset, ReadBE16(cbo, VALUE32_COUNT_OFFSET));
	ASSERT_LOAD_FAILS(INVALID_BYTECODE, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(NameIndexOutOfRange)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const size_t operandOffset = FindOperand(__FILE__, __LINE__, cbo, Bond::OC_PARAM_NAME);
	WriteBE16(cbo, operandOffset, ReadBE16(cbo, QUALIFIED_NAME_COUNT_OFFSET));
	ASSERT_LOAD_FAILS(INVALID_BYTECODE, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(FunctionNameIndexOutOfRange)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const FunctionBlob blob = GetFirstFunctionBlob(__FILE__, __LINE__, cbo);
	WriteBE16(cbo, blob.mNameOffset, ReadBE16(cbo, QUALIFIED_NAME_COUNT_OFFSET));
	ASSERT_LOAD_FAILS(MALFORMED_CBO, cbo, DEFAULT_LOAD_OPTIONS);
}


DEFINE_TEST(UnboundNativeFunction)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const LoadOptions options = {1, true, nullptr};
	ASSERT_LOAD_FAILS("Native function 'UnboundNative' is not bound.", cbo, options);
}


DEFINE_TEST(UnresolvedCrossCboReference)
{
	// The script calls Bond.SqrtD, which is defined in the math library's CBO.
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const LoadOptions options = {1, false, &UNBOUND_NATIVE_BINDING_COLLECTION};
	ASSERT_LOAD_FAILS("Unresolved qualified name 'Bond.SqrtD'.", cbo, options);
}


DEFINE_TEST(FunctionDefinedTwice)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const LoadOptions options = {2, true, nullptr};
	ASSERT_LOAD_FAILS("Function '", cbo, options);
}


DEFINE_TEST(DataDefinedTwice)
{
	Cbo cbo;
	COMPILE_CBO(DATA_SCRIPT_NAME, cbo);
	const LoadOptions options = {2, false, nullptr};
	ASSERT_LOAD_FAILS("Data 'Global' is defined multiple times.", cbo, options);
}


DEFINE_TEST(BindingTargetNotNative)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const Bond::NativeFunctionBinding bindings[] = {{"GetLargeConstant", &UnboundNative}};
	const Bond::NativeBindingCollection bindingCollection = {bindings, 1};
	const LoadOptions options = {1, true, &bindingCollection};
	ASSERT_LOAD_FAILS("Target function 'GetLargeConstant' of native function binding is not native.", cbo, options);
}


DEFINE_TEST(BindingWithNoFunction)
{
	Cbo cbo;
	COMPILE_CBO(SCRIPT_NAME, cbo);
	const Bond::NativeFunctionBinding bindings[] = {{"NoSuchFunction", &UnboundNative}};
	const Bond::NativeBindingCollection bindingCollection = {bindings, 1};
	const LoadOptions options = {1, true, &bindingCollection};
	ASSERT_LOAD_FAILS("Unresolved qualified name 'NoSuchFunction'.", cbo, options);
}


#define TEST_ITEMS                              \
  TEST_ITEM(ValidCboLoads)                      \
  TEST_ITEM(Truncation)                         \
  TEST_ITEM(InvalidMagicNumber)                 \
  TEST_ITEM(UnsupportedVersion)                 \
  TEST_ITEM(IncompatiblePointerSize)            \
  TEST_ITEM(BadTableSize)                       \
  TEST_ITEM(BadBlobSize)                        \
  TEST_ITEM(BadCodeSize)                        \
  TEST_ITEM(InvalidFunctionBlob)                \
  TEST_ITEM(ConstantIndexOutOfRange)            \
  TEST_ITEM(NameIndexOutOfRange)                \
  TEST_ITEM(FunctionNameIndexOutOfRange)        \
  TEST_ITEM(UnboundNativeFunction)              \
  TEST_ITEM(UnresolvedCrossCboReference)        \
  TEST_ITEM(FunctionDefinedTwice)               \
  TEST_ITEM(DataDefinedTwice)                   \
  TEST_ITEM(BindingTargetNotNative)             \
  TEST_ITEM(BindingWithNoFunction)              \

RUN_TESTS(CboLoader, TEST_ITEMS)
