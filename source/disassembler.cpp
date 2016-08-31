#include "bond/io/memoryinputstream.h"
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
			InputStream &cboStream,
			OutputStream &outputStream):
		mValue32Table(validationResult.mValue32Count, Value32(), Value32Table::allocator_type(&allocator)),
		mValue64Table(validationResult.mValue64Count, Value64(), Value64Table::allocator_type(&allocator)),
		mStringTable(validationResult.mStringCount, SimpleString(), StringTable::allocator_type(&allocator)),
		mStringBytes(validationResult.mStringByteCount + validationResult.mStringCount, char(), StringBytes::allocator_type(&allocator)),
		mQualifiedNameTable(validationResult.mQualifiedNameCount, QualifiedName(), QualifiedNameTable::allocator_type(&allocator)),
		mQualifiedNameElementTable(validationResult.mQualifiedNameElementCount + validationResult.mQualifiedNameCount, nullptr, QualifiedNameElementTable::allocator_type(&allocator)),
		mValidationResult(validationResult),
		mCboStream(cboStream),
		mOutStream(outputStream)
	{}

	DisassemblerCore(const DisassemblerCore &other) = delete;
	DisassemblerCore &operator=(const DisassemblerCore &other) = delete;

	void Disassemble();

private:
	typedef Vector<Value32> Value32Table;
	typedef Vector<Value64> Value64Table;
	typedef Vector<SimpleString> StringTable;
	typedef Vector<char> StringBytes;
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

	size_t GetPosition() const { return size_t(mCboStream.GetPosition()); }
	void SetPosition(size_t pos) { mCboStream.SetPosition(Stream::pos_t(pos)); }
	void Skip(size_t numBytes) { mCboStream.AddOffset(Stream::off_t(numBytes)); }

	Value32Table mValue32Table;
	Value64Table mValue64Table;
	StringTable mStringTable;
	StringBytes mStringBytes;
	QualifiedNameTable mQualifiedNameTable;
	QualifiedNameElementTable mQualifiedNameElementTable;
	CboValidator::Result mValidationResult;
	InputStream &mCboStream;
	OutputStream &mOutStream;
};


void Disassembler::Disassemble(const void *byteCode, size_t length, OutputStream &outputStream)
{
	MemoryInputStream cboStream(byteCode, Stream::pos_t(length));
	Disassemble(cboStream, outputStream);
}


void Disassembler::Disassemble(InputStream &cboStream, OutputStream &outputStream)
{
	const auto pos = cboStream.GetPosition();
	CboValidator validator;
	CboValidator::Result result = validator.Validate(cboStream);
	cboStream.SetPosition(pos);
	DisassemblerCore disassembler(result, mAllocator, cboStream, outputStream);
	disassembler.Disassemble();
}


