#include "bond/api/libmemory.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"

namespace Bond
{

void Allocate(Bond::CalleeStackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	const size_t size = size_t(frame.GetArg<bu32_t>(0));
	void *ptr = allocator.Allocate(size);
	frame.SetReturnValue(ptr);
}


void AllocateAligned(Bond::CalleeStackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	const size_t size = size_t(frame.GetArg<bu32_t>(0));
	const size_t alignment = size_t(frame.GetArg<bu32_t>(1));
	void *ptr = allocator.Allocate(size);
	frame.SetReturnValue(ptr);
}


void Free(Bond::CalleeStackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	void *ptr = frame.GetArg<void *>(0);
	allocator.Free(ptr);
}


void FreeAligned(Bond::CalleeStackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	void *ptr = frame.GetArg<void *>(0);
	allocator.FreeAligned(ptr);
}

}
