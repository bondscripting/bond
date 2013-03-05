#include "bond/systems/defaultallocator.h"
#include "bond/systems/math.h"

namespace Bond
{

void *DefaultAllocator::AllocateAligned(size_t size, size_t align)
{
	const size_t allocAlign = Max(RoundUpToPowerOfTwo32(align), sizeof(void *));
	const size_t allocSize = size + sizeof(void *) + allocAlign - 1;
	char *mem = Alloc<char>(allocSize);
	void *buffer = mem;

	if (mem != NULL)
	{
		buffer = AlignPointerUp(mem + sizeof(void*), allocAlign);
		reinterpret_cast<void **>(buffer)[-1] = mem;
	}

	return buffer;
}


void DefaultAllocator::FreeAligned(void *buffer)
{
	if (buffer != NULL)
	{
		Free(reinterpret_cast<void **>(buffer)[-1]);
	}
}

}
