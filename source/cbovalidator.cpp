#include "bond/systems/assert.h"
#include "bond/systems/endian.h"
#include "bond/systems/math.h"
#include "bond/tools/cboutil.h"
#include "bond/tools/cbovalidator.h"
#include "bond/types/opcodes.h"
#include "bond/types/stringutil.h"
#include "bond/version.h"

namespace Bond
{

class CboValidatorCore
{
public:
	CboValidatorCore(const bu8_t *byteCode, size_t length):
		mByteCode(reinterpret_cast<const bu8_t *>(byteCode)),
		mLength(length),
		mIndex(0),
		mValue32Table(nullptr)
	{}

	CboValidator::Result Validate();

private:
	void ValidateBlob();
	void ValidateListBlob();
	void ValidateFunctionBlob();
	void ValidateQualifiedIdentifier();
	void ValidateReturnSignature();
	void ValidateParamListSignature();

	Value16 ReadValue16();
	Value32 ReadValue32();

	void AssertBytesRemaining(size_t numBytes) const;
	void FunctionIsInvalid() const;
	void CodeIsInvalid() const;
	void CboIsInvalid() const;

	CboValidator::Result mResult;
	const bu8_t *mByteCode;
	size_t mLength;
	size_t mIndex;
	const Value32 *mValue32Table;
};


CboValidator::Result CboValidator::Validate(const void *byteCode, size_t length)
{
	CboValidatorCore validator(static_cast<const bu8_t *>(byteCode), length);
	return validator.Validate();
}


CboValidator::Result CboValidatorCore::Validate()
{
	AssertBytesRemaining((2 * sizeof(Value32)) + (6 * sizeof(Value16)));

	const bu32_t magicNumber = ReadValue32().mUInt;
	const bu32_t majorVersion = ReadValue16().mUShort;
	const bu32_t minorVersion = ReadValue16().mUShort;
	const bu16_t flags = ReadValue16().mUShort;
	const size_t tableStart = mIndex;
	const size_t tableSize = ReadValue32().mUInt;
	const size_t value32Count = ReadValue16().mUShort;
	const size_t value64Count = ReadValue16().mUShort;
	const size_t stringCount = ReadValue16().mUShort;

	BOND_ASSERT_FORMAT(magicNumber == MAGIC_NUMBER, ("CBO file contains invalid magic number: 0x%" BOND_PRIx32 ".", magicNumber));
	BOND_ASSERT_FORMAT(majorVersion == MAJOR_VERSION, ("Unexpected major version: %" BOND_PRIu32 ".", majorVersion));
	BOND_ASSERT_FORMAT(minorVersion == MINOR_VERSION, ("Unexpected minor version: %02" BOND_PRIu32 ".", minorVersion));

	mResult.mMajorVersion = majorVersion;
	mResult.mMinorVersion = minorVersion;
	mResult.mPointerSize = DecodePointerSize(flags);
	mResult.mValue32Count = value32Count;
	mResult.mValue64Count = value64Count;
	mResult.mStringCount = stringCount;

	const size_t valueSize = (value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64));
	AssertBytesRemaining(tableSize - (mIndex - tableStart));
	AssertBytesRemaining(valueSize);

	mValue32Table = reinterpret_cast<const Value32 *>(mByteCode + mIndex);
	mIndex += (value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64));

	size_t stringByteCount = 0;
	for (size_t i = 0; i < stringCount; ++i)
	{
		AssertBytesRemaining(sizeof(Value16));
		const size_t stringLength = ReadValue16().mUShort;
		stringByteCount += stringLength;

		AssertBytesRemaining(stringLength);
		const char *str = reinterpret_cast<const char *>(mByteCode + mIndex);
		if (StringEqual(str, stringLength, "List", 4))
		{
			mResult.mListBlobIdIndex = i;
		}
		else if (StringEqual(str, stringLength, "Func", 4))
		{
			mResult.mFunctionBlobIdIndex = i;
		}
		mIndex += stringLength;
	}

	mResult.mStringByteCount += stringByteCount;

	const size_t tableEnd = tableStart + tableSize;
	if (mIndex != tableEnd)
	{
		CboIsInvalid();
		return mResult;
	}

	ValidateBlob();

	if (mIndex != mLength)
	{
		CboIsInvalid();
	}

	return mResult;
}


