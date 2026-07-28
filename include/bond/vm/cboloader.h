#ifndef BOND_VM_CBOLOADER_H
#define BOND_VM_CBOLOADER_H

#include "bond/api/nativebinding.h"
#include "bond/io/memoryinputstream.h"
#include "bond/stl/list.h"
#include "bond/stl/vector.h"
#include "bond/types/types.h"

namespace Bond
{

class QualifiedName;
class CodeSegment;
struct CboLoaderResources;
struct Function;

/// \addtogroup vm
/// @{

/// \brief An owning pointer to a dynamically allocated CodeSegment.
typedef Allocator::AlignedHandle<const CodeSegment> CodeSegmentHandle;

/// \brief Loads CBO modules and resolves their native function bindings.
///
/// A CboLoader aggregates one or more CBO inputs, combines their contents into a single
/// CodeSegment, and binds native functions from supplied NativeBindingCollections.
class CboLoader
{
public:
	/// \brief Constructs a CboLoader using the same allocator for temporary and persistent data.
	/// \param allocator The allocator used while loading and for the resulting CodeSegment.
	explicit CboLoader(Allocator &allocator):
		mNativeBindingList(NativeBindingList::allocator_type(&allocator)),
		mOwnedInputStreamList(OwnedInputStreamList::allocator_type(&allocator)),
		mInputStreamList(InputStreamList::allocator_type(&allocator)),
		mTempAllocator(allocator),
		mPermAllocator(allocator)
	{}

	/// \brief Constructs a CboLoader with separate allocators for temporary and persistent data.
	/// \param tempAllocator Allocator used for temporary state during loading.
	/// \param permAllocator Allocator used for data that survives in the resulting CodeSegment.
	CboLoader(Allocator &tempAllocator, Allocator &permAllocator):
		mNativeBindingList(NativeBindingList::allocator_type(&tempAllocator)),
		mOwnedInputStreamList(OwnedInputStreamList::allocator_type(&tempAllocator)),
		mInputStreamList(InputStreamList::allocator_type(&tempAllocator)),
		mTempAllocator(tempAllocator),
		mPermAllocator(permAllocator)
	{}

	CboLoader(const CboLoader &other) = delete;
	CboLoader &operator=(const CboLoader &other) = delete;

	/// \brief Adds a collection of native function bindings used to resolve native declarations.
	/// \param nativeBinding Native binding collection to search when binding native functions.
	///        The collection is not copied and must remain valid until Load() returns.
	void AddNativeBinding(const NativeBindingCollection &nativeBinding) { mNativeBindingList.push_back(&nativeBinding); }

	/// \brief Adds an in-memory CBO file.
	/// \param byteCode Pointer to the CBO byte stream.
	/// \param length Number of bytes in the CBO stream.
	void AddCboFile(const void *byteCode, size_t length)
	{
		mOwnedInputStreamList.emplace_back(byteCode, Stream::pos_t(length));
		AddCboFile(mOwnedInputStreamList.back());
	}

	/// \brief Adds a CBO file stream.
	/// \param cboStream Input stream containing CBO data. The stream is not copied and must
	///        remain valid until Load() returns.
	void AddCboFile(InputStream &cboStream) { mInputStreamList.push_back(&cboStream); }

	/// \brief Loads all added CBO files and returns the merged code segment.
	/// \returns A CodeSegmentHandle owning the loaded CodeSegment.
	CodeSegmentHandle Load();

private:
	typedef Vector<const NativeBindingCollection *> NativeBindingList;
	typedef List<MemoryInputStream> OwnedInputStreamList;
	typedef Vector<InputStream *> InputStreamList;

	void BindNativeFunction(const NativeFunctionBinding &binding, const CodeSegment &codeSegment);
	void ProcessFunction(Function &function, const CodeSegment &codeSegment);

	void FunctionIsNotNative(const Function &function) const;
	void FunctionIsNotBound(const Function &function) const;
	void FunctionIsDefinedMultipleTimes(const QualifiedName &name) const;
	void DataIsDefinedMultipleTimes(const QualifiedName &name) const;
	void UnresolvedQualifiedName(const QualifiedName &name) const;
	void UnresolvedQualifiedName(const char *name) const;

	NativeBindingList mNativeBindingList;
	OwnedInputStreamList mOwnedInputStreamList;
	InputStreamList mInputStreamList;
	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
};

/// @}

}

#endif
