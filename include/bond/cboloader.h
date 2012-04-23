#ifndef BOND_CBOLOADER_H
#define BOND_CBOLOADER_H

#include "bond/types.h"

namespace Bond
{

class Allocator;
class CodeSegment;
struct FileData;
struct Function;

class CboLoader
{
public:
	CboLoader(Allocator &allocator):
		mTempAllocator(allocator),
		mPermAllocator(allocator)
	{}

	CboLoader(Allocator &tempAllocator, Allocator &permAllocator):
		mTempAllocator(tempAllocator),
		mPermAllocator(permAllocator)
	{}

	const CodeSegment *Load(const FileData *cboFiles, size_t numFiles);
	void Dispose(const CodeSegment *codeSegment);

private:
	struct FunctionHashComparator
	{
		bool operator()(const Function &a, const Function &b) const;
	};

	void ProcessFunction(Function &function, const CodeSegment &codeSegment) const;

	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
};

}

#endif
