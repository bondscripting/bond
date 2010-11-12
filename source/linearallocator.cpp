#include "bond/linearallocator.h"
#include <assert.h>

namespace Bond
{

void LinearAllocator::SetBuffer(char *buffer, int length)
{
	mBuffer = buffer;
	mLength = length;
	mIndex = 0;
}


void *LinearAllocator::Allocate(int size)
{
	assert((size >= 0) && ((mIndex + size) <= mLength));
	char *buffer = mBuffer + mIndex;
	mIndex += size;
	return static_cast<void *>(buffer);
}

}