void DisassemblerCore::Disassemble()
{
	// Skip some header information that is already included in the validation result.
	Skip((2 * sizeof(Value32)) + (7 * sizeof(Value16)));

	mOutStream.Print("Version %" BOND_PRIu32 ".%" BOND_PRIu32 "\n", mValidationResult.mMajorVersion, mValidationResult.mMinorVersion);
	mOutStream.Print("Pointer size: %d bits\n", (mValidationResult.mPointerSize == POINTER_64BIT) ? 64 : 32);

	for (size_t i = 0; i < mValidationResult.mValue32Count; ++i)
	{
		mValue32Table[i] = ReadValue32();
	}

	for (size_t i = 0; i < mValidationResult.mValue64Count; ++i)
	{
		mValue64Table[i] = ReadValue64();
	}

	char *stringBytes = mStringBytes.data();
	for (size_t i = 0; i < mValidationResult.mStringCount; ++i)
	{
		const size_t length = ReadValue16().mUShort;
		mStringTable[i] = SimpleString(stringBytes, length);
		mCboStream.Read(stringBytes, length);
		stringBytes += length;
		*stringBytes++ = '\0';
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
	const size_t blobStart = GetPosition();
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
		SetPosition(blobEnd);
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

	mOutStream.Print("Function: ");
	DisassembleSizeAndType();
	mOutStream.Print(" ");
	mQualifiedNameTable[functionNameIndex].PrintTo(mOutStream);
	mOutStream.Print("(");
	DisassembleParamListSignature();
	mOutStream.Print(")\n");

	const uint32_t argSize = ReadValue32().mUInt;
	const uint32_t packedArgSize = ReadValue32().mUInt;
	const uint32_t localSize = ReadValue32().mUInt;
	const uint32_t stackSize = ReadValue32().mUInt;
	const uint32_t framePointerAlignment = ReadValue32().mUInt;
	const uint32_t codeSize = ReadValue32().mUInt;
	const size_t codeStart = GetPosition();
	const size_t codeEnd = codeStart + codeSize;
	mOutStream.Print(
		"\targ size: %" BOND_PRIu32 "\n"
		"\tpacked arg size: %" BOND_PRIu32 "\n"
		"\tlocal size: %" BOND_PRIu32 "\n"
		"\tstack size: %" BOND_PRIu32 "\n"
		"\tframe pointer alignment: %" BOND_PRIu32 "\n"
		"\tcode size: %" BOND_PRIu32 "\n",
		argSize, packedArgSize, localSize, stackSize, framePointerAlignment, codeSize);

	while (GetPosition() < codeEnd)
	{
		const uint32_t opCodeAddress = uint32_t(GetPosition() - codeStart);
		const OpCode opCode = static_cast<OpCode>(mCboStream.Read());
		const OpCodeParam param = GetOpCodeParamType(opCode);

		mOutStream.Print("%6" BOND_PRIu32 ": %-12s", opCodeAddress, GetOpCodeMnemonic(opCode));

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
				mOutStream.Print("%" BOND_PRId32, int32_t(int8_t(mCboStream.Read())));
				break;
			case OC_PARAM_UCHAR:
				mOutStream.Print("%" BOND_PRIu32, uint32_t(mCboStream.Read()));
				break;
			case OC_PARAM_UCHAR_CHAR:
				mOutStream.Print("%" BOND_PRIu32 ", %" BOND_PRId32, uint32_t(mCboStream.Read()), int32_t(int8_t(mCboStream.Read())));
				break;
			case OC_PARAM_SHORT:
				mOutStream.Print("%" BOND_PRId32, int32_t(ReadValue16().mShort));
				break;
			case OC_PARAM_USHORT:
				mOutStream.Print("%" BOND_PRId32, uint32_t(ReadValue16().mUShort));
				break;
			case OC_PARAM_INT:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const int32_t value = mValue32Table[valueIndex].mInt;
				mOutStream.Print("%" BOND_PRId32, value);
			}
			break;
			case OC_PARAM_VAL32:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const uint32_t value = mValue32Table[valueIndex].mUInt;
				mOutStream.Print("0x%" BOND_PRIx32, value);
			}
			break;
			case OC_PARAM_VAL64:
			{
				const size_t valueIndex = ReadValue16().mUShort;
				const uint64_t value = mValue64Table[valueIndex].mULong;
				mOutStream.Print("0x%" BOND_PRIx64, value);
			}
			break;
			case OC_PARAM_OFF16:
			{
				const int32_t offset = ReadValue16().mShort;
				const uint32_t baseAddress = uint32_t(GetPosition() - codeStart);
				mOutStream.Print("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const size_t offsetIndex = ReadValue16().mUShort;
				const int32_t offset = mValue32Table[offsetIndex].mInt;
				const uint32_t baseAddress = uint32_t(GetPosition() - codeStart);
				mOutStream.Print("%" BOND_PRId32 " (%" BOND_PRIu32 ")", offset, baseAddress + offset);
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
				mQualifiedNameTable[nameIndex].PrintTo(mOutStream);
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				Skip(AlignUpDelta(GetPosition() - codeStart, sizeof(Value32)));
				const int32_t defaultOffset = ReadValue32().mInt;
				const uint32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const int32_t baseAddress = int32_t(GetPosition() + tableSize - codeStart);
				mOutStream.Print("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (uint32_t i = 0; i < numMatches; ++i)
				{
					const int32_t match = ReadValue32().mInt;
					const int32_t offset = ReadValue32().mInt;
					mOutStream.Print("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				Skip(AlignUpDelta(GetPosition() - codeStart, sizeof(Value32)));
				const int32_t defaultOffset = ReadValue32().mInt;
				const int32_t minMatch = ReadValue32().mInt;
				const int32_t maxMatch = ReadValue32().mInt;
				const uint32_t numMatches = maxMatch - minMatch + 1;
				const size_t tableSize = numMatches * sizeof(Value32);
				const int32_t baseAddress = int32_t(GetPosition() + tableSize - codeStart);
				mOutStream.Print("\n%16s: %" BOND_PRId32 " (%" BOND_PRIu32 ")", "default", defaultOffset, baseAddress + defaultOffset);

				for (uint32_t i = 0; i < numMatches; ++i)
				{
					const int32_t match = minMatch + i;
					const int32_t offset = ReadValue32().mInt;
					mOutStream.Print("\n%16" BOND_PRId32 ": %" BOND_PRId32 " (%" BOND_PRIu32 ")", match, offset, baseAddress + offset);
				}
			}
			break;
		}
		mOutStream.Print("\n");
	}

	// Disassemble the optional metadata blob.
	if (GetPosition() < blobEnd)
	{
		DisassembleBlob();
	}
}


void DisassemblerCore::DisassembleDataBlob(size_t blobEnd)
{
	const size_t nameIndex = ReadValue16().mUShort;

	mOutStream.Print("Data: ");
	const SignatureType type = DisassembleSizeAndType();
	mOutStream.Print(" ");
	mQualifiedNameTable[nameIndex].PrintTo(mOutStream);

	const Value32 payload = ReadValue32();

	switch (type)
	{
		case SIG_BOOL:
		{
			mOutStream.Print(" = %s\n", (payload.mInt == 0) ? "false" : "true");
		}
		break;

		case SIG_CHAR:
		case SIG_UCHAR:
		{
			mOutStream.Print(" = %c\n", payload.mInt);
		}
		break;

		case SIG_SHORT:
		case SIG_INT:
		{
			mOutStream.Print(" = %" BOND_PRId32 "\n", payload.mInt);
		}
		break;

		case SIG_USHORT:
		case SIG_UINT:
		{
			mOutStream.Print(" = %" BOND_PRIu32 "\n", payload.mUInt);
		}
		break;

		case SIG_LONG:
		{
			const int64_t value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mLong : 0;
			mOutStream.Print(" = %" BOND_PRId64 "\n", value);
		}
		break;

		case SIG_ULONG:
		{
			const uint64_t value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mULong : 0;
			mOutStream.Print(" = %" BOND_PRIu64 "\n", value);
		}
		break;

		case SIG_FLOAT:
		{
			mOutStream.Print(" = %f\n", payload.mFloat);
		}
		break;

		case SIG_DOUBLE:
		{
			const double value = IsInRange<uint16_t>(payload.mUInt) ? mValue64Table[payload.mUInt].mDouble : 0.0;
			mOutStream.Print(" = %f\n", value);
		}
		break;

		case SIG_VOID:
		case SIG_POINTER:
		case SIG_AGGREGATE:
		{
			mOutStream.Print("\n\talignment: %" BOND_PRIu32 "\n", payload.mUInt);
		}
		break;
	}

	// Disassemble the optional metadata blob.
	if (GetPosition() < blobEnd)
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
		Skip(sizeof(Value32));
		if (i > 0)
		{
			mOutStream.Print(", ");
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
	mOutStream.Print(str, size);
	return type;
}


void DisassemblerCore::WriteAbbreviatedString(const SimpleString &str)
{
	const size_t length = str.GetLength();
	const char *s = str.GetString();

	mOutStream.Print("%c", '"');
	if (length < 28)
	{
		for (size_t i = 0; i < length; ++i)
		{
			mOutStream.Print("%c", s[i]);
		}
	}
	else
	{
		for (size_t i = 0; i < 12; ++i)
		{
			mOutStream.Print("%c", s[i]);
		}
		mOutStream.Print("%s", "...");
		for (size_t i = length - 12; i < length; ++i)
		{
			mOutStream.Print("%c", s[i]);
		}
	}
	mOutStream.Print("%c", '"');
}


Value16 DisassemblerCore::ReadValue16()
{
	Value16 value;
	mCboStream.Read(value.mBytes, sizeof(Value16));
	return ConvertBigEndian16(value);
}


Value32 DisassemblerCore::ReadValue32()
{
	Value32 value;
	mCboStream.Read(value.mBytes, sizeof(Value32));
	return ConvertBigEndian32(value);
}


Value64 DisassemblerCore::ReadValue64()
{
	Value64 value;
	mCboStream.Read(value.mBytes, sizeof(Value64));
	return ConvertBigEndian64(value);
}

}
