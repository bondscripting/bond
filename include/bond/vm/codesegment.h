#ifndef BOND_VM_CODESEGMENT_H
#define BOND_VM_CODESEGMENT_H

#include "bond/api/nativebinding.h"
#include "bond/stl/stringview.h"
#include "bond/types/qualifiedname.h"
#include "bond/types/value.h"

namespace Bond
{

class VM;

/// \addtogroup vm
/// @{

/// \brief Collection of per-CBO Constant tables.
struct ConstantTable
{
	const Value32 *mValue32Table;
	const Value64 *mValue64Table;
	const StringView *mStringTable;
	const QualifiedName *mQualifiedNameTable;
};


/// \brief Signature metadata for a single function parameter.
struct ParamSignature
{
	/// \brief Constructs a ParamSignature.
	/// \param framePointerOffset Offset in bytes from the frame pointer to the parameter value.
	/// \param size Size of the parameter in bytes.
	/// \param type Encoded Bond type identifier for the parameter.
	explicit ParamSignature(int32_t framePointerOffset = 0, uint32_t size = 0, uint32_t type = 0):
		mFramePointerOffset(framePointerOffset),
		mSize(size),
		mType(type)
	{}

	int32_t mFramePointerOffset;
	uint32_t mSize:28;
	uint32_t mType:4;
};


/// \brief Signature metadata for a function's parameter list.
struct ParamListSignature
{
	/// \brief Constructs a ParamListSignature.
	/// \param paramSignatures Pointer to the parameter signature array.
	/// \param paramCount Number of entries in the parameter signature array.
	explicit ParamListSignature(const ParamSignature *paramSignatures = nullptr, uint32_t paramCount = 0):
		mParamSignatures(paramSignatures),
		mParamCount(paramCount)
	{}

	const ParamSignature *mParamSignatures;
	uint32_t mParamCount;
};


/// \brief Signature metadata for a function return value.
struct ReturnSignature
{
	/// \brief Constructs a ReturnSignature.
	/// \param size Size of the return value in bytes.
	/// \param type Encoded Bond type identifier for the return value.
	explicit ReturnSignature(uint32_t size = 0, uint32_t type = 0):
		mSize(size),
		mType(type)
	{}

	uint32_t mSize:28;
	uint32_t mType:4;
};


/// \brief Function metadata and executable payload loaded from CBO.
struct Function
{
	/// \brief Indicates whether this function is native.
	/// \returns \c true if the function is native, otherwise \c false.
	bool IsNative() const { return mCodeSize == 0; }

	QualifiedName mName;
	ReturnSignature mReturnSignature;
	ParamListSignature mParamListSignature;
	const ConstantTable *mConstantTable;

	union
	{
		const uint8_t *mCode;
		NativeFunction mNativeFunction;
	};

	uint32_t mCodeSize;
	uint32_t mArgSize;
	uint32_t mPackedArgSize;
	uint32_t mLocalSize;
	uint32_t mStackSize;
	uint32_t mFramePointerAlignment;
	bool mUnpackArguments;
};


/// \brief A named data slot loaded from CBO global data tables.
struct DataEntry
{
	QualifiedName mName;
	void *mData;
};


/// \brief Collection of functions, static initializers and data loaded from CBO files.
///
/// A CodeSegment is produced by CboLoader and consumed by the VM. It supports looking up
/// functions and data entries by qualified name, and calling static initializers before
/// an application's entry point is invoked.
class CodeSegment
{
public:
	/// \brief Constructs a CodeSegment from prebuilt tables.
	/// \param functionTable Array of callable functions.
	/// \param functionCount Number of entries in \p functionTable.
	/// \param staticInitializerTable Array of static initializer functions.
	/// \param staticInitializerCount Number of entries in \p staticInitializerTable.
	/// \param dataTable Array of named data entries.
	/// \param dataCount Number of entries in \p dataTable.
	CodeSegment(
			const Function *functionTable,
			size_t functionCount,
			const Function *staticInitializerTable,
			size_t staticInitializerCount,
			const DataEntry *dataTable,
			size_t dataCount):
		mFunctionTable(functionTable),
		mFunctionCount(functionCount),
		mStaticInitializerTable(staticInitializerTable),
		mStaticInitializerCount(staticInitializerCount),
		mDataTable(dataTable),
		mDataCount(dataCount)
	{}

