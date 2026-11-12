#ifndef BOND_SYSTEMS_DEFAULTALLOCATOR_H
#define BOND_SYSTEMS_DEFAULTALLOCATOR_H

#include "bond/conf.h"
#include "bond/systems/allocator.h"

namespace Bond
{

/// \brief A simple implementation of Allocator that allocates and frees memory using the new and delete
/// operators and also tracks the number of outstanding allocations.
///
/// Allocation failures are reported by throwing std::bad_alloc.
///
/// \ingroup systems
class BOND_API DefaultAllocator: public Allocator
{
public:
	/// \brief Constructs a DefaultAllocator object with no outstanding allocations.
	DefaultAllocator(): mNumAllocations(0) {}

	/// \brief Destroys the DefaultAllocator. Outstanding allocations are not freed.
	virtual ~DefaultAllocator() {}

	/// \brief Allocates a block of memory of the given size with the new operator.
	/// \param size The size of the block in bytes.
	virtual void *Allocate(size_t size) override { ++mNumAllocations; return static_cast<void *>(new char[size]); }

	/// \brief Allocates a block of memory with the given size and alignment with the new operator.
	/// \param size The size of the block in bytes.
	/// \param align The alignment of the block in bytes. It is rounded up to a power of two that is
	///        at least the size of a pointer.
	virtual void *AllocateAligned(size_t size, size_t align) override;

	/// \brief Frees memory allocated by Allocate with the delete operator.
	/// \param buffer Pointer to the memory to be freed, or null.
	virtual void Free(void *buffer) override { mNumAllocations -= (buffer == nullptr) ? 0 : 1; delete [] static_cast<char *>(buffer); }

	/// \brief Frees memory allocated by AllocateAligned with the delete operator.
	/// \param buffer Pointer to the memory to be freed, or null.
	virtual void FreeAligned(void *buffer) override;

	/// \brief Returns the number of outstanding allocations.
	int GetNumAllocations() const { return mNumAllocations; };

private:
	int mNumAllocations;
};

}

#endif
