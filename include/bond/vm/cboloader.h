#ifndef BOND_VM_CBOLOADER_H
#define BOND_VM_CBOLOADER_H

#include "bond/api/nativebinding.h"
#include "bond/stl/vector.h"
#include "bond/types/types.h"

namespace Bond
{

class QualifiedName;
class CodeSegment;
struct CboLoaderResources;
struct FileData;
struct Function;

typedef Allocator::AlignedHandle<const CodeSegment> CodeSegmentHandle;

class CboLoader
{
public:
	CboLoader(Allocator &allocator):
		mNativeBindingList(NativeBindingList::Allocator(&allocator)),
		mFileDataList(FileDataList::Allocator(&allocator)),
		mTempAllocator(allocator),
		mPermAllocator(allocator)
	{}

	CboLoader(Allocator &tempAllocator, Allocator &permAllocator):
		mNativeBindingList(NativeBindingList::Allocator(&tempAllocator)),
		mFileDataList(FileDataList::Allocator(&tempAllocator)),
		mTempAllocator(tempAllocator),
		mPermAllocator(permAllocator)
	{}

	void AddNativeBinding(const NativeBindingCollection &nativeBinding) { mNativeBindingList.push_back(&nativeBinding); }
	void AddCboFile(const FileData &cboFile) { mFileDataList.push_back(&cboFile); }

	CodeSegmentHandle Load();

	CboLoader &operator=(const CboLoader &other) = delete;

private:
	typedef Vector<const NativeBindingCollection *> NativeBindingList;
	typedef Vector<const FileData *> FileDataList;

	void BindNativeFunction(const NativeFunctionBinding &binding, const CodeSegment &codeSegment);
	void ProcessFunction(Function &function, const CodeSegment &codeSegment);

	void FunctionIsNotNative(const Function &function) const;
	void FunctionIsNotBound(const Function &function) const;
	void UnresolvedQualifiedName(const QualifiedName &name) const;
	void UnresolvedQualifiedName(const char *name) const;

	NativeBindingList::Type mNativeBindingList;
	FileDataList::Type mFileDataList;
	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
};

}

#endif
