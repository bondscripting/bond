#include "bond/io/memoryinputstream.h"
#include "bond/systems/assert.h"
#include "bond/systems/endian.h"
#include "bond/systems/math.h"
#include "bond/tools/cboutil.h"
#include "bond/tools/cbovalidator.h"
#include "bond/types/opcodes.h"
#include "bond/types/stringutil.h"

namespace Bond
{

class CboValidatorCore
{
public:
	explicit CboValidatorCore(InputStream &stream):
		mStream(stream),
		mStreamEnd(stream.GetEndPosition()),
		mValue32TableOffset(0)
	{}

	CboValidatorCore(const CboValidatorCore &other) = delete;
	CboValidatorCore &operator=(const CboValidatorCore &other) = delete;

	CboValidator::Result Validate();

private:
	void ValidateBlob();
	void ValidateListBlob();
	void ValidateFunctionBlob(size_t blobEnd);
	void ValidateDataBlob(size_t blobEnd);
	size_t ValidateQualifiedNameIndex();
	void ValidateSizeAndType();
	void ValidateParamListSignature();

	Value16 ReadValue16();
	Value32 ReadValue32();
	Value32 ReadValue32TableAt(size_t index);

	void AssertBytesRemaining(size_t numBytes) const;
	void FunctionIsInvalid() const;
	void CodeIsInvalid() const;
	void CboIsInvalid() const;

	size_t GetPosition() const { return size_t(mStream.GetPosition()); }
	void SetPosition(size_t pos) { mStream.SetPosition(Stream::pos_t(pos)); }
	void Skip(size_t numBytes) { mStream.AddOffset(Stream::off_t(numBytes)); }

	CboValidator::Result mResult;
	InputStream& mStream;
	size_t mStreamEnd;
	size_t mValue32TableOffset;
};


CboValidator::Result CboValidator::Validate(const void *byteCode, size_t length)
{
	MemoryInputStream stream(byteCode, Stream::pos_t(length));
	CboValidatorCore validator(stream);
	return validator.Validate();
}


CboValidator::Result CboValidator::Validate(InputStream &stream)
{
	CboValidatorCore validator(stream);
	return validator.Validate();
}


CboValidator::Result CboValidatorCore::Validate()
{
	const uint32_t magicNumber = ReadValue32().mUInt;
	const uint32_t majorVersion = ReadValue16().mUShort;
	const uint32_t minorVersion = ReadValue16().mUShort;
	const uint16_t flags = ReadValue16().mUShort;
	const size_t tableStart = GetPosition();
	const size_t tableSize = ReadValue32().mUInt;
	const size_t value32Count = ReadValue16().mUShort;
	const size_t value64Count = ReadValue16().mUShort;
	const size_t stringCount = ReadValue16().mUShort;
	const size_t qualifiedNameCount = ReadValue16().mUShort;

	BOND_ASSERT_FORMAT(magicNumber == MAGIC_NUMBER, ("CBO file contains invalid magic number: 0x%" BOND_PRIx32 ".", magicNumber));
	BOND_ASSERT_FORMAT(IsCBOFormatLoadable(majorVersion, minorVersion), ("Unsupported CBO file format version: %" BOND_PRIu32 ".%" BOND_PRIu32 ". Only CBO file format versions in the range %" BOND_PRIu32 ".%" BOND_PRIu32 " through %" BOND_PRIu32 ".XX are supported.", majorVersion, minorVersion, CBO_MIN_SUPPORTED_MAJOR_VERSION, CBO_MIN_SUPPORTED_MINOR_VERSION, CBO_MAX_SUPPORTED_MAJOR_VERSION));

	mResult.mMajorVersion = majorVersion;
	mResult.mMinorVersion = minorVersion;
	mResult.mPointerSize = DecodePointerSize(flags);
	mResult.mValue32Count = value32Count;
	mResult.mValue64Count = value64Count;
	mResult.mStringCount = stringCount;
	mResult.mQualifiedNameCount = qualifiedNameCount;

	const size_t valueSize = (value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64));
	AssertBytesRemaining(tableSize - (GetPosition() - tableStart));
	AssertBytesRemaining(valueSize);

