#include "bond/stringallocator.h"
#include <assert.h>
#include <string.h>

namespace Bond
{

void StringAllocator::SetBuffer(char *buffer, size_t length)
{
	mBuffer = buffer;
	mLength = length;
	mIndex = 0;
}


char *StringAllocator::Alloc(size_t length)
{
	assert((length >= 0) && ((mIndex + length + 1) <= mLength));
	char *buffer = mBuffer + mIndex;
	mIndex += length + 1;
	return buffer;
}


char *StringAllocator::Alloc(const char *content, size_t length)
{
	char *buffer = Alloc(length);
	memcpy(buffer, content, length);
	buffer[length] = '\0';
	return buffer;
}

}
