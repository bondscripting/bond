#ifndef BOND_LINEARALLOCATOR_H
#define BOND_LINEARALLOCATOR_H

#include "bond/allocator.h"

namespace Bond
{

class LinearAllocator: public Allocator
{
public:
	LinearAllocator():
		mBuffer(0),
		mLength(0),
		mIndex(0)
	{}

	LinearAllocator(char *buffer, int length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{}

	virtual ~LinearAllocator() {}

	void SetBuffer(char *buffer, int length);

	virtual void *Allocate(int size);
	virtual void Free(void *buffer) {}

private:
	char *mBuffer;
	int mLength;
	int mIndex;
};

}

#endif
