#ifndef BOND_PRIVATE_STLALLOCATOR_H
#define BOND_PRIVATE_STLALLOCATOR_H

#include "bond/systems/allocator.h"

#include <cstddef>
#include <new>
#include <type_traits>

namespace Bond
{

template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	explicit StlAllocator(Allocator *allocator) noexcept:
		mAllocator(allocator)
	{}

	template <typename U> StlAllocator(const StlAllocator<U> &other) noexcept:
		mAllocator(other.GetAllocator())
	{}

	value_type *allocate(std::size_t numElements)
	{
		if (numElements > max_size())
		{
			throw std::bad_array_new_length();
		}

		value_type *p = nullptr;

		if constexpr (UsesAlignedAllocation())
		{
			p = mAllocator->AllocAligned<T>(numElements, alignof(T));
		}
		else
		{
			p = mAllocator->Alloc<T>(numElements);
		}

		if (p == nullptr)
		{
			throw std::bad_alloc();
		}

		return p;
	}

	void deallocate(value_type *p, std::size_t) noexcept
	{
		if constexpr (UsesAlignedAllocation())
		{
			mAllocator->FreeAligned(p);
		}
		else
		{
			mAllocator->Free(p);
		}
	}

	std::size_t max_size() const noexcept
	{
		return static_cast<std::size_t>(-1) / sizeof(T);
	}

	Allocator *GetAllocator() const { return mAllocator; }

private:
	static constexpr bool UsesAlignedAllocation() noexcept
	{
		return alignof(T) > alignof(std::max_align_t);
	}

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
