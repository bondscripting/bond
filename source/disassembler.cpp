#include "bond/io/outputstream.h"
#include "bond/stl/vector.h"
#include "bond/systems/endian.h"
#include "bond/systems/math.h"
#include "bond/tools/cboutil.h"
#include "bond/tools/cbovalidator.h"
#include "bond/tools/disassembler.h"
#include "bond/types/opcodes.h"
#include "bond/types/qualifiedname.h"
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
			OutputStream &stream,
			const uint8_t *byteCode):
		mValue32Table(validationResult.mValue32Count, Value32(), Value32Table::Allocator(&allocator)),
		mValue64Table(validationResult.mValue64Count, Value64(), Value64Table::Allocator(&allocator)),
		mStringTable(validationResult.mStringCount, SimpleString(), StringTable::Allocator(&allocator)),
		mQualifiedNameTable(validationResult.mQualifiedNameCount, QualifiedName(), QualifiedNameTable::Allocator(&allocator)),
		mQualifiedNameElementTable(validationResult.mQualifiedNameElementCount + validationResult.mQualifiedNameCount, nullptr, QualifiedNameElementTable::Allocator(&allocator)),
		mValidationResult(validationResult),
		mStream(stream),
		mByteCode(byteCode),
		mIndex(0)
	{}

	void Disassemble();

	DisassemblerCore &operator=(const DisassemblerCore &other) = delete;

private:
	typedef Vector<Value32> Value32Table;
	typedef Vector<Value64> Value64Table;
	typedef Vector<SimpleString> StringTable;
	typedef Vector<QualifiedName> QualifiedNameTable;
	typedef Vector<const char *> QualifiedNameElementTable;

	void DisassembleBlob();
	void DisassembleListBlob();
	void DisassembleFunctionBlob(size_t blobEnd);
	void DisassembleDataBlob(size_t blobEnd);
	void DisassembleParamListSignature();
	SignatureType DisassembleSizeAndType();
	void WriteAbbreviatedString(const SimpleString &str);

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	Value32Table::Type mValue32Table;
	Value64Table::Type mValue64Table;
	StringTable::Type mStringTable;
	QualifiedNameTable::Type mQualifiedNameTable;
	QualifiedNameElementTable::Type mQualifiedNameElementTable;
	CboValidator::Result mValidationResult;
	OutputStream &mStream;
	const uint8_t *mByteCode;
	size_t mIndex;
};


void Disassembler::Disassemble(OutputStream &stream, const void *byteCode, size_t length)
{
	CboValidator validator;
	CboValidator::Result result = validator.Validate(byteCode, length);
	DisassemblerCore disassembler(result, mAllocator, stream, static_cast<const uint8_t *>(byteCode));
	disassembler.Disassemble();
}


