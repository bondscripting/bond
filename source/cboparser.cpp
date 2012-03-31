#include "bond/cbohandler.h"
#include "bond/cboparser.h"
#include "bond/hashedstring.h"
#include "bond/endian.h"
#include "bond/opcodes.h"
#include "bond/version.h"

namespace Bond
{

CboHandler::~CboHandler() {}

class CboParserCore
{
public:
	CboParserCore(CboHandler &handler, unsigned char *byteCode, size_t length):
		mHandler(handler),
		mByteCode(byteCode),
		mLength(length),
		mIndex(0),
		mHasError(false),
		mReverseByteOrder(false)
	{}

	void Parse();

private:
	void ParseMagicNumber();
	void ParseBlob();
	void ParseListBlob();
	void ParseFunctionBlob();
	QualifiedId ParseQualifiedIdentifier();

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value64 ReadValue64();

	bool AssertBytesRemaining(size_t numBytes);
	void CboIsCorrupt();

	CboHandler &mHandler;
	unsigned char *mByteCode;
	size_t mLength;
	size_t mIndex;
	bool mHasError;
	bool mReverseByteOrder;
};


void CboParser::Parse(CboHandler &handler, unsigned char *byteCode, size_t length)
{
	CboParserCore parser(handler, byteCode, length);
	parser.Parse();
}


void CboParserCore::Parse()
{
	if (!AssertBytesRemaining((2 * sizeof(Value32)) + (4 * sizeof(Value16))))
	{
		return;
	}

	ParseMagicNumber();
	const int majorVersionAndFlags = ReadValue16().mUShort;
	const int minorVersion = ReadValue16().mUShort;
	const size_t tableStart = mIndex;
	const size_t tableSize = ReadValue32().mUInt;
	const size_t numValue64s = ReadValue16().mUShort;
	const size_t numValue32s = ReadValue16().mUShort;

	const size_t valueSize = (numValue64s * sizeof(Value64)) + (numValue32s * sizeof(Value32));
	if (!AssertBytesRemaining(tableSize - (mIndex - tableStart)) || !AssertBytesRemaining(valueSize))
	{
		return;
	}

	const int majorVersion = majorVersionAndFlags & 0x7fff;
	const PointerSize pointerSize = ((majorVersionAndFlags & 0x8000) != 0) ? POINTER_64BIT : POINTER_32BIT;
	mHandler.ReceiveVersionInfo(majorVersion, minorVersion);
	mHandler.ReceivePointerSize(pointerSize);

	unsigned char *value64Table = mByteCode + mIndex;
	mIndex += numValue64s * sizeof(Value64);
	unsigned char *value32Table = mByteCode + mIndex;
	mIndex += numValue32s * sizeof(Value32);

	if (mReverseByteOrder)
	{
		BatchReverseByteOrder64(value64Table, numValue64s);
		BatchReverseByteOrder32(value32Table, numValue32s);
	}

	mHandler.ReceiveValue64Table(numValue64s, reinterpret_cast<const Value64 *>(mByteCode + mIndex));
	mHandler.ReceiveValue32Table(numValue32s, reinterpret_cast<const Value32 *>(mByteCode + mIndex));

	if (!AssertBytesRemaining(sizeof(Value16)))
	{
		return;
	}

	const size_t numStrings = ReadValue16().mUShort;
	mHandler.ReceiveStringTableSize(numStrings);

	for (size_t i = 0; i < numStrings; ++i)
	{
		if (!AssertBytesRemaining(sizeof(Value16)))
		{
			return;
		}

		const int stringLength = ReadValue16().mUShort;
		if (!AssertBytesRemaining(stringLength))
		{
			return;
		}

		mHandler.ReceiveString(HashedString(reinterpret_cast<const char *>(mByteCode + mIndex), stringLength), i);
		mIndex += stringLength;
	}

	const size_t tableEnd = tableStart + tableSize;
	if (mIndex != tableEnd)
	{
		CboIsCorrupt();
	}

	ParseBlob();

	if (mIndex != mLength)
	{
		CboIsCorrupt();
	}
}


void CboParserCore::ParseMagicNumber()
{
	const bu32_t magicNumber = ReadValue32().mUInt;
	if (magicNumber == BACKWARDS_MAGIC_NUMBER)
	{
		mReverseByteOrder = true;
		mIndex -= sizeof(Value32);
		ReadValue32();
	}
	else if (magicNumber != MAGIC_NUMBER)
	{
		mHandler.InvalidMagicNumber(magicNumber);
		mHasError = true;
	}
}


void CboParserCore::ParseBlob()
{
	if (!AssertBytesRemaining(sizeof(Value32) + sizeof(Value16)))
	{
		return;
	}

	const size_t blobStart = mIndex;
	const bu32_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const bu32_t idIndex = ReadValue16().mUShort;

	if (!AssertBytesRemaining(blobSize - (mIndex - blobStart)))
	{
		return;
	}

	const HashedString &id = mHandler.GetString(idIndex);
	if (id == HashedString("List"))
	{
		ParseListBlob();
	}
	else if (id == HashedString("Func"))
	{
		ParseFunctionBlob();
	}
	else
	{
		mHandler.ReceiveBlob(mByteCode + blobStart, blobSize);
		mIndex = blobEnd;
	}

	if (mIndex != blobEnd)
	{
		CboIsCorrupt();
	}
}


void CboParserCore::ParseListBlob()
{
	if (!AssertBytesRemaining(sizeof(Value32)))
	{
		return;
	}

	const bu32_t numBlobs = ReadValue32().mUInt;
	for (bu32_t i = 0; (i < numBlobs) && !mHasError; ++i)
	{
		ParseBlob();
	}
}


void CboParserCore::ParseFunctionBlob()
{
	if (!AssertBytesRemaining(sizeof(Value32)))
	{
		return;
	}

	const bu32_t hash = ReadValue32().mUInt;
	QualifiedId name = ParseQualifiedIdentifier();

	if (!AssertBytesRemaining(sizeof(Value32)))
	{
		return;
	}

	const bu32_t codeSize = ReadValue32().mUInt;
	if (!AssertBytesRemaining(codeSize))
	{
		return;
	}

	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;

	// Do a validation pass on the byte-code and ensure everything is converted to the correct endianness.
	while (!mHasError && (mIndex < codeEnd))
	{
		const OpCode opCode = static_cast<OpCode>(mByteCode[mIndex++]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
			case OC_PARAM_UCHAR:
				++mIndex;
				break;
			case OC_PARAM_SHORT:
			case OC_PARAM_USHORT:
				ReadValue16();
				break;
			case OC_PARAM_INT:
			case OC_PARAM_VAL32:
			{
				const bu32_t valueIndex = ReadValue16().mUShort;
				if (valueIndex >= mHandler.GetValue32TableSize())
				{
					mHandler.InvalidByteCode();
					mHasError = true;
				}
			}
			break;
			case OC_PARAM_VAL64:
			{
				const bu32_t valueIndex = ReadValue16().mUShort;
				if (valueIndex >= mHandler.GetValue64TableSize())
				{
					mHandler.InvalidByteCode();
					mHasError = true;
				}
			}
			break;
			case OC_PARAM_OFF16:
			{
				const bi32_t offset = ReadValue16().mShort;
				const bi32_t baseAddress = static_cast<bi32_t>(mIndex - codeStart);
				const bi32_t targetAddress = baseAddress + offset;
				if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
				{
					mHandler.InvalidByteCode();
					mHasError = true;
				}
			}
			break;
			case OC_PARAM_OFF32:
			{
				const bu32_t offsetIndex = ReadValue16().mUShort;
				if (offsetIndex >= mHandler.GetValue32TableSize())
				{
					mHandler.InvalidByteCode();
					mHasError = true;
				}
				else
				{
					const bi32_t offset = mHandler.GetValue32(offsetIndex).mInt;
					const bi32_t baseAddress = static_cast<bi32_t>(mIndex - codeStart);
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
					{
						mHandler.InvalidByteCode();
						mHasError = true;
					}
				}
			}
			break;
			case OC_PARAM_HASH:
				// TODO
				break;
		}
	}

	if (!mHasError)
	{
		mHandler.ReceiveFunctionBlob(name, hash, codeSize, mByteCode + codeStart);
		// TODO: parse metadata
		mHandler.EndFunctionBlob();
	}
}


QualifiedId CboParserCore::ParseQualifiedIdentifier()
{
	if (!AssertBytesRemaining(sizeof(Value16)))
	{
		return QualifiedId();
	}

	const int numElements = ReadValue16().mUShort;
	if (!AssertBytesRemaining(numElements * sizeof(Value16)))
	{
		return QualifiedId();
	}

	unsigned char *elementIndices = mByteCode + mIndex;
	mIndex += numElements * sizeof(Value16);
	if (mReverseByteOrder)
	{
		BatchReverseByteOrder16(elementIndices, numElements);
	}

	return QualifiedId(numElements, reinterpret_cast<const bu16_t *>(elementIndices));
}


Value16 CboParserCore::ReadValue16()
{
	if (mReverseByteOrder)
	{
		ReverseByteOrder16(mByteCode + mIndex);
	}
	const Value16 value(mByteCode + mIndex);
	mIndex += sizeof(Value16);
	return value;
}


Value32 CboParserCore::ReadValue32()
{
	if (mReverseByteOrder)
	{
		ReverseByteOrder32(mByteCode + mIndex);
	}
	const Value32 value(mByteCode + mIndex);
	mIndex += sizeof(Value32);
	return value;
}


Value64 CboParserCore::ReadValue64()
{
	if (mReverseByteOrder)
	{
		ReverseByteOrder64(mByteCode + mIndex);
	}
	const Value64 value(mByteCode + mIndex);
	mIndex += sizeof(Value64);
	return value;
}


bool CboParserCore::AssertBytesRemaining(size_t numBytes)
{
	if (!mHasError && ((mIndex + numBytes) > mLength))
	{
		CboIsCorrupt();
	}
	return !mHasError;
}


void CboParserCore::CboIsCorrupt()
{
	if (!mHasError)
	{
		mHandler.CboIsCorrupt();
		mHasError = true;
	}
}

}
