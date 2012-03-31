#include "bond/cbohandler.h"
#include "bond/cboparser.h"
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

class DisassemblerHandler: public CboHandler
{
public:
	DisassemblerHandler(Allocator &allocator, TextWriter &writer):
		mAllocator(allocator),
		mWriter(writer),
		mStringTable(NULL),
		mStringTableSize(0),
		mValue32Table(NULL),
		mValue32TableSize(0),
		mValue64Table(NULL),
		mValue64TableSize(0)
	{}

	virtual ~DisassemblerHandler();

	virtual void ReceiveVersionInfo(int majorVersion, int minorVersion);

	virtual void ReceivePointerSize(PointerSize size);

	virtual void ReceiveValue32Table(size_t numValues, const Value32 *values);
	virtual const Value32 &GetValue32(size_t index) const { return mValue32Table[index]; }
	virtual const size_t &GetValue32TableSize() const { return mValue32TableSize; }

	virtual void ReceiveValue64Table(size_t numValues, const Value64 *values);
	virtual const Value64 &GetValue64(size_t index) const { return mValue64Table[index]; }
	virtual const size_t &GetValue64TableSize() const { return mValue64TableSize; }

	virtual void ReceiveStringTableSize(size_t numStrings);
	virtual void ReceiveString(const HashedString &str, size_t index) { mStringTable[index] = str; }
	virtual const HashedString &GetString(size_t index) const { return mStringTable[index]; }
	virtual const size_t &GetStringTableSize() const { return mStringTableSize; }

	virtual void BeginList(size_t numElements) {}
	virtual void EndList() {}

	virtual void ReceiveFunctionBlob(const QualifiedId &name, bu32_t hash, size_t codeSize, unsigned char *byteCode);
	virtual void EndFunctionBlob() {}

	virtual void ReceiveBlob(unsigned char *blob, size_t blobSize) {}

	virtual void CboIsCorrupt();
	virtual void InvalidMagicNumber(bu32_t number);
	virtual void InvalidByteCode();

private:
	void WriteQualifiedIdentifier(const QualifiedId &id);
	void WriteHashedString(const HashedString &str);
	void FreeStringTable();

	Allocator &mAllocator;
	TextWriter &mWriter;
	HashedString *mStringTable;
	size_t mStringTableSize;
	const Value32 *mValue32Table;
	size_t mValue32TableSize;
	const Value64 *mValue64Table;
	size_t mValue64TableSize;
};


DisassemblerHandler::~DisassemblerHandler()
{
	FreeStringTable();
}


void Disassembler::Disassemble(TextWriter &writer, unsigned char *byteCode, size_t length)
{
	DisassemblerHandler handler(mAllocator, writer);
	CboParser parser;
	parser.Parse(handler, byteCode, length);
}


void DisassemblerHandler::ReceiveVersionInfo(int majorVersion, int minorVersion)
{
	mWriter.Write("Version %d.%02d\n", majorVersion, minorVersion);
}


void DisassemblerHandler::ReceivePointerSize(PointerSize size)
{
	mWriter.Write("Pointer size: %d bits\n", (size == POINTER_64BIT) ? 64 : 32);
}


void DisassemblerHandler::ReceiveValue32Table(size_t numValues, const Value32 *values)
{
	mValue32TableSize = numValues;
	mValue32Table = values;
}


void DisassemblerHandler::ReceiveValue64Table(size_t numValues, const Value64 *values)
{
	mValue64TableSize = numValues;
	mValue64Table = values;
}


void DisassemblerHandler::ReceiveStringTableSize(size_t numStrings)
{
	FreeStringTable();
	if (numStrings > 0)
	{
		mStringTable = mAllocator.Alloc<HashedString>(numStrings);
		mStringTableSize = numStrings;
	}
}