void DisassemblerCore::Disassemble()
{
	// Skip some header information that is already included in the validation result.
	mIndex += (2 * sizeof(Value32)) + (7 * sizeof(Value16));

	mStream.Print("Version %" BOND_PRIu32 ".%" BOND_PRIu32 "\n", mValidationResult.mMajorVersion, mValidationResult.mMinorVersion);
	mStream.Print("Pointer size: %d bits\n", (mValidationResult.mPointerSize == POINTER_64BIT) ? 64 : 32);

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

	QualifiedName *name = mQualifiedNameTable.data();
	const char **element = mQualifiedNameElementTable.data();
	for (size_t i = 0; i < mValidationResult.mQualifiedNameCount; ++i)
	{
		*name++ = QualifiedName(element);
		const size_t numElements = ReadValue16().mUShort;
		for (size_t j = 0; j < numElements; ++j)
		{
			const size_t elementIndex = ReadValue16().mUShort;
			*element++ = mStringTable[elementIndex].GetString();
		}
		*element++ = nullptr;
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
		DisassembleFunctionBlob(blobEnd);
	}
	else if (idIndex == mValidationResult.mDataBlobIdIndex)
	{
		DisassembleDataBlob(blobEnd);
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


void DisassemblerCore::DisassembleFunctionBlob(size_t blobEnd)
{
	const size_t functionNameIndex = ReadValue16().mUShort;

	mStream.Print("Function: ");
	DisassembleSizeAndType();
	mStream.Print(" ");
	mQualifiedNameTable[functionNameIndex].PrintTo(mStream);
	mStream.Print("(");
	DisassembleParamListSignature();
	mStream.Print(")\n");

	const uint32_t argSize = ReadValue32().mUInt;
	const uint32_t packedArgSize = ReadValue32().mUInt;
	const uint32_t localSize = ReadValue32().mUInt;
	const uint32_t stackSize = ReadValue32().mUInt;
	const uint32_t framePointerAlignment = ReadValue32().mUInt;
	const uint32_t codeSize = ReadValue32().mUInt;
	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;
	mStream.Print(
		"\targ size: %" BOND_PRIu32 "\n"
		"\tpacked arg size: %" BOND_PRIu32 "\n"
		"\tlocal size: %" BOND_PRIu32 "\n"
		"\tstack size: %" BOND_PRIu32 "\n"
		"\tframe pointer alignment: %" BOND_PRIu32 "\n"
		"\tcode size: %" BOND_PRIu32 "\n",
		argSize, packedArgSize, localSize, stackSize, framePointerAlignment, codeSize);

	while (mIndex < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(mByteCode[mIndex]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		mStream.Print("%6d: %-12s", mIndex - codeStart, GetOpCodeMnemonic(opCode));
		++mIndex;

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
				mStream.Print("%" BOND_PRId32, int32_t(int8_t(mByteCode[mIndex++])));
				break;
			case OC_PARAM_UCHAR:
				mStream.Print("%" BOND_PRIu32, uint32_t(mByteCode[mIndex++]));
				break;
			case OC_PARAM_UCHAR_CHAR:
				mStream.Print("%" BOND_PRIu32 ", %" BOND_PRId32, uint32_t(mByteCode[mIndex]), int32_t(int8_t(mByteCode[mIndex + 1])));
				mIndex += 2;
				break;
			case OC_PARAM_SHORT:
				mStream.Print("%" BOND_PRId32, int32_t(ReadValue16().mShort));
				break;
			case OC_PARAM_USHORT:
				mStream.Print("%" BOND_PRId32, uint32_t(ReadValue16().mUShort));
				break;
			case OC_PARAM_INT:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const int32_t value = mValue32Table[valueIndex].mInt;
				mStream.Print("%" BOND_PRId32, value);
			}
			break;
			case OC_PARAM_VAL32:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const uint32_t value = mValue32Table[valueIndex].mUInt;
				mStream.Print("0x%" BOND_PRIx32, value);
			}
			break;
			case OC_PARAM_VAL64:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const uint64_t value = mValue64Table[valueIndex].mULong;
				mStream.Print("0x%" BOND_PRIx64, value);
			}
			break;
			case OC_PARAM_OFF16:
			{
				const int32_t offset = ReadValue16().mShort;
				const uint32_t baseAddress = uint32_t(mIndex - codeStart);
				mStream.Print("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const size_t offsetIndex = ReadValue16().mUShort;
				const int32_t offset = mValue32Table[offsetIndex].mInt;
				const uint32_t baseAddress = uint32_t(mIndex - codeStart);
				mStream.Print("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_STRING:
			{
				const size_t stringIndex = ReadValue16().mUShort;
				WriteAbbreviatedString(mStringTable[stringIndex]);
			}
			break;
			case OC_PARAM_NAME:
			{
				const size_t nameIndex = ReadValue16().mUShort;
				mQualifiedNameTable[nameIndex].PrintTo(mStream);
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				const int32_t defaultOffset = ReadValue32().mInt;
				const uint32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const int32_t baseAddress = int32_t(mIndex + tableSize - codeStart);
				mStream.Print("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (uint32_t i = 0; i < numMatches; ++i)
				{
					const int32_t match = ReadValue32().mInt;
					const int32_t offset = ReadValue32().mInt;
					mStream.Print("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				const int32_t defaultOffset = ReadValue32().mInt;
				const int32_t minMatch = ReadValue32().mInt;
				const int32_t maxMatch = ReadValue32().mInt;
				const uint32_t numMatches = maxMatch - minMatch + 1;
				const size_t tableSize = numMatches * sizeof(Value32);
				const int32_t baseAddress = int32_t(mIndex + tableSize - codeStart);
				mStream.Print("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (uint32_t i = 0; i < numMatches; ++i)
				{
					const int32_t match = minMatch + i;
					const int32_t offset = ReadValue32().mInt;
					mStream.Print("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
		}
		mStream.Print("\n");
	}

	// Disassemble the optional metadata blob.
	if (mIndex < blobEnd)
	{
		DisassembleBlob();
	}
}


void DisassemblerCore::DisassembleDataBlob(size_t blobEnd)
{
	const size_t nameIndex = ReadValue16().mUShort;

	mStream.Print("Data: ");
	const SignatureType type = DisassembleSizeAndType();
	mStream.Print(" ");
	mQualifiedNameTable[nameIndex].PrintTo(mStream);

	const Value32 payload = ReadValue32();

	switch (type)
	{
		case SIG_BOOL:
		{
			mStream.Print(" = %s\n", (payload.mInt == 0) ? "false" : "true");
		}
		break;

		case SIG_CHAR:
		case SIG_UCHAR:
		{
			mStream.Print(" = %c\n", payload.mInt);
		}
		break;

		case SIG_SHORT:
		case SIG_INT:
		{
			mStream.Print(" = %" BOND_PRId32 "\n", payload.mInt);
		}
		break;

		case SIG_USHORT:
		case SIG_UINT:
		{
			mStream.Print(" = %" BOND_PRIu32 "\n", payload.mUInt);
		}
		break;

		case SIG_LONG:
		{
			const int64_t value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mLong : 0;
			mStream.Print(" = %" BOND_PRId64 "\n", value);
		}
		break;

		case SIG_ULONG:
		{
			const uint64_t value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mULong : 0;
			mStream.Print(" = %" BOND_PRIu64 "\n", value);
		}
		break;

		case SIG_FLOAT:
		{
			mStream.Print(" = %f\n", payload.mFloat);
		}
		break;

		case SIG_DOUBLE:
		{
			const double value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mDouble : 0.0;
			mStream.Print(" = %f\n", value);
		}
		break;

		case SIG_VOID:
		case SIG_POINTER:
		case SIG_AGGREGATE:
		{
			mStream.Print("\n\talignment: %" BOND_PRIu32 "\n", payload.mUInt);
		}
		break;
	}

	// Disassemble the optional metadata blob.
	if (mIndex < blobEnd)
	{
		DisassembleBlob();
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
			mStream.Print(", ");
		}
		DisassembleSizeAndType();
	}
}


SignatureType DisassemblerCore::DisassembleSizeAndType()
{
	const uint32_t sizeAndType = ReadValue32().mUInt;
	uint32_t size;
	SignatureType type;
	DecodeSizeAndType(sizeAndType, size, type);
	const char *str = GetBondTypeMnemonic(type);
	mStream.Print(str, size);
	return type;
}


void DisassemblerCore::WriteAbbreviatedString(const SimpleString &str)
{
	const size_t length = str.GetLength();
	const char *s = str.GetString();

	mStream.Print("%c", '"');
	if (length < 28)
	{
		for (size_t i = 0; i < length; ++i)
		{
			mStream.Print("%c", s[i]);
		}
	}
	else
	{
		for (size_t i = 0; i < 12; ++i)
		{
			mStream.Print("%c", s[i]);
		}
		mStream.Print("%s", "...");
		for (size_t i = length - 12; i < length; ++i)
		{
			mStream.Print("%c", s[i]);
		}
	}
	mStream.Print("%c", '"');
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
