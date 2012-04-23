#include "bond/cbovalidator.h"
#include "bond/disassembler.h"
#include "bond/endian.h"
#include "bond/hashedstring.h"
#include "bond/opcodes.h"
#include "bond/textwriter.h"
#include "bond/value.h"
#include "bond/vector.h"
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
			const unsigned char *byteCode):
		mStringTable(validationResult.mStringCount, HashedString(), StringTable::Allocator(&allocator)),
		mValue32Table(validationResult.mValue32Count, Value32(), Value32Table::Allocator(&allocator)),
		mValue64Table(validationResult.mValue64Count, Value64(), Value64Table::Allocator(&allocator)),
		mValidationResult(validationResult),
		mAllocator(allocator),
		mWriter(writer),
		mByteCode(byteCode),
		mIndex(0)
	{}

	void Disassemble();

private:
	typedef Vector<HashedString> StringTable;
	typedef Vector<Value32> Value32Table;
	typedef Vector<Value64> Value64Table;

	void DisassembleBlob();
	void DisassembleListBlob();
	void DisassembleFunctionBlob();
	void DisassembleQualifiedIdentifier();
	void WriteHashedString(const HashedString &str);

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	StringTable::Type mStringTable;
	Value32Table::Type mValue32Table;
	Value64Table::Type mValue64Table;
	CboValidator::Result mValidationResult;
	Allocator &mAllocator;
	TextWriter &mWriter;
	const unsigned char *mByteCode;
	size_t mIndex;
};


void Disassembler::Disassemble(TextWriter &writer, const unsigned char *byteCode, size_t length)
{
	CboValidator validator;
	CboValidator::Result result = validator.Validate(byteCode, length);

	switch (result.mValidity)
	{
		case CboValidator::CBO_VALID:
		{
			DisassemblerCore disassembler(result, mAllocator, writer, byteCode);
			disassembler.Disassemble();
		}
		break;

		case CboValidator::CBO_INVALID_MAGIC_NUMBER:
			writer.Write("CBO file's magic number is incorrect\n");
			break;

		case CboValidator::CBO_INVALID_VERSION:
			writer.Write("CBO file's version is unknown\n");
			break;

		case CboValidator::CBO_INVALID_BYTECODE:
			writer.Write("CBO file contains invalid bytecode\n");
			break;

		case CboValidator::CBO_INVALID_FORMAT:
			writer.Write("CBO file is incomplete or malformed\n");
			break;
	}
}


void DisassemblerCore::Disassemble()
{
	// Skip some header information.
	mIndex += 16;

	mWriter.Write("Version %d.%02d\n", mValidationResult.mMajorVersion, mValidationResult.mMinorVersion);
	mWriter.Write("Pointer size: %d bits\n", (mValidationResult.mPointerSize == POINTER_64BIT) ? 64 : 32);

	for (size_t i = 0; i < mValidationResult.mValue64Count; ++i)
	{
		mValue64Table[i] = ReadValue64();
	}

	for (size_t i = 0; i < mValidationResult.mValue32Count; ++i)
	{
		mValue32Table[i] = ReadValue32();
	}

	mIndex += 2;
	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const int length = ReadValue16().mUShort;
		mStringTable[i] = HashedString(reinterpret_cast<const char *>(mByteCode + mIndex), length);
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
	const bu32_t hash = ReadValue32().mUInt;

	mWriter.Write("Function: ");
	DisassembleQualifiedIdentifier();

	const bu32_t codeSize = ReadValue32().mUInt;
	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;
	mWriter.Write("\n  hash: 0x%" BOND_PRIx32 "\n  code size: %" BOND_PRIu32 "\n", hash, codeSize);

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
				mWriter.Write("%" BOND_PRId32, static_cast<bi32_t>(static_cast<char>(mByteCode[mIndex++])));
				break;
			case OC_PARAM_UCHAR:
				mWriter.Write("%" BOND_PRIu32, static_cast<bu32_t>(mByteCode[mIndex++]));
				break;
			case OC_PARAM_SHORT:
				mWriter.Write("%" BOND_PRId32, static_cast<bi32_t>(ReadValue16().mShort));
				break;
			case OC_PARAM_USHORT:
				mWriter.Write("%" BOND_PRId32, static_cast<bu32_t>(ReadValue16().mUShort));
				break;
			case OC_PARAM_INT:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const bi32_t value = mValue32Table[valueIndex].mInt;
				mWriter.Write("%" BOND_PRId32, value);
			}
			break;
			case OC_PARAM_VAL32:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const bu32_t value = mValue32Table[valueIndex].mUInt;
				mWriter.Write("0x%" BOND_PRIx32, value);
			}
			break;
			case OC_PARAM_VAL64:
				// TODO.
				break;
			case OC_PARAM_OFF16:
			{
				const bi32_t offset = ReadValue16().mShort;
				const bu32_t baseAddress = static_cast<bu32_t>(mIndex - codeStart);
				mWriter.Write("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const size_t offsetIndex = ReadValue16().mUShort;
				const bi32_t offset = mValue32Table[offsetIndex].mInt;
				const bu32_t baseAddress = static_cast<bu32_t>(mIndex - codeStart);
				mWriter.Write("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_HASH:
				// TODO
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
		const HashedString &id = mStringTable[idIndex];
		if (i > 0)
		{
			mWriter.Write("::");
		}
		WriteHashedString(id);
	}
}


void DisassemblerCore::WriteHashedString(const HashedString &str)
{
	const int length = str.GetLength();
	const char *s = str.GetString();
	for (int i = 0; i < length; ++i)
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
