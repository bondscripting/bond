#ifndef BOND_STRINGALLOCATOR_H
#define BOND_STRINGALLOCATOR_H

#include "bond/conf.h"

namespace Bond
{

class StringAllocator
{
public:
	StringAllocator():
		mBuffer(NULL),
		mLength(0),
		mIndex(0)
	{}

	StringAllocator(char *buffer, size_t length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{}

	void SetBuffer(char *buffer, size_t length);
	char *Alloc(size_t length);
	char *Alloc(const char *content, size_t length);

private:
	StringAllocator(const StringAllocator &other);
	StringAllocator &operator=(const StringAllocator &other);

	char *mBuffer;
	size_t mLength;
	size_t mIndex;
};

}

#endif
