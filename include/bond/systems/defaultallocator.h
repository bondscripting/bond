#ifndef BOND_SYSTEMS_DEFAULTALLOCATOR_H
#define BOND_SYSTEMS_DEFAULTALLOCATOR_H

#include "bond/systems/allocator.h"

namespace Bond
{

/// \brief A simple implementation of Allocator that allocates and frees memory using the new and delete
/// operators and also tracks the number of outstanding allocations.
///
/// \ingroup systems
class DefaultAllocator: public Allocator
{
public:
	DefaultAllocator(): mNumAllocations(0) {}
	virtual ~DefaultAllocator() {}

	virtual void *Allocate(size_t size) override { ++mNumAllocations; return static_cast<void *>(new char[size]); }
	virtual void *AllocateAligned(size_t size, size_t align) override;

	virtual void Free(void *buffer) override { mNumAllocations -= (buffer == nullptr) ? 0 : 1; delete [] static_cast<char *>(buffer); }
	virtual void FreeAligned(void *buffer) override;

	/// \brief Returns the number of outstanding allocations.
	int GetNumAllocations() const { return mNumAllocations; };

private:
	int mNumAllocations;
};

}

#endif