void CboValidatorCore::ValidateBlob()
{
	AssertBytesRemaining(sizeof(Value32) + sizeof(Value16));

	const size_t blobStart = mIndex;
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	AssertBytesRemaining(blobSize - (mIndex - blobStart));

	if (idIndex == mResult.mListBlobIdIndex)
	{
		ValidateListBlob();
	}
	else if (idIndex == mResult.mFunctionBlobIdIndex)
	{
		ValidateFunctionBlob();
	}
	else
	{
		mIndex = blobEnd;
	}

	if (mIndex != blobEnd)
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


void CboValidatorCore::ValidateFunctionBlob()
{
	++mResult.mFunctionCount;
	ValidateReturnSignature();
	ValidateQualifiedIdentifier();
	ValidateParamListSignature();

	AssertBytesRemaining(6 * sizeof(Value32));

	// Ignore the hash.
	mIndex += sizeof(Value32);

	const bu32_t argSize = ReadValue32().mUInt;
	const bu32_t packedArgSize = ReadValue32().mUInt;
	const bu32_t localSize = ReadValue32().mUInt;
	mIndex += sizeof(Value32); // Ignore the stack size.
	const bu32_t framePointerAlignment = ReadValue32().mUInt;
	const size_t codeSize = ReadValue32().mUInt;

	if ((packedArgSize > argSize) ||
	    ((argSize % BOND_SLOT_SIZE) != 0) ||
	    ((packedArgSize % BOND_SLOT_SIZE) != 0) ||
	    ((localSize % BOND_SLOT_SIZE) != 0) ||
	    ((framePointerAlignment % BOND_SLOT_SIZE) != 0))
	{
		FunctionIsInvalid();
	}

	mResult.mCodeByteCount += AlignUp(codeSize, sizeof(Value32));
	AssertBytesRemaining(codeSize);
	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;

	// Do a validation pass on the byte-code and ensure everything is converted to the correct endianness.
	while (mIndex < codeEnd)
	{
		const OpCode opCode = static_cast<OpCode>(mByteCode[mIndex++]);
		const OpCodeParam param = GetOpCodeParamType(opCode);

		switch (param)
		{
			case OC_PARAM_NONE:
				break;
			case OC_PARAM_CHAR:
			case OC_PARAM_UCHAR:
				AssertBytesRemaining(1);
				++mIndex;
				break;
			case OC_PARAM_UCHAR_CHAR:
			case OC_PARAM_SHORT:
			case OC_PARAM_USHORT:
				AssertBytesRemaining(sizeof(Value16));
				ReadValue16();
				break;
			case OC_PARAM_INT:
			case OC_PARAM_VAL32:
			case OC_PARAM_HASH:
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
				const bi32_t offset = ReadValue16().mShort;
				const bi32_t baseAddress = bi32_t(mIndex - codeStart);
				const bi32_t targetAddress = baseAddress + offset;
				if ((targetAddress < 0) || (bu32_t(targetAddress) > codeSize))
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
					const bi32_t offset = ConvertBigEndian32(mValue32Table[offsetIndex]).mInt;
					const bi32_t baseAddress = bi32_t(mIndex - codeStart);
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (bu32_t(targetAddress) > codeSize))
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
			case OC_PARAM_LOOKUPSWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				AssertBytesRemaining(2 * sizeof(Value32));

				const bi32_t defaultOffset = ReadValue32().mInt;
				const bu32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const bi32_t baseAddress = bi32_t(mIndex + tableSize - codeStart);
				const bi32_t defaultAddress = baseAddress + defaultOffset;

				AssertBytesRemaining(tableSize);
				if ((defaultAddress < 0) || (bu32_t(defaultAddress) > codeSize))
				{
					CodeIsInvalid();
				}

				for (bu32_t i = 0; i < numMatches; ++i)
				{
					// Skip the match.
					mIndex += sizeof(Value32);
					const bi32_t offset = ReadValue32().mInt;
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (bu32_t(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				AssertBytesRemaining(3 * sizeof(Value32));

				const bi32_t defaultOffset = ReadValue32().mInt;
				const bi32_t minMatch = ReadValue32().mInt;
				const bi32_t maxMatch = ReadValue32().mInt;
				const bu32_t numMatches = bu32_t(maxMatch - minMatch + 1);
				const size_t tableSize = numMatches * sizeof(Value32);
				const bi32_t baseAddress = bi32_t(mIndex + tableSize - codeStart);
				const bi32_t defaultAddress = baseAddress + defaultOffset;

				AssertBytesRemaining(tableSize);
				if (minMatch > maxMatch)
				{
					CodeIsInvalid();
				}
				if ((defaultAddress < 0) || (bu32_t(defaultAddress) > codeSize))
				{
					CodeIsInvalid();
				}

				for (size_t i = 0; i < numMatches; ++i)
				{
					const bi32_t offset = ReadValue32().mInt;
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (bu32_t(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
		}
	}
}


void CboValidatorCore::ValidateQualifiedIdentifier()
{
	AssertBytesRemaining(sizeof(Value16));

	const size_t numElements = ReadValue16().mUShort;
	AssertBytesRemaining(numElements * sizeof(Value16));

	++mResult.mQualifiedIdCount;
	mResult.mQualifiedIdElementCount += numElements;

	for (size_t i = 0; i < numElements; ++i)
	{
		const size_t elementIndex = ReadValue16().mUShort;
		if (elementIndex >= mResult.mStringCount)
		{
			CboIsInvalid();
		}
	}
}


void CboValidatorCore::ValidateReturnSignature()
{
	AssertBytesRemaining(sizeof(Value32));
	const bu32_t returnSizeAndType = ReadValue32().mUInt;
	bu32_t returnSize;
	bu32_t returnType;
	DecodeSizeAndType(returnSizeAndType, returnSize, returnType);
	// TODO: Do some sanity checks.
}


void CboValidatorCore::ValidateParamListSignature()
{
	AssertBytesRemaining(sizeof(Value16));

	const size_t numParams = ReadValue16().mUShort;
	AssertBytesRemaining(numParams * 2 * sizeof(Value32));

	++mResult.mParamListSignatureCount;
	mResult.mParamSignatureCount += numParams;

	bi32_t prevOffset = 0;
	for (size_t i = 0; i < numParams; ++i)
	{
		const bi32_t offset = ReadValue32().mInt;
		if (offset >= prevOffset)
		{
			FunctionIsInvalid();
		}
		prevOffset = offset;

		const bu32_t paramSizeAndType = ReadValue32().mUInt;
		bu32_t paramSize;
		bu32_t paramType;
		DecodeSizeAndType(paramSizeAndType, paramSize, paramType);
		// TODO: Do some sanity checks.
	}
}


Value16 CboValidatorCore::ReadValue16()
{
	const Value16 value(mByteCode + mIndex);
	mIndex += sizeof(Value16);
	return ConvertBigEndian16(value);
}


Value32 CboValidatorCore::ReadValue32()
{
	const Value32 value(mByteCode + mIndex);
	mIndex += sizeof(Value32);
	return ConvertBigEndian32(value);
}


void CboValidatorCore::AssertBytesRemaining(size_t numBytes) const
{
	if ((mIndex + numBytes) > mLength)
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
	BOND_FAIL_MESSAGE("CBO file contains invalid byte-code");
}


void CboValidatorCore::CboIsInvalid() const
{
	BOND_FAIL_MESSAGE("CBO file is incomplete or malformed.");
}

}
