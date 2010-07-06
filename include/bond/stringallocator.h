#ifndef STRING_ALLOCATOR_H
#define STRING_ALLOCATOR_H

namespace Bond
{

class StringAllocator
{
public:
	StringAllocator():
		mBuffer(0),
		mLength(0),
		mIndex(0)
	{
	}

	StringAllocator(char *buffer, int length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{
	}

	void SetBuffer(char *buffer, int length);
	char *Alloc(int length);
	char *Alloc(const char *content, int length);

private:
	StringAllocator(const StringAllocator &other);
	StringAllocator &operator=(const StringAllocator &other);

	char *mBuffer;
	int mLength;
	int mIndex;
};

}

#endif
