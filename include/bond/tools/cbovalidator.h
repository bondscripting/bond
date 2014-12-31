#ifndef BOND_TOOLS_CBOVALIDATOR_H
#define BOND_TOOLS_CBOVALIDATOR_H

#include "bond/types/types.h"

namespace Bond
{

class CboValidator
{
public:

	struct Result
	{
		Result():
			mMajorVersion(0),
			mMinorVersion(0),
			mPointerSize(POINTER_32BIT),
			mListBlobIdIndex(0),
			mFunctionBlobIdIndex(0),
			mDataBlobIdIndex(0),
			mValue32Count(0),
			mValue64Count(0),
			mStringCount(0),
			mStringByteCount(0),
			mQualifiedIdCount(0),
			mQualifiedIdElementCount(0),
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
		size_t mValue32Count;
		size_t mValue64Count;
		size_t mStringCount;
		size_t mStringByteCount;
		size_t mQualifiedIdCount;
		size_t mQualifiedIdElementCount;
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
};

}

#endif
