#ifndef BOND_VM_CODESEGMENT_H
#define BOND_VM_CODESEGMENT_H

#include "bond/conf.h"
#include "bond/api/nativebinding.h"
#include "bond/stl/stringview.h"
#include "bond/types/qualifiedname.h"
#include "bond/types/value.h"

namespace Bond
{

class VM;

/// \addtogroup vm
/// @{

/// \brief The constant tables of a single CBO, which are referenced by index from the bytecode of
/// its functions.
struct ConstantTable
{
	/// \brief The table of 32-bit constants.
	const Value32 *mValue32Table;

	/// \brief The table of 64-bit constants.
	const Value64 *mValue64Table;

	/// \brief The table of string constants.
	const StringView *mStringTable;

	/// \brief The table of qualified names of the symbols referenced by the CBO.
	const QualifiedName *mQualifiedNameTable;
};


/// \brief Signature metadata for a single function parameter.
struct ParamSignature
{
	/// \brief Constructs a ParamSignature object.
	/// \param framePointerOffset Offset in bytes from the frame pointer to the parameter value.
	/// \param size Size of the parameter in bytes.
	/// \param type The SignatureType of the parameter.
	explicit ParamSignature(int32_t framePointerOffset = 0, uint32_t size = 0, uint32_t type = 0):
		mFramePointerOffset(framePointerOffset),
		mSize(size),
		mType(type)
	{}

	/// \brief Offset in bytes from the frame pointer to the parameter value.
	int32_t mFramePointerOffset;

	/// \brief Size of the parameter in bytes.
	uint32_t mSize:28;

	/// \brief The SignatureType of the parameter.
	uint32_t mType:4;
};


/// \brief Signature metadata for a function's parameter list.
struct ParamListSignature
{
	/// \brief Constructs a ParamListSignature object.
	/// \param paramSignatures Pointer to the parameter signature array. It is not copied.
	/// \param paramCount Number of entries in the parameter signature array.
	explicit ParamListSignature(const ParamSignature *paramSignatures = nullptr, uint32_t paramCount = 0):
		mParamSignatures(paramSignatures),
		mParamCount(paramCount)
	{}

	/// \brief Pointer to the parameter signature array, in declaration order.
	const ParamSignature *mParamSignatures;

	/// \brief Number of entries in the parameter signature array.
	uint32_t mParamCount;
};


/// \brief Signature metadata for a function return value.
struct ReturnSignature
{
	/// \brief Constructs a ReturnSignature object.
	/// \param size Size of the return value in bytes.
	/// \param type The SignatureType of the return value.
	explicit ReturnSignature(uint32_t size = 0, uint32_t type = 0):
		mSize(size),
		mType(type)
	{}

	/// \brief Size of the return value in bytes.
	uint32_t mSize:28;

	/// \brief The SignatureType of the return value.
	uint32_t mType:4;
};


/// \brief Metadata and executable code or native binding of a function loaded from a CBO.
struct Function
{
	/// \brief Returns whether this function is native, i.e. implemented in C++ rather than Bond
	/// bytecode.
	/// \returns \c true if the function is native, otherwise \c false.
	bool IsNative() const { return mCodeSize == 0; }

	/// \brief The fully qualified name of the function.
	QualifiedName mName;

	/// \brief The signature of the return value.
	ReturnSignature mReturnSignature;

	/// \brief The signature of the parameter list.
	ParamListSignature mParamListSignature;

	/// \brief The constant tables of the CBO in which the function is defined.
	const ConstantTable *mConstantTable;

	union
	{
		/// \brief The bytecode of a function that is not native.
		const uint8_t *mCode;

		/// \brief The C++ implementation of a native function.
		NativeFunction mNativeFunction;
	};

	/// \brief The size of the bytecode in bytes, which is 0 for a native function.
	uint32_t mCodeSize;

	/// \brief The size in bytes of the arguments laid out in the function's frame.
	uint32_t mArgSize;

	/// \brief The size in bytes of the arguments packed into consecutive slots by the caller.
	uint32_t mPackedArgSize;

	/// \brief The size in bytes of the function's local variables.
	uint32_t mLocalSize;

	/// \brief The maximum size in bytes of the function's operand stack.
	uint32_t mStackSize;

	/// \brief The alignment in bytes of the function's frame pointer.
	uint32_t mFramePointerAlignment;
};


/// \brief A named global variable loaded from a CBO.
struct DataEntry
{
	/// \brief The fully qualified name of the global variable.
	QualifiedName mName;

	/// \brief Pointer to the storage of the global variable.
	void *mData;
};


/// \brief Collection of functions, static initializers and data loaded from CBO files.
///
/// A CodeSegment is produced by CboLoader and consumed by the VM. It supports looking up
/// functions and data entries by qualified name, and calling static initializers before
/// an application's entry point is invoked.
class BOND_API CodeSegment
{
public:
	/// \brief Constructs a CodeSegment object from prebuilt tables.
	///
	/// The tables are not copied, so they must remain valid for the lifetime of the CodeSegment.
	/// Lookups by name use a binary search, so the function and data tables must be sorted by
	/// name in the order defined by QualifiedName::Compare.
	/// \param functionTable Array of callable functions, sorted by name.
	/// \param functionCount Number of entries in \p functionTable.
	/// \param staticInitializerTable Array of static initializer functions.
	/// \param staticInitializerCount Number of entries in \p staticInitializerTable.
	/// \param dataTable Array of named data entries, sorted by name.
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
	/// \param qualifiedName Fully qualified function name with its elements separated by the '.'
	///        character, e.g. "Namespace.Function".
	/// \returns Pointer to the function, or \c nullptr if it is not found.
	const Function *GetFunction(const char *qualifiedName) const;

	/// \brief Gets the index of a function by qualified name.
	/// \param qualifiedName Fully qualified function name.
	/// \returns Function index, or -1 if it is not found.
	int32_t GetFunctionIndex(const QualifiedName &qualifiedName) const;

	/// \brief Gets the index of a function by qualified name string.
	/// \param qualifiedName Fully qualified function name with its elements separated by the '.'
	///        character.
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
	/// \param qualifiedName Fully qualified data entry name with its elements separated by the '.'
	///        character.
	/// \returns Pointer to the data entry, or \c nullptr if it is not found.
	const DataEntry *GetDataEntry(const char *qualifiedName) const;

	/// \brief Gets the index of a data entry by qualified name.
	/// \param qualifiedName Fully qualified data entry name.
	/// \returns Data entry index, or -1 if it is not found.
	int32_t GetDataEntryIndex(const QualifiedName &qualifiedName) const;

	/// \brief Gets the index of a data entry by qualified name string.
	/// \param qualifiedName Fully qualified data entry name with its elements separated by the '.'
	///        character.
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
	///
	/// Each CBO contributes at most one static initializer. They are called in the order the CBOs
	/// were added to the CboLoader. Global data is zero-filled when the code segment is loaded, so
	/// a global read before its initializer has run is zero.
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
