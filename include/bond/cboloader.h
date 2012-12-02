#ifndef BOND_CBOLOADER_H
#define BOND_CBOLOADER_H

#include "bond/cbovalidator.h"
#include "bond/types.h"

namespace Bond
{

class Allocator;
class CodeSegment;
class TextWriter;
struct FileData;
struct Function;

class CboLoader
{
public:
	enum Status
	{
		LOAD_VALID,
		LOAD_INVALID_CBO,
		LOAD_UNRESOLVED_HASH,
		LOAD_HASH_COLLISION
	};

	CboLoader(Allocator &allocator):
		mTempAllocator(allocator),
		mPermAllocator(allocator),
		mStatus(LOAD_VALID),
		mStatusArg(BOND_UINT_MAX)
	{}

	CboLoader(Allocator &tempAllocator, Allocator &permAllocator):
		mTempAllocator(tempAllocator),
		mPermAllocator(permAllocator),
		mStatus(LOAD_VALID),
		mStatusArg(BOND_UINT_MAX)
	{}

	const CodeSegment *Load(const FileData *cboFiles, size_t numFiles);
	void Dispose(const CodeSegment *codeSegment);

	bool HasError() const { return mStatus != LOAD_VALID; }
	Status GetStatus() const { return mStatus; }
	bu32_t GetStatusArg() const { return mStatusArg; }

	void WriteStatus(TextWriter& writer);

private:
	struct FunctionHashComparator
	{
		bool operator()(const Function &a, const Function &b) const;
	};

	void ProcessFunction(Function &function, const CodeSegment &codeSegment);

	void InvalidCbo(CboValidator::Status status);
	void UnresolvedHash(bu32_t hash);
	void HashCollision(bu32_t hash);

	Allocator &mTempAllocator;
	Allocator &mPermAllocator;
	Status mStatus;
	bu32_t mStatusArg;
};

}

#endif
