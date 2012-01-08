#ifndef BOND_DEFAULTALLOCATOR_H
#define BOND_DEFAULTALLOCATOR_H

#include "bond/allocator.h"

namespace Bond
{

class DefaultAllocator: public Allocator
{
public:
	DefaultAllocator(): mNumAllocations(0) {}
	virtual ~DefaultAllocator() {}

	virtual void *Allocate(size_t size) { ++mNumAllocations; return static_cast<void *>(new char[size]); }
	virtual void Free(void *buffer) { mNumAllocations -= (buffer == NULL) ? 0 : 1; delete [] static_cast<char *>(buffer); }

	int GetNumAllocations() const { return mNumAllocations; };

private:
	int mNumAllocations;
};

}

#endif
