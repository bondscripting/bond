#include "bond/io/textwriter.h"
#include "bond/stl/vector.h"
#include "bond/systems/endian.h"
#include "bond/systems/math.h"
#include "bond/tools/cboutil.h"
#include "bond/tools/cbovalidator.h"
#include "bond/tools/disassembler.h"
#include "bond/types/opcodes.h"
#include "bond/types/simplestring.h"
#include "bond/types/value.h"
#include "bond/version.h"

namespace Bond
{

class DisassemblerCore
{
public:
	DisassemblerCore(
			const CboValidator::Result &validationResult,
			Allocator &allocator,
			TextWriter &writer,
			const bu8_t *byteCode):
		mValue32Table(validationResult.mValue32Count, Value32(), Value32Table::Allocator(&allocator)),
		mValue64Table(validationResult.mValue64Count, Value64(), Value64Table::Allocator(&allocator)),
		mStringTable(validationResult.mStringCount, SimpleString(), StringTable::Allocator(&allocator)),
		mValidationResult(validationResult),
		mAllocator(allocator),
		mWriter(writer),
		mByteCode(byteCode),
		mIndex(0)
	{}

	void Disassemble();

private:
	typedef Vector<Value32> Value32Table;
	typedef Vector<Value64> Value64Table;
	typedef Vector<SimpleString> StringTable;

	void DisassembleBlob();
	void DisassembleListBlob();
	void DisassembleFunctionBlob();
	void DisassembleQualifiedIdentifier();
	void DisassembleParamListSignature();
	void DisassembleSizeAndType();
	void WriteSimpleString(const SimpleString &str);

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	Value32Table::Type mValue32Table;
	Value64Table::Type mValue64Table;
	StringTable::Type mStringTable;
	CboValidator::Result mValidationResult;
	Allocator &mAllocator;
	TextWriter &mWriter;
	const bu8_t *mByteCode;
	size_t mIndex;
};


void Disassembler::Disassemble(TextWriter &writer, const void *byteCode, size_t length)
{
	CboValidator validator;
	CboValidator::Result result = validator.Validate(byteCode, length);
	DisassemblerCore disassembler(result, mAllocator, writer, static_cast<const bu8_t *>(byteCode));
	disassembler.Disassemble();
}


void DisassemblerCore::Disassemble()
{
	// Skip some header information.
	mIndex += 20;

	mWriter.Write("Version %d.%02d\n", mValidationResult.mMajorVersion, mValidationResult.mMinorVersion);
	mWriter.Write("Pointer size: %d bits\n", (mValidationResult.mPointerSize == POINTER_64BIT) ? 64 : 32);

	for (size_t i = 0; i < mValidationResult.mValue32Count; ++i)
	{
		mValue32Table[i] = ReadValue32();
	}

	for (size_t i = 0; i < mValidationResult.mValue64Count; ++i)
	{
		mValue64Table[i] = ReadValue64();
	}

	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const size_t length = ReadValue16().mUShort;
		mStringTable[i] = SimpleString(reinterpret_cast<const char *>(mByteCode + mIndex), length);
		mIndex += length;
	}

	DisassembleBlob();
}


void DisassemblerCore::DisassembleBlob()
{
	const size_t blobStart = mIndex;
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	if (idIndex == mValidationResult.mListBlobIdIndex)
	{
		DisassembleListBlob();
	}
	else if (idIndex == mValidationResult.mFunctionBlobIdIndex)
	{
		DisassembleFunctionBlob();
	}
	else
	{
		mIndex = blobEnd;
	}
}


void DisassemblerCore::DisassembleListBlob()
{
	const size_t numBlobs = ReadValue32().mUInt;
	for (size_t i = 0; i < numBlobs; ++i)
	{
		DisassembleBlob();
	}
}


