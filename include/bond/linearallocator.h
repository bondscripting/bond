#ifndef BOND_LINEARALLOCATOR_H
#define BOND_LINEARALLOCATOR_H

#include "bond/allocator.h"

namespace Bond
{

class LinearAllocator: public Allocator
{
public:
	LinearAllocator():
		mBuffer(NULL),
		mLength(0),
		mIndex(0)
	{}

	LinearAllocator(char *buffer, size_t length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{}

	virtual ~LinearAllocator() {}

	void SetBuffer(char *buffer, size_t length);

	virtual void *Allocate(size_t size);
	virtual void Free(void *buffer) {}

private:
	char *mBuffer;
	size_t mLength;
	size_t mIndex;
};

}

#endif
