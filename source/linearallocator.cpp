#include "bond/linearallocator.h"
#include <assert.h>

namespace Bond
{

void LinearAllocator::SetBuffer(char *buffer, size_t length)
{
	mBuffer = buffer;
	mLength = length;
	mIndex = 0;
}


void *LinearAllocator::Allocate(size_t size)
{
	assert((size >= 0) && ((mIndex + size) <= mLength));
	char *buffer = mBuffer + mIndex;
	mIndex += size;
	return static_cast<void *>(buffer);
}

}
