#ifndef BOND_CBOVALIDATOR_H
#define BOND_CBOVALIDATOR_H

#include "bond/types.h"

namespace Bond
{

class TextWriter;

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
			mValue32Count(0),
			mValue64Count(0),
			mStringCount(0),
			mStringByteCount(0),
			mQualifiedIdCount(0),
			mQualifiedIdElementCount(0),
			mParamListSignatureCount(0),
			mParamSignatureCount(0),
			mFunctionCount(0),
			mCodeByteCount(0)
		{}

		int mMajorVersion;
		int mMinorVersion;
		PointerSize mPointerSize;
		size_t mListBlobIdIndex;
		size_t mFunctionBlobIdIndex;
		size_t mValue32Count;
		size_t mValue64Count;
		size_t mStringCount;
		size_t mStringByteCount;
		size_t mQualifiedIdCount;
		size_t mQualifiedIdElementCount;
		size_t mParamListSignatureCount;
		size_t mParamSignatureCount;
		size_t mFunctionCount;
		size_t mCodeByteCount;
	};

	Result Validate(const void *byteCode, size_t length);
};

}

#endif
