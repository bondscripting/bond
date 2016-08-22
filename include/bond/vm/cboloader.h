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

typedef Allocator::AlignedHandle<const CodeSegment> CodeSegmentHandle;

class CboLoader
{
public:
	CboLoader(Allocator &allocator):
		mNativeBindingList(NativeBindingList::allocator_type(&allocator)),
		mOwnedInputStreamList(OwnedInputStreamList::allocator_type(&allocator)),
		mInputStreamList(InputStreamList::allocator_type(&allocator)),
		mTempAllocator(allocator),
		mPermAllocator(allocator)
	{}

	CboLoader(Allocator &tempAllocator, Allocator &permAllocator):
		mNativeBindingList(NativeBindingList::allocator_type(&tempAllocator)),
		mOwnedInputStreamList(OwnedInputStreamList::allocator_type(&tempAllocator)),
		mInputStreamList(InputStreamList::allocator_type(&tempAllocator)),
		mTempAllocator(tempAllocator),
		mPermAllocator(permAllocator)
	{}

	CboLoader(const CboLoader &other) = delete;
	CboLoader &operator=(const CboLoader &other) = delete;

	void AddNativeBinding(const NativeBindingCollection &nativeBinding) { mNativeBindingList.push_back(&nativeBinding); }

	void AddCboFile(const void *byteCode, size_t length)
	{
		mOwnedInputStreamList.emplace_back(byteCode, Stream::pos_t(length));
		AddCboFile(mOwnedInputStreamList.back());
	}

	void AddCboFile(InputStream &cboStream) { mInputStreamList.push_back(&cboStream); }

	CodeSegmentHandle Load();

private:
	typedef Vector<const NativeBindingCollection *> NativeBindingList;
	typedef List<MemoryInputStream> OwnedInputStreamList;
	typedef Vector<InputStream *> InputStreamList;

	void BindNativeFunction(const NativeFunctionBinding &binding, const CodeSegment &codeSegment);
	void ProcessFunction(Function &function, const CodeSegment &codeSegment);

	void FunctionIsNotNative(const Function &function) const;
	void FunctionIsNotBound(const Function &function) const;
	void UnresolvedQualifiedName(const QualifiedName &name) const;
	void UnresolvedQualifiedName(const char *name) const;

	NativeBindingList mNativeBindingList;
	OwnedInputStreamList mOwnedInputStreamList;
	InputStreamList mInputStreamList;
	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
};

}

#endif
