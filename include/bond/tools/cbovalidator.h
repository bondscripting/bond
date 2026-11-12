#ifndef BOND_TOOLS_CBOVALIDATOR_H
#define BOND_TOOLS_CBOVALIDATOR_H

#include "bond/conf.h"
#include "bond/types/types.h"

namespace Bond
{

class InputStream;

/// \brief A validator for Bond CBO bytecode streams.
///
/// The CboValidator verifies the structure of serialized CBO data and reports summary
/// information about the validated contents, such as the number of entries in each table, which
/// CboLoader uses to size its allocations. A Bond::Exception is thrown if the CBO data is
/// invalid, incomplete or has an unsupported format version.
///
/// \sa CboLoader, InputStream
/// \ingroup tools
class BOND_API CboValidator
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

		/// \brief The major version of the CBO file format.
		uint32_t mMajorVersion;

		/// \brief The minor version of the CBO file format.
		uint32_t mMinorVersion;

		/// \brief The pointer size for which the CBO was compiled.
		PointerSize mPointerSize;

		/// \brief Index of the "List" blob identifier in the string table, or BOND_SIZE_MAX.
		size_t mListBlobIdIndex;

		/// \brief Index of the "Func" blob identifier in the string table, or BOND_SIZE_MAX.
		size_t mFunctionBlobIdIndex;

		/// \brief Index of the "Data" blob identifier in the string table, or BOND_SIZE_MAX.
		size_t mDataBlobIdIndex;

		/// \brief Index of the empty qualified name, which names the static initializer, in the
		/// qualified name table, or BOND_SIZE_MAX.
		size_t mStaticInitializerNameIndex;

		/// \brief Number of entries in the 32-bit constant table.
		size_t mValue32Count;

		/// \brief Number of entries in the 64-bit constant table.
		size_t mValue64Count;

		/// \brief Number of entries in the string table.
		size_t mStringCount;

		/// \brief Total number of bytes in the strings of the string table, excluding null
		/// terminators.
		size_t mStringByteCount;

		/// \brief Number of entries in the qualified name table.
		size_t mQualifiedNameCount;

		/// \brief Total number of elements in the qualified names of the qualified name table,
		/// excluding terminators.
		size_t mQualifiedNameElementCount;

		/// \brief Number of parameter list signatures.
		size_t mParamListSignatureCount;

		/// \brief Total number of parameter signatures.
		size_t mParamSignatureCount;

		/// \brief Number of functions, excluding the static initializer.
		size_t mFunctionCount;

		/// \brief Number of static initializers.
		size_t mStaticInitializerCount;

		/// \brief Total number of bytes of bytecode.
		size_t mCodeByteCount;

		/// \brief Number of global variables.
		size_t mDataCount;

		/// \brief Total size in bytes of the global variables, including padding.
		size_t mDataSize;

		/// \brief The largest alignment of the global variables, which is at least BOND_SLOT_SIZE.
		size_t mDataAlignment;
	};

	/// \brief Validates CBO data from an in-memory byte buffer.
	/// \param byteCode Pointer to the beginning of the byte buffer.
	/// \param length The number of bytes in the buffer.
	/// \returns A Result containing decoded metadata and aggregate counts.
	Result Validate(const void *byteCode, size_t length);

	/// \brief Validates CBO data from an input stream, starting at its current position.
	/// \param stream Input stream containing CBO data. Its position is left after the validated
	///        data, so it must be reset before the data is read again.
	/// \returns A Result containing decoded metadata and aggregate counts.
	Result Validate(InputStream &stream);
};

}

#endif
