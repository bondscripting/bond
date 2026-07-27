#ifndef BOND_TOOLS_CBOVALIDATOR_H
#define BOND_TOOLS_CBOVALIDATOR_H

#include "bond/types/types.h"

namespace Bond
{

class InputStream;

/// \brief A validator for Bond CBO bytecode streams.
///
/// The CboValidator verifies the structure of serialized CBO data and reports summary
/// information about the validated contents.
///
/// \sa InputStream
/// \ingroup tools
class CboValidator
{
public:

/// \brief Summary information produced during CBO validation.
	struct Result
	{
		/// \brief Constructs an empty validation result with default values.
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

	/// \brief Validates CBO data from an in-memory byte buffer.
	/// \param byteCode Pointer to the beginning of the byte buffer.
	/// \param length The number of bytes in the buffer.
	/// \returns A Result containing decoded metadata and aggregate counts.
	Result Validate(const void *byteCode, size_t length);

	/// \brief Validates CBO data from an input stream.
	/// \param stream Input stream containing CBO data.
	/// \returns A Result containing decoded metadata and aggregate counts.
	Result Validate(InputStream &stream);
};

}

#endif