	mValue32TableOffset = GetPosition();
	Skip((value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64)));

	size_t stringByteCount = 0;
	for (size_t i = 0; i < stringCount; ++i)
	{
		AssertBytesRemaining(sizeof(Value16));
		const size_t stringLength = ReadValue16().mUShort;
		stringByteCount += stringLength;

		AssertBytesRemaining(stringLength);
		if (stringLength == BOND_BLOB_ID_LENGTH)
		{
			char str[BOND_BLOB_ID_LENGTH];
			mStream.Read(str, BOND_BLOB_ID_LENGTH);
			if (StringEqual(str, BOND_BLOB_ID_LENGTH, BOND_LIST_BLOB_ID, BOND_BLOB_ID_LENGTH))
			{
				mResult.mListBlobIdIndex = i;
			}
			else if (StringEqual(str, BOND_BLOB_ID_LENGTH, BOND_FUNCTION_BLOB_ID, BOND_BLOB_ID_LENGTH))
			{
				mResult.mFunctionBlobIdIndex = i;
			}
			else if (StringEqual(str, BOND_BLOB_ID_LENGTH, BOND_DATA_BLOB_ID, BOND_BLOB_ID_LENGTH))
			{
				mResult.mDataBlobIdIndex = i;
			}
		}
		else
		{
			mStream.AddOffset(Stream::pos_t(stringLength));
		}
	}
	mResult.mStringByteCount = stringByteCount;

	size_t qualifiedNameElementCount = 0;
	for (size_t i = 0; i < qualifiedNameCount; ++i)
	{
		AssertBytesRemaining(sizeof(Value16));
		const size_t numElements = ReadValue16().mUShort;
		AssertBytesRemaining(numElements * sizeof(Value16));
		qualifiedNameElementCount += numElements;

		if (numElements == 0)
		{
			mResult.mStaticInitializerNameIndex = i;
		}

		for (size_t j = 0; j < numElements; ++j)
		{
			const size_t elementIndex = ReadValue16().mUShort;
			if (elementIndex >= stringCount)
			{
				CboIsInvalid();
			}
		}
	}
	mResult.mQualifiedNameElementCount = qualifiedNameElementCount;

	const size_t tableEnd = tableStart + tableSize;
	if (GetPosition() != tableEnd)
	{
		CboIsInvalid();
		return mResult;
	}

	mResult.mDataAlignment = size_t(BOND_SLOT_SIZE);
	ValidateBlob();

	if (GetPosition() != mStreamEnd)
	{
		CboIsInvalid();
	}

	return mResult;
}


void CboValidatorCore::ValidateBlob()
{
	AssertBytesRemaining(sizeof(Value32) + sizeof(Value16));
	const size_t blobStart = GetPosition();
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	AssertBytesRemaining(blobSize - (GetPosition() - blobStart));

	if (idIndex == mResult.mListBlobIdIndex)
	{
		ValidateListBlob();
	}
	else if (idIndex == mResult.mFunctionBlobIdIndex)
	{
		ValidateFunctionBlob(blobEnd);
	}
	else if (idIndex == mResult.mDataBlobIdIndex)
	{
		ValidateDataBlob(blobEnd);
	}
	else
	{
		SetPosition(blobEnd);
	}

	if (GetPosition() != blobEnd)
	{
		CboIsInvalid();
	}
}


void CboValidatorCore::ValidateListBlob()
{
	AssertBytesRemaining(sizeof(Value32));
	const size_t numBlobs = ReadValue32().mUInt;
	for (size_t i = 0; i < numBlobs; ++i)
	{
		ValidateBlob();
	}
}


