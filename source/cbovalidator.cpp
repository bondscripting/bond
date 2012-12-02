#include "bond/cboutil.h"
#include "bond/cbovalidator.h"
#include "bond/endian.h"
#include "bond/math.h"
#include "bond/opcodes.h"
#include "bond/textwriter.h"
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
		mValue32Table(NULL)
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
	Value64 ReadValue64();

	bool HasError() const { return mResult.mStatus != CboValidator::CBO_VALID; }
	bool AssertBytesRemaining(size_t numBytes);
	void FunctionIsInvalid();
	void CodeIsInvalid();
	void CboIsInvalid();

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


void CboValidator::WriteStatus(TextWriter& writer, Status status)
{
	switch (status)
	{
		case CBO_VALID:
			writer.Write("CBO file valid\n");
		break;

		case CBO_INVALID_MAGIC_NUMBER:
			writer.Write("CBO file's magic number is incorrect\n");
			break;

		case CBO_INVALID_VERSION:
			writer.Write("CBO file's version is unknown\n");
			break;

		case CBO_INVALID_FUNCTION_DESCRIPTION:
			writer.Write("CBO file contains an invalid function description\n");
			break;

		case CBO_INVALID_BYTECODE:
			writer.Write("CBO file contains invalid bytecode\n");
			break;

		case CBO_INVALID_FORMAT:
			writer.Write("CBO file is incomplete or malformed\n");
			break;
	}
}


