#ifndef BOND_CBOLOADER_H
#define BOND_CBOLOADER_H

#include "bond/api/nativebinding.h"
#include "bond/stl/list.h"
#include "bond/types/types.h"

namespace Bond
{

class CodeSegment;
class TextWriter;
struct CboLoaderResources;
struct FileData;
struct Function;

class CboLoader
{
public:
	typedef Allocator::AlignedHandle<const CodeSegment> Handle;

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

	Handle Load();

private:
	typedef List<const NativeBindingCollection *> NativeBindingList;
	typedef List<const FileData *> FileDataList;

	struct FunctionHashComparator
	{
		bool operator()(const Function &a, const Function &b) const;
	};

	void AddNativeFunction(CboLoaderResources &resources, const NativeFunctionBinding &binding);

	void ProcessFunction(Function &function, const CodeSegment &codeSegment);

	void UnresolvedHash(bu32_t hash) const;
	void HashCollision(bu32_t hash) const;

	NativeBindingList::Type mNativeBindingList;
	FileDataList::Type mFileDataList;
	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
};

}

#endif