void CboValidatorCore::ValidateFunctionBlob(size_t blobEnd)
{
	const size_t functionNameIndex = ValidateQualifiedNameIndex();
	ValidateSizeAndType();
	ValidateParamListSignature();

	AssertBytesRemaining(6 * sizeof(Value32));
	const uint32_t argSize = ReadValue32().mUInt;
	const uint32_t packedArgSize = ReadValue32().mUInt;
	const uint32_t localSize = ReadValue32().mUInt;
	Skip(sizeof(Value32)); // Ignore the stack size.
	const uint32_t framePointerAlignment = ReadValue32().mUInt;
	const size_t codeSize = ReadValue32().mUInt;

	if ((packedArgSize > argSize) ||
	    ((argSize % BOND_SLOT_SIZE) != 0) ||
	    ((packedArgSize % BOND_SLOT_SIZE) != 0) ||
	    ((localSize % BOND_SLOT_SIZE) != 0) ||
	    ((framePointerAlignment % BOND_SLOT_SIZE) != 0))
	{
		FunctionIsInvalid();
	}

	if (functionNameIndex == mResult.mStaticInitializerNameIndex)
	{
		++mResult.mStaticInitializerCount;
	}
	else
	{
		++mResult.mFunctionCount;
	}

	mResult.mCodeByteCount += AlignUp(codeSize, sizeof(Value32));
	AssertBytesRemaining(codeSize);
	const size_t codeStart = GetPosition();
	const size_t codeEnd = codeStart + codeSize;

	// Do a validation pass on the bytecode and ensure everything is converted to the correct endianness.
	while (GetPosition() < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(mStream.Read());
		const OpCodeParam param = GetOpCodeParamType(opCode);

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
			case OC_PARAM_UCHAR:
				AssertBytesRemaining(1);
				Skip(1);
				break;
			case OC_PARAM_UCHAR_CHAR:
			case OC_PARAM_SHORT:
			case OC_PARAM_USHORT:
				AssertBytesRemaining(sizeof(Value16));
				ReadValue16();
				break;
			case OC_PARAM_INT:
			case OC_PARAM_VAL32:
			{
				AssertBytesRemaining(sizeof(Value16));
				const size_t valueIndex = ReadValue16().mUShort;
				if (valueIndex >= mResult.mValue32Count)
				{
					CodeIsInvalid();
				}
			}
			break;
			case OC_PARAM_VAL64:
			{
				AssertBytesRemaining(sizeof(Value16));
				const size_t valueIndex = ReadValue16().mUShort;
				if (valueIndex >= mResult.mValue64Count)
				{
					CodeIsInvalid();
				}
			}
			break;
			case OC_PARAM_OFF16:
			{
				AssertBytesRemaining(sizeof(Value16));
				const int32_t offset = ReadValue16().mShort;
				const int32_t baseAddress = int32_t(GetPosition() - codeStart);
				const int32_t targetAddress = baseAddress + offset;
				if ((targetAddress < 0) || (uint32_t(targetAddress) > codeSize))
				{
					CodeIsInvalid();
				}
			}
			break;
			case OC_PARAM_OFF32:
			{
				AssertBytesRemaining(sizeof(Value16));
				const size_t offsetIndex = ReadValue16().mUShort;
				if (offsetIndex >= mResult.mValue32Count)
				{
					CodeIsInvalid();
				}
				else
				{
					const int32_t offset = ReadValue32TableAt(offsetIndex).mInt;
					const int32_t baseAddress = int32_t(GetPosition() - codeStart);
					const int32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (uint32_t(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_STRING:
			{
				AssertBytesRemaining(sizeof(Value16));
				const size_t stringIndex = ReadValue16().mUShort;
				if (stringIndex >= mResult.mStringCount)
				{
					CodeIsInvalid();
				}
			}
			break;
			case OC_PARAM_NAME:
			{
				AssertBytesRemaining(sizeof(Value16));
				const size_t nameIndex = ReadValue16().mUShort;
				if (nameIndex >= mResult.mQualifiedNameCount)
				{
					CodeIsInvalid();
				}
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				Skip(AlignUpDelta(GetPosition() - codeStart, sizeof(Value32)));
				AssertBytesRemaining(2 * sizeof(Value32));
				const int32_t defaultOffset = ReadValue32().mInt;
				const uint32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const int32_t baseAddress = int32_t(GetPosition() + tableSize - codeStart);
				const int32_t defaultAddress = baseAddress + defaultOffset;

				AssertBytesRemaining(tableSize);
				if ((defaultAddress < 0) || (uint32_t(defaultAddress) > codeSize))
				{
					CodeIsInvalid();
				}

				for (uint32_t i = 0; i < numMatches; ++i)
				{
					// Skip the match.
					Skip(sizeof(Value32));
					const int32_t offset = ReadValue32().mInt;
					const int32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (uint32_t(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				Skip(AlignUpDelta(GetPosition() - codeStart, sizeof(Value32)));
				AssertBytesRemaining(3 * sizeof(Value32));
				const int32_t defaultOffset = ReadValue32().mInt;
				const int32_t minMatch = ReadValue32().mInt;
				const int32_t maxMatch = ReadValue32().mInt;
				const uint32_t numMatches = uint32_t(maxMatch - minMatch + 1);
				const size_t tableSize = numMatches * sizeof(Value32);
				const int32_t baseAddress = int32_t(GetPosition() + tableSize - codeStart);
				const int32_t defaultAddress = baseAddress + defaultOffset;

				AssertBytesRemaining(tableSize);
				if (minMatch > maxMatch)
				{
					CodeIsInvalid();
				}
				if ((defaultAddress < 0) || (uint32_t(defaultAddress) > codeSize))
				{
					CodeIsInvalid();
				}

				for (size_t i = 0; i < numMatches; ++i)
				{
					const int32_t offset = ReadValue32().mInt;
					const int32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (uint32_t(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
		}
	}

	// Validate the optional metadata blob.
	if (GetPosition() < blobEnd)
	{
		ValidateBlob();
	}
}


void CboValidatorCore::ValidateDataBlob(size_t blobEnd)
{
	++mResult.mDataCount;

	ValidateQualifiedNameIndex();

	AssertBytesRemaining(2 * sizeof(Value32));
	const uint32_t sizeAndType = ReadValue32().mUInt;
	uint32_t size;
	SignatureType type;
	DecodeSizeAndType(sizeAndType, size, type);
	const Value32 payload = ReadValue32();
	size_t alignment = size_t(BOND_SLOT_SIZE);

	if (type == SIG_AGGREGATE)
	{
		alignment = Max(alignment, size_t(payload.mUInt));
	}

	mResult.mDataSize = AlignUp(mResult.mDataSize, alignment);
	mResult.mDataSize += size;
	mResult.mDataAlignment = Max(mResult.mDataAlignment, alignment);

	// Validate the optional metadata blob.
	if (GetPosition() < blobEnd)
	{
		ValidateBlob();
	}
}


size_t CboValidatorCore::ValidateQualifiedNameIndex()
{
	AssertBytesRemaining(sizeof(Value16));
	const size_t nameIndex = ReadValue16().mUShort;
	if (nameIndex >= mResult.mQualifiedNameCount)
	{
		CboIsInvalid();
	}
	return nameIndex;
}


void CboValidatorCore::ValidateSizeAndType()
{
	AssertBytesRemaining(sizeof(Value32));
	const uint32_t sizeAndType = ReadValue32().mUInt;
	uint32_t size;
	SignatureType type;
	DecodeSizeAndType(sizeAndType, size, type);
	// TODO: Do some sanity checks.
}


void CboValidatorCore::ValidateParamListSignature()
{
	AssertBytesRemaining(sizeof(Value16));
	const size_t numParams = ReadValue16().mUShort;
	AssertBytesRemaining(numParams * 2 * sizeof(Value32));
	++mResult.mParamListSignatureCount;
	mResult.mParamSignatureCount += numParams;

	int32_t prevOffset = 0;
	for (size_t i = 0; i < numParams; ++i)
	{
		const int32_t offset = ReadValue32().mInt;
		if (offset >= prevOffset)
		{
			FunctionIsInvalid();
		}
		prevOffset = offset;

		ValidateSizeAndType();
	}
}


Value16 CboValidatorCore::ReadValue16()
{
	Value16 value;
	mStream.Read(value.mBytes, sizeof(Value16));
	return ConvertBigEndian16(value);
}


Value32 CboValidatorCore::ReadValue32()
{
	Value32 value;
	mStream.Read(value.mBytes, sizeof(Value32));
	return ConvertBigEndian32(value);
}


Value32 CboValidatorCore::ReadValue32TableAt(size_t index)
{
	const auto pos = mStream.GetPosition();
	mStream.SetPosition(Stream::pos_t(mValue32TableOffset + (index * sizeof(Value32))));
	const Value32 value = ReadValue32();
	mStream.SetPosition(pos);
	return value;
}


void CboValidatorCore::AssertBytesRemaining(size_t numBytes) const
{
	if ((GetPosition() + numBytes) > mStreamEnd)
	{
		CboIsInvalid();
	}
}


void CboValidatorCore::FunctionIsInvalid() const
{
	BOND_FAIL_MESSAGE("CBO file contains an invalid function blob");
}


void CboValidatorCore::CodeIsInvalid() const
{
	BOND_FAIL_MESSAGE("CBO file contains invalid bytecode");
}


void CboValidatorCore::CboIsInvalid() const
{
	BOND_FAIL_MESSAGE("CBO file is incomplete or malformed.");
}

}
