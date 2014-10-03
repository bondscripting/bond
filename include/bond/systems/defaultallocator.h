#ifndef BOND_SYSTEMS_DEFAULTALLOCATOR_H
#define BOND_SYSTEMS_DEFAULTALLOCATOR_H

#include "bond/systems/allocator.h"

namespace Bond
{

class DefaultAllocator: public Allocator
{
public:
	DefaultAllocator(): mNumAllocations(0) {}
	virtual ~DefaultAllocator() {}

	virtual void *Allocate(size_t size) override { ++mNumAllocations; return static_cast<void *>(new char[size]); }
	virtual void *AllocateAligned(size_t size, size_t align) override;

	virtual void Free(void *buffer) override { mNumAllocations -= (buffer == nullptr) ? 0 : 1; delete [] static_cast<char *>(buffer); }
	virtual void FreeAligned(void *buffer) override;

	int GetNumAllocations() const { return mNumAllocations; };

private:
	int mNumAllocations;
};

}

#endif