void DisassemblerHandler::ReceiveFunctionBlob(const QualifiedId &name, bu32_t hash, size_t codeSize, unsigned char *byteCode)
{
	mWriter.Write("Function: ");
	WriteQualifiedIdentifier(name);

	mWriter.Write("\n  hash: 0x" BOND_UHEX_FORMAT "\n  code size: %u\n", hash, codeSize);

	size_t index = 0;
	while (index < codeSize)
	{
		const OpCode opCode = static_cast<OpCode>(byteCode[index]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		mWriter.Write("%6d: %-12s", index, GetOpCodeMnemonic(opCode));
		++index;

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
				mWriter.Write(BOND_DECIMAL_FORMAT, static_cast<bi32_t>(static_cast<char>(byteCode[index++])));
				break;
			case OC_PARAM_UCHAR:
				mWriter.Write(BOND_UDECIMAL_FORMAT, static_cast<bu32_t>(byteCode[index++]));
				break;
			case OC_PARAM_SHORT:
				mWriter.Write(BOND_DECIMAL_FORMAT, Value16(byteCode + index).mShort);
				index += sizeof(Value16);
				break;
			case OC_PARAM_USHORT:
				mWriter.Write(BOND_UDECIMAL_FORMAT, Value16(byteCode + index).mUShort);
				index += sizeof(Value16);
				break;
			case OC_PARAM_INT:
			{
				const size_t valueIndex = Value16(byteCode + index).mUShort;
				index += sizeof(Value16);
				const bi32_t value = mValue32Table[valueIndex].mInt;
				mWriter.Write(BOND_DECIMAL_FORMAT, value);
			}
			break;
			case OC_PARAM_VAL32:
			{
				const size_t valueIndex = Value16(byteCode + index).mUShort;
				index += sizeof(Value16);
				const bu32_t value = mValue32Table[valueIndex].mUInt;
				mWriter.Write(BOND_UHEX_FORMAT, value);
			}
			break;
			case OC_PARAM_VAL64:
				// TODO.
				break;
			case OC_PARAM_OFF16:
			{
				const bi32_t offset = Value16(byteCode + index).mShort;
				index += sizeof(Value16);
				mWriter.Write(BOND_DECIMAL_FORMAT " (" BOND_DECIMAL_FORMAT ")", offset, index + offset);
			}
			break;
			case OC_PARAM_OFF32:
			{
				const size_t offsetIndex = Value16(byteCode + index).mUShort;
				const bi32_t offset = mValue32Table[offsetIndex].mInt;
				mWriter.Write(BOND_DECIMAL_FORMAT " (" BOND_DECIMAL_FORMAT ")", offset, index + offset);
			}
			break;
			case OC_PARAM_HASH:
				// TODO
				break;
		}
		mWriter.Write("\n");
	}
}


void DisassemblerHandler::CboIsCorrupt()
{
	mWriter.Write("CBO file is incomplete or malformed\n");
}


void DisassemblerHandler::InvalidMagicNumber(bu32_t number)
{
	mWriter.Write("Invalid magic number: " BOND_UHEX_FORMAT "\n", number);
}


void DisassemblerHandler::InvalidByteCode()
{
	mWriter.Write("Byte-code failed validation\n");
}


void DisassemblerHandler::WriteQualifiedIdentifier(const QualifiedId &id)
{
	const int numElements = id.GetNumElements();
	for (int i = 0; i < numElements; ++i)
	{
		const int elementIndex = id.GetElementIndex(i);
		const HashedString &id = mStringTable[elementIndex];
		if (i > 0)
		{
			mWriter.Write("::");
		}
		WriteHashedString(id);
	}
}


void DisassemblerHandler::WriteHashedString(const HashedString &str)
{
	const int length = str.GetLength();
	const char *s = str.GetString();
	for (int i = 0; i < length; ++i)
	{
		mWriter.Write("%c", s[i]);
	}
}


void DisassemblerHandler::FreeStringTable()
{
	mAllocator.Free(mStringTable);
	mStringTable = NULL;
	mStringTableSize = 0;
}

}