void DisassemblerCore::DisassembleFunctionBlob()
{
	mWriter.Write("Function: ");
	DisassembleSizeAndType();
	mWriter.Write(" ");
	DisassembleQualifiedIdentifier();
	mWriter.Write("(");
	DisassembleParamListSignature();
	mWriter.Write(")\n");

	const bu32_t functionHash = ReadValue32().mUInt;
	const bu32_t argSize = ReadValue32().mUInt;
	const bu32_t packedArgSize = ReadValue32().mUInt;
	const bu32_t localSize = ReadValue32().mUInt;
	const bu32_t stackSize = ReadValue32().mUInt;
	const bu32_t framePointerAlignment = ReadValue32().mUInt;
	const bu32_t codeSize = ReadValue32().mUInt;
	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;
	mWriter.Write(
		"\thash: 0x%" BOND_PRIx32 "\n"
		"\targ size: %" BOND_PRIu32 "\n"
		"\tpacked arg size: %" BOND_PRIu32 "\n"
		"\tlocal size: %" BOND_PRIu32 "\n"
		"\tstack size: %" BOND_PRIu32 "\n"
		"\tframe pointer alignment: %" BOND_PRIu32 "\n"
		"\tcode size: %" BOND_PRIu32 "\n",
		functionHash, argSize, packedArgSize, localSize, stackSize, framePointerAlignment, codeSize);

	while (mIndex < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(mByteCode[mIndex]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		mWriter.Write("%6d: %-12s", mIndex - codeStart, GetOpCodeMnemonic(opCode));
		++mIndex;

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
				mWriter.Write("%" BOND_PRId32, bi32_t(char(mByteCode[mIndex++])));
				break;
			case OC_PARAM_UCHAR:
				mWriter.Write("%" BOND_PRIu32, bu32_t(mByteCode[mIndex++]));
				break;
			case OC_PARAM_UCHAR_CHAR:
				mWriter.Write("%" BOND_PRIu32 ", %" BOND_PRId32, bu32_t(mByteCode[mIndex]), bi32_t(char(mByteCode[mIndex + 1])));
				mIndex += 2;
				break;
			case OC_PARAM_SHORT:
				mWriter.Write("%" BOND_PRId32, bi32_t(ReadValue16().mShort));
				break;
			case OC_PARAM_USHORT:
				mWriter.Write("%" BOND_PRId32, bu32_t(ReadValue16().mUShort));
				break;
			case OC_PARAM_INT:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const bi32_t value = mValue32Table[valueIndex].mInt;
				mWriter.Write("%" BOND_PRId32, value);
			}
			break;
			case OC_PARAM_VAL32:
			case OC_PARAM_HASH:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const bu32_t value = mValue32Table[valueIndex].mUInt;
				mWriter.Write("0x%" BOND_PRIx32, value);
			}
			break;
			case OC_PARAM_VAL64:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const bu64_t value = mValue64Table[valueIndex].mULong;
				mWriter.Write("0x%" BOND_PRIx64, value);
			}
			break;
			case OC_PARAM_OFF16:
			{
				const bi32_t offset = ReadValue16().mShort;
				const bu32_t baseAddress = bu32_t(mIndex - codeStart);
				mWriter.Write("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const size_t offsetIndex = ReadValue16().mUShort;
				const bi32_t offset = mValue32Table[offsetIndex].mInt;
				const bu32_t baseAddress = bu32_t(mIndex - codeStart);
				mWriter.Write("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				const bi32_t defaultOffset = ReadValue32().mInt;
				const bu32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const bi32_t baseAddress = bi32_t(mIndex + tableSize - codeStart);
				mWriter.Write("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (bu32_t i = 0; i < numMatches; ++i)
				{
					const bi32_t match = ReadValue32().mInt;
					const bi32_t offset = ReadValue32().mInt;
					mWriter.Write("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				const bi32_t defaultOffset = ReadValue32().mInt;
				const bi32_t minMatch = ReadValue32().mInt;
				const bi32_t maxMatch = ReadValue32().mInt;
				const bu32_t numMatches = maxMatch - minMatch + 1;
				const size_t tableSize = numMatches * sizeof(Value32);
				const bi32_t baseAddress = bi32_t(mIndex + tableSize - codeStart);
				mWriter.Write("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (bu32_t i = 0; i < numMatches; ++i)
				{
					const bi32_t match = minMatch + i;
					const bi32_t offset = ReadValue32().mInt;
					mWriter.Write("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
		}
		mWriter.Write("\n");
	}
}


void DisassemblerCore::DisassembleQualifiedIdentifier()
{
	const size_t numElements = ReadValue16().mUShort;
	for (size_t i = 0; i < numElements; ++i)
	{
		const size_t idIndex = ReadValue16().mUShort;
		const SimpleString &id = mStringTable[idIndex];
		if (i > 0)
		{
			mWriter.Write("::");
		}
		WriteSimpleString(id);
	}
}


void DisassemblerCore::DisassembleParamListSignature()
{
	const size_t numElements = ReadValue16().mUShort;
	for (size_t i = 0; i < numElements; ++i)
	{
		// Skip the frame pointer offset.
		mIndex += sizeof(Value32);
		if (i > 0)
		{
			mWriter.Write(", ");
		}
		DisassembleSizeAndType();
	}
}


void DisassemblerCore::DisassembleSizeAndType()
{
	const bu32_t sizeAndType = ReadValue32().mUInt;
	bu32_t size;
	bu32_t type;
	DecodeSizeAndType(sizeAndType, size, type);
	const char *str = GetBondTypeMnemonic(SignatureType(type));
	mWriter.Write(str, size);
}


void DisassemblerCore::WriteSimpleString(const SimpleString &str)
{
	const size_t length = str.GetLength();
	const char *s = str.GetString();
	for (size_t i = 0; i < length; ++i)
	{
		mWriter.Write("%c", s[i]);
	}
}


Value16 DisassemblerCore::ReadValue16()
{
	const Value16 value(mByteCode + mIndex);
	mIndex += sizeof(Value16);
	return ConvertBigEndian16(value);
}


Value32 DisassemblerCore::ReadValue32()
{
	const Value32 value(mByteCode + mIndex);
	mIndex += sizeof(Value32);
	return ConvertBigEndian32(value);
}


Value64 DisassemblerCore::ReadValue64()
{
	const Value64 value(mByteCode + mIndex);
	mIndex += sizeof(Value64);
	return ConvertBigEndian64(value);
}

}
