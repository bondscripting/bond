#ifndef BOND_PRIVATE_STLALLOCATOR_H
#define BOND_PRIVATE_STLALLOCATOR_H

#include "bond/stl/utility.h"

namespace Bond
{

template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator(Allocator *allocator) noexcept:
		mAllocator(allocator)
	{}

	template <typename U> StlAllocator(const StlAllocator<U> &other) noexcept:
		mAllocator(other.GetAllocator())
	{}

	value_type *allocate(size_t numElements)
	{
		return mAllocator->Alloc<T>(numElements);
	}

	void deallocate(value_type *p, size_t) noexcept
	{
		mAllocator->Free(p);
	}

	Allocator *GetAllocator() const { return mAllocator; }

private:
	Allocator *mAllocator;
};


template <typename T1, typename T2>
bool operator ==(const StlAllocator<T1> &a, const StlAllocator<T2> &b)
{
	return a.GetAllocator() == b.GetAllocator();
}


template <typename T1, typename T2>
bool operator !=(const StlAllocator<T1> &a, const StlAllocator<T2> &b)
{
	return a.GetAllocator() != b.GetAllocator();
}

}

#endif
