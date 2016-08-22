#ifndef BOND_TOOLS_CBOVALIDATOR_H
#define BOND_TOOLS_CBOVALIDATOR_H

#include "bond/types/types.h"

namespace Bond
{

class InputStream;

class CboValidator
{
public:

	struct Result
	{
		Result():
			mMajorVersion(0),
			mMinorVersion(0),
			mPointerSize(POINTER_32BIT),
			mListBlobIdIndex(BOND_SIZE_MAX),
			mFunctionBlobIdIndex(BOND_SIZE_MAX),
			mDataBlobIdIndex(BOND_SIZE_MAX),
			mStaticInitializerNameIndex(BOND_SIZE_MAX),
			mValue32Count(0),
			mValue64Count(0),
			mStringCount(0),
			mStringByteCount(0),
			mQualifiedNameCount(0),
			mQualifiedNameElementCount(0),
			mParamListSignatureCount(0),
			mParamSignatureCount(0),
			mFunctionCount(0),
			mStaticInitializerCount(0),
			mCodeByteCount(0),
			mDataCount(0),
			mDataSize(0),
			mDataAlignment(0)
		{}

		uint32_t mMajorVersion;
		uint32_t mMinorVersion;
		PointerSize mPointerSize;
		size_t mListBlobIdIndex;
		size_t mFunctionBlobIdIndex;
		size_t mDataBlobIdIndex;
		size_t mStaticInitializerNameIndex;
		size_t mValue32Count;
		size_t mValue64Count;
		size_t mStringCount;
		size_t mStringByteCount;
		size_t mQualifiedNameCount;
		size_t mQualifiedNameElementCount;
		size_t mParamListSignatureCount;
		size_t mParamSignatureCount;
		size_t mFunctionCount;
		size_t mStaticInitializerCount;
		size_t mCodeByteCount;
		size_t mDataCount;
		size_t mDataSize;
		size_t mDataAlignment;
	};

	Result Validate(const void *byteCode, size_t length);
	Result Validate(InputStream &stream);
};

}

#endif
