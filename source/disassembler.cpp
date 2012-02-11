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
	DisassemblerCore(Allocator &allocator, TextWriter &writer, const unsigned char *byteCode, size_t length):
		mStringList(StringList::Allocator(&allocator)),
		mAllocator(allocator),
		mWriter(writer),
		mByteCode(byteCode),
		mLength(length),
		mIndex(0)
	{}

	void Disassemble();

private:
	typedef Vector<HashedString> StringList;

	Value16 ReadValue16();
	Value32 ReadValue32();
	void ReadStringTable();
	void ReadBlob();
	void ReadListBlob(size_t expectedEnd);
	void ReadFunctionBlob(size_t expectedEnd);
	void ReportMalformedCBO();

	StringList::Type mStringList;
	Allocator &mAllocator;
	TextWriter &mWriter;
	const unsigned char *mByteCode;
	size_t mLength;
	size_t mIndex;
};


void Disassembler::Disassemble(TextWriter &writer, const unsigned char *byteCode, size_t length)
{
	DisassemblerCore disassembler(mAllocator, writer, byteCode, length);
	disassembler.Disassemble();
}


void DisassemblerCore::Disassemble()
{
	if (mLength < 8)
	{
		ReportMalformedCBO();
		return;
	}

	const bu32_t magicNumber = ReadValue32().mUInt;
	if (magicNumber != MAGIC_NUMBER)
	{
		mWriter.Write("Magic number 0x%X does not match expected magic number 0x%X\n", magicNumber, MAGIC_NUMBER);
	}

	const bu32_t majorVersion = ReadValue16().mUShort;
	const bu32_t minorVersion = ReadValue16().mUShort;
	mWriter.Write("Version %d.%02d\n", majorVersion, minorVersion);

	ReadStringTable();
	ReadBlob();
}


void DisassemblerCore::ReadStringTable()
{
	if ((mIndex < mLength) && ((mIndex + 6) > mLength))
	{
		ReportMalformedCBO();
		return;
	}

	const size_t size = ReadValue32().mUInt;
	const size_t expectedEnd = mIndex + size - 4;
	if (expectedEnd > mLength)
	{
		ReportMalformedCBO();
		return;
	}

	const int numStrings = ReadValue16().mUShort;
	mStringList.reserve(numStrings);

	for (int i = 0; i < numStrings; ++i)
	{
		if ((mIndex + 2) > mLength)
		{
			ReportMalformedCBO();
			return;
		}

		const int length = ReadValue16().mUShort;
		if ((mIndex + length) > mLength)
		{
			ReportMalformedCBO();
			return;
		}

		mStringList.push_back(HashedString(reinterpret_cast<const char *>(mByteCode + mIndex), length));
		mIndex += length;
	}

	if (mIndex != expectedEnd)
	{
		ReportMalformedCBO();
	}
}


void DisassemblerCore::ReadBlob()
{
	if ((mIndex < mLength) && ((mIndex + 6) > mLength))
	{
		ReportMalformedCBO();
		return;
	}

	const size_t size = ReadValue32().mUInt;
	const size_t idIndex = ReadValue16().mUShort;
	const size_t expectedEnd = mIndex + size - 6;
	if (expectedEnd > mLength)
	{
		ReportMalformedCBO();
		return;
	}

	const HashedString id = mStringList[idIndex];
	if (id == HashedString("List"))
	{
		ReadListBlob(expectedEnd);
	}
	else if (id == HashedString("Func"))
	{
		ReadFunctionBlob(expectedEnd);
	}
}


void DisassemblerCore::ReadListBlob(size_t expectedEnd)
{
	if ((mIndex < mLength) && ((mIndex + 4) > mLength))
	{
		ReportMalformedCBO();
		return;
	}

	const size_t numBlobs = ReadValue32().mUInt;
	mWriter.Write("List num elements: %u\n", numBlobs);
	for (size_t i = 0; (i < numBlobs) && (mIndex < mLength); ++i)
	{
		ReadBlob();
	}
}


void DisassemblerCore::ReadFunctionBlob(size_t expectedEnd)
{
	if ((mIndex < mLength) && ((mIndex + 8) > mLength))
	{
		ReportMalformedCBO();
		return;
	}

	const bu32_t hash = ReadValue32().mUInt;
	const size_t codeSize = ReadValue32().mUInt;
	mWriter.Write("Function hash: 0x" BOND_UHEX_FORMAT " code size: %u\n", hash, codeSize);
	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;

	while (mIndex < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(mByteCode[mIndex++]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		mWriter.Write("%6d: %-12s", mIndex - 1 - codeStart, GetOpCodeMnemonic(opCode));

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
				mWriter.Write(BOND_DECIMAL_FORMAT, static_cast<bi32_t>(mByteCode[mIndex++]));
				break;
			case OC_PARAM_UCHAR:
				mWriter.Write(BOND_UDECIMAL_FORMAT, static_cast<bu32_t>(mByteCode[mIndex++]));
				break;
			case OC_PARAM_SHORT:
				mWriter.Write(BOND_DECIMAL_FORMAT, static_cast<bi32_t>(ReadValue16().mShort));
				break;
			case OC_PARAM_USHORT:
				mWriter.Write(BOND_UDECIMAL_FORMAT, static_cast<bi32_t>(ReadValue16().mUShort));
				break;
			case OC_PARAM_INT:
				mWriter.Write(BOND_DECIMAL_FORMAT, ReadValue32().mInt);
				break;
			case OC_PARAM_VAL32:
				mWriter.Write(BOND_UHEX_FORMAT, ReadValue32().mUInt);
				break;
			case OC_PARAM_VAL64:
				// TODO.
				break;
			case OC_PARAM_OFF16:
			{
				const int offset = static_cast<int>(ReadValue16().mShort);
				const int baseAddress = static_cast<int>(mIndex - codeStart);
				mWriter.Write("%d (%d)", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const int offset = static_cast<int>(ReadValue32().mInt);
				const int baseAddress = static_cast<int>(mIndex - codeStart);
				mWriter.Write("%d (%d)", offset, baseAddress + offset);
			}
			break;
			case OC_PARAM_HASH:
				// TODO
				break;
		}
		mWriter.Write("\n");
	}
}


Value16 DisassemblerCore::ReadValue16()
{
	Value16 value(mByteCode + mIndex);
	mIndex += 2;
	ConvertBigEndian16(value.mBytes);
	return value;
}


Value32 DisassemblerCore::ReadValue32()
{
	Value32 value(mByteCode + mIndex);
	mIndex += 4;
	ConvertBigEndian32(value.mBytes);
	return value;
}


void DisassemblerCore::ReportMalformedCBO()
{
	mWriter.Write("CBO file is incomplete or malformed\n");
	mIndex = mLength;
}

}