	/// \brief Finds a function by qualified name.
	/// \param qualifiedName Fully qualified function name.
	/// \returns Pointer to the function, or \c nullptr if it is not found.
	const Function *GetFunction(const QualifiedName &qualifiedName) const;

	/// \brief Finds a function by qualified name string.
	/// \param qualifiedName Fully qualified function name string.
	/// \returns Pointer to the function, or \c nullptr if it is not found.
	const Function *GetFunction(const char *qualifiedName) const;

	/// \brief Gets the index of a function by qualified name.
	/// \param qualifiedName Fully qualified function name.
	/// \returns Function index, or -1 if it is not found.
	int32_t GetFunctionIndex(const QualifiedName &qualifiedName) const;

	/// \brief Gets the index of a function by qualified name string.
	/// \param qualifiedName Fully qualified function name string.
	/// \returns Function index, or -1 if it is not found.
	int32_t GetFunctionIndex(const char *qualifiedName) const;

	/// \brief Returns the function at the specified index.
	/// \param functionIndex Index of the function in the function table.
	/// \returns Reference to the function at \p functionIndex.
	const Function &GetFunctionAtIndex(uint32_t functionIndex) const { return mFunctionTable[functionIndex]; }

	/// \brief Returns the number of functions in the function table.
	/// \returns Function table size.
	size_t GetFunctionCount() const { return mFunctionCount; }

	/// \brief Returns the static initializer at the specified index.
	/// \param initializerIndex Index of the static initializer.
	/// \returns Reference to the static initializer at \p initializerIndex.
	const Function &GetStaticInitializerAtIndex(uint32_t initializerIndex) const { return mStaticInitializerTable[initializerIndex]; }

	/// \brief Returns the number of static initializers.
	/// \returns Static initializer table size.
	size_t GetStaticInitializerCount() const { return mStaticInitializerCount; }

	/// \brief Finds a data entry by qualified name.
	/// \param qualifiedName Fully qualified data entry name.
	/// \returns Pointer to the data entry, or \c nullptr if it is not found.
	const DataEntry *GetDataEntry(const QualifiedName &qualifiedName) const;

	/// \brief Finds a data entry by qualified name string.
	/// \param qualifiedName Fully qualified data entry name string.
	/// \returns Pointer to the data entry, or \c nullptr if it is not found.
	const DataEntry *GetDataEntry(const char *qualifiedName) const;

	/// \brief Gets the index of a data entry by qualified name.
	/// \param qualifiedName Fully qualified data entry name.
	/// \returns Data entry index, or -1 if it is not found.
	int32_t GetDataEntryIndex(const QualifiedName &qualifiedName) const;

	/// \brief Gets the index of a data entry by qualified name string.
	/// \param qualifiedName Fully qualified data entry name string.
	/// \returns Data entry index, or -1 if it is not found.
	int32_t GetDataEntryIndex(const char *qualifiedName) const;

	/// \brief Returns the data entry at the specified index.
	/// \param dataIndex Index of the data entry in the data table.
	/// \returns Reference to the data entry at \p dataIndex.
	const DataEntry &GetDataEntryAtIndex(uint32_t dataIndex) const { return mDataTable[dataIndex]; }

	/// \brief Returns the number of data entries in the data table.
	/// \returns Data table size.
	size_t GetDataCount() const { return mDataCount; }

	/// \brief Calls all static initializer functions in this code segment.
	/// \param vm VM instance used to invoke initializer functions.
	void CallStaticInitializers(VM &vm) const;

private:
	const Function *mFunctionTable;
	size_t mFunctionCount;
	const Function *mStaticInitializerTable;
	size_t mStaticInitializerCount;
	const DataEntry *mDataTable;
	size_t mDataCount;
};

/// @}

}

#endif