CboValidator::Result CboValidatorCore::Validate()
{
	if (!AssertBytesRemaining((2 * sizeof(Value32)) + (6 * sizeof(Value16))))
	{
		return mResult;
	}

	const bu32_t magicNumber = ReadValue32().mUInt;
	if (magicNumber != MAGIC_NUMBER)
	{
		mResult.mStatus = CboValidator::CBO_INVALID_MAGIC_NUMBER;
		return mResult;
	}

	const int majorVersion = ReadValue16().mUShort;
	const int minorVersion = ReadValue16().mUShort;
	const bu16_t flags = ReadValue16().mUShort;
	if ((majorVersion != MAJOR_VERSION) && (minorVersion != MINOR_VERSION))
	{
		mResult.mStatus = CboValidator::CBO_INVALID_VERSION;
		return mResult;
	}

	mResult.mMajorVersion = majorVersion;
	mResult.mMinorVersion = minorVersion;
	mResult.mPointerSize = DecodePointerSize(flags);

	const size_t tableStart = mIndex;
	const size_t tableSize = ReadValue32().mUInt;
	const size_t value32Count = ReadValue16().mUShort;
	const size_t value64Count = ReadValue16().mUShort;
	const size_t stringCount = ReadValue16().mUShort;
	mResult.mValue32Count = value32Count;
	mResult.mValue64Count = value64Count;
	mResult.mStringCount = stringCount;

	const size_t valueSize = (value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64));
	if (!AssertBytesRemaining(tableSize - (mIndex - tableStart)) || !AssertBytesRemaining(valueSize))
	{
		return mResult;
	}

	mValue32Table = reinterpret_cast<const Value32 *>(mByteCode + mIndex);
	mIndex += (value32Count * sizeof(Value32)) + (value64Count * sizeof(Value64));

	size_t stringByteCount = 0;
	for (size_t i = 0; i < stringCount; ++i)
	{
		if (!AssertBytesRemaining(sizeof(Value16)))
		{
			return mResult;
		}

		const int stringLength = ReadValue16().mUShort;
		stringByteCount += stringLength;
		if (!AssertBytesRemaining(stringLength))
		{
			return mResult;
		}

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
	if (!AssertBytesRemaining(sizeof(Value32) + sizeof(Value16)))
	{
		return;
	}

	const size_t blobStart = mIndex;
	const size_t blobSize = ReadValue32().mUInt;
	const size_t blobEnd = blobStart + blobSize;
	const size_t idIndex = ReadValue16().mUShort;

	if (!AssertBytesRemaining(blobSize - (mIndex - blobStart)))
	{
		return;
	}

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
	if (!AssertBytesRemaining(sizeof(Value32)))
	{
		return;
	}

	const size_t numBlobs = ReadValue32().mUInt;
	for (size_t i = 0; (i < numBlobs) && !HasError(); ++i)
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

	if (!AssertBytesRemaining(6 * sizeof(Value32)))
	{
		return;
	}

	// Ignore the hash.
	mIndex += sizeof(Value32);

	const bu32_t argSize = ReadValue32().mUInt;
	const bu32_t packedArgSize = ReadValue32().mUInt;
	const bu32_t localSize = ReadValue32().mUInt;
	const bu32_t framePointerAlignment = ReadValue32().mUInt;
	if ((packedArgSize > argSize) ||
	    ((argSize % BOND_SLOT_SIZE) != 0) ||
	    ((packedArgSize % BOND_SLOT_SIZE) != 0) ||
	    ((localSize % BOND_SLOT_SIZE) != 0) ||
	    ((framePointerAlignment % BOND_SLOT_SIZE) != 0))
	{
		FunctionIsInvalid();
		return;
	}

	const size_t codeSize = ReadValue32().mUInt;
	mResult.mCodeByteCount += AlignUp(codeSize, sizeof(Value32));
	if (!AssertBytesRemaining(codeSize))
	{
		return;
	}

	const size_t codeStart = mIndex;
	const size_t codeEnd = mIndex + codeSize;

	// Do a validation pass on the byte-code and ensure everything is converted to the correct endianness.
	while (!HasError() && (mIndex < codeEnd))
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
			{
				if (AssertBytesRemaining(sizeof(Value16)))
				{
					const size_t valueIndex = ReadValue16().mUShort;
					if (valueIndex >= mResult.mValue32Count)
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_VAL64:
			{
				if (AssertBytesRemaining(sizeof(Value16)))
				{
					const size_t valueIndex = ReadValue16().mUShort;
					if (valueIndex >= mResult.mValue64Count)
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_OFF16:
			{
				if (AssertBytesRemaining(sizeof(Value16)))
				{
					const bi32_t offset = ReadValue16().mShort;
					const bi32_t baseAddress = static_cast<bi32_t>(mIndex - codeStart);
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_OFF32:
			{
				if (AssertBytesRemaining(sizeof(Value16)))
				{
					const size_t offsetIndex = ReadValue16().mUShort;
					if (offsetIndex >= mResult.mValue32Count)
					{
						CodeIsInvalid();
					}
					else
					{
						const bi32_t offset = ConvertBigEndian32(mValue32Table[offsetIndex]).mInt;
						const bi32_t baseAddress = static_cast<bi32_t>(mIndex - codeStart);
						const bi32_t targetAddress = baseAddress + offset;
						if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
						{
							CodeIsInvalid();
						}
					}
				}
			}
			break;
			case OC_PARAM_HASH:
			{
				mIndex += GetPointerSize(mResult.mPointerSize);
			}
			break;
			case OC_PARAM_LOOKUPSWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				if (!AssertBytesRemaining(2 * sizeof(Value32)))
				{
					break;
				}

				const bi32_t defaultOffset = ReadValue32().mInt;
				const bu32_t numMatches = ReadValue32().mUInt;
				const size_t tableSize = numMatches * 2 * sizeof(Value32);
				const bi32_t baseAddress = static_cast<bi32_t>(mIndex + tableSize - codeStart);

				if (!AssertBytesRemaining(tableSize))
				{
					break;
				}

				const bi32_t targetAddress = baseAddress + defaultOffset;
				if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
				{
					CodeIsInvalid();
				}

				for (bu32_t i = 0; i < numMatches; ++i)
				{
					// Skip the match.
					mIndex += sizeof(Value32);
					const bi32_t offset = ReadValue32().mInt;
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
					{
						CodeIsInvalid();
					}
				}
			}
			break;
			case OC_PARAM_TABLESWITCH:
			{
				mIndex = codeStart + AlignUp(mIndex - codeStart, sizeof(Value32));
				if (!AssertBytesRemaining(3 * sizeof(Value32)))
				{
					break;
				}

				const bi32_t defaultOffset = ReadValue32().mInt;
				const bi32_t minMatch = ReadValue32().mInt;
				const bi32_t maxMatch = ReadValue32().mInt;

				if (minMatch > maxMatch)
				{
					CodeIsInvalid();
					break;
				}

				const bu32_t numMatches = maxMatch - minMatch + 1;
				const size_t tableSize = numMatches * sizeof(Value32);
				const bi32_t baseAddress = static_cast<bi32_t>(mIndex + tableSize - codeStart);

				if (!AssertBytesRemaining(tableSize))
				{
					break;
				}

				const bi32_t targetAddress = baseAddress + defaultOffset;
				if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
				{
					CodeIsInvalid();
					break;
				}

				for (size_t i = 0; i < numMatches; ++i)
				{
					const bi32_t offset = ReadValue32().mInt;
					const bi32_t targetAddress = baseAddress + offset;
					if ((targetAddress < 0) || (static_cast<bu32_t>(targetAddress) > codeSize))
					{
						CodeIsInvalid();
						break;
					}
				}
			}
			break;
		}
	}
}


void CboValidatorCore::ValidateQualifiedIdentifier()
{
	if (!AssertBytesRemaining(sizeof(Value16)))
	{
		return;
	}

	const size_t numElements = ReadValue16().mUShort;
	if (!AssertBytesRemaining(numElements * sizeof(Value16)))
	{
		return;
	}

	++mResult.mQualifiedIdCount;
	mResult.mQualifiedIdElementCount += numElements;

	for (size_t i = 0; i < numElements; ++i)
	{
		const size_t elementIndex = ReadValue16().mUShort;
		if (elementIndex >= mResult.mStringCount)
		{
			CboIsInvalid();
			return;
		}
	}
}


void CboValidatorCore::ValidateReturnSignature()
{
	if (!AssertBytesRemaining(sizeof(Value32)))
	{
		return;
	}

	const bu32_t returnSizeAndType = ReadValue32().mUInt;
	bu32_t returnSize;
	bu32_t returnType;
	DecodeSizeAndType(returnSizeAndType, returnSize, returnType);
	// TODO: Do some sanity checks.
}


void CboValidatorCore::ValidateParamListSignature()
{
	if (!AssertBytesRemaining(sizeof(Value16)))
	{
		return;
	}

	const size_t numParams = ReadValue16().mUShort;
	if (!AssertBytesRemaining(numParams * 2 * sizeof(Value32)))
	{
		return;
	}

	++mResult.mParamListSignatureCount;
	mResult.mParamSignatureCount += numParams;

	bi32_t prevOffset = 0;
	for (size_t i = 0; i < numParams; ++i)
	{
		const bi32_t offset = ReadValue32().mInt;
		if (offset >= prevOffset)
		{
			FunctionIsInvalid();
			return;
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


Value64 CboValidatorCore::ReadValue64()
{
	const Value64 value(mByteCode + mIndex);
	mIndex += sizeof(Value64);
	return ConvertBigEndian64(value);
}


bool CboValidatorCore::AssertBytesRemaining(size_t numBytes)
{
	if (!HasError() && ((mIndex + numBytes) > mLength))
	{
		CboIsInvalid();
	}
	return !HasError();
}


void CboValidatorCore::FunctionIsInvalid()
{
	if (!HasError())
	{
		mResult.mStatus = CboValidator::CBO_INVALID_FUNCTION_DESCRIPTION;
	}
}


void CboValidatorCore::CodeIsInvalid()
{
	if (!HasError())
	{
		mResult.mStatus = CboValidator::CBO_INVALID_BYTECODE;
	}
}


void CboValidatorCore::CboIsInvalid()
{
	if (!HasError())
	{
		mResult.mStatus = CboValidator::CBO_INVALID_FORMAT;
	}
}

}
