#ifndef BOND_ALLOCATOR_H
#define BOND_ALLOCATOR_H

#include "bond/conf.h"

namespace Bond
{

class Allocator
{
public:
	virtual ~Allocator() {}
	virtual void *Allocate(size_t size) = 0;
	virtual void *AllocateAligned(size_t size, size_t align) = 0;
	virtual void Free(void *buffer) = 0;
	virtual void FreeAligned(void *buffer) = 0;

	template <typename T> T *Alloc() { return static_cast<T *>(Allocate(sizeof(T))); }
	template <typename T> T *Alloc(size_t numElements) { return static_cast<T *>(Allocate(sizeof(T) * numElements)); }

	template <typename T> T *AllocAligned(size_t align) { return static_cast<T *>(AllocateAligned(sizeof(T), align)); }
	template <typename T> T *AllocAligned(size_t numElements, size_t align) { return static_cast<T *>(AllocateAligned(sizeof(T) * numElements, align)); }
};

}

#endif
