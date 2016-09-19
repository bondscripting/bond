#include "bond/api/libmemory.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"
#include <cstring>

namespace Bond
{

typedef WrapperCollectable<Allocator::AlignedHandle<uint8_t> > AlignedCollectable;

void GetCollector(Bond::StackFrame &frame)
{
	Collector *collector = &frame.GetVM().GetCallerStackFrame().GetCollector();
	frame.SetReturnValue(collector);
}


void Allocate(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	const size_t size = size_t(frame.GetArg<uint64_t>(0));
	void *ptr = allocator.Allocate(size);
	frame.SetReturnValue(ptr);
}


void *AllocateCollected(Allocator &allocator, Collector &collector, size_t size)
{
	const size_t collectableSize = sizeof(Collectable);
	auto memHandle = allocator.AllocOwned<uint8_t>(collectableSize + size);
	Collectable *collectable = new (memHandle.get()) Collectable();
	void *ptr = memHandle.get() + collectableSize;
	memHandle.release();
	collector.Register(CollectableHandle(allocator, collectable));
	return ptr;
}


void AllocateCollected(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const size_t size = size_t(frame.GetArg<uint64_t>(0));
	void *ptr = AllocateCollected(allocator, collector, size);
	frame.SetReturnValue(ptr);
}


void AllocateWithCollector(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	Collector &collector = *frame.GetArg<Collector *>(0);
	const size_t size = size_t(frame.GetArg<uint64_t>(1));
	void *ptr = AllocateCollected(allocator, collector, size);
	frame.SetReturnValue(ptr);
}


void AllocateAligned(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	const size_t size = size_t(frame.GetArg<uint64_t>(0));
	const size_t alignment = size_t(frame.GetArg<uint32_t>(1));
	void *ptr = allocator.AllocateAligned(size, alignment);
	frame.SetReturnValue(ptr);
}


void *AllocateAlignedCollected(Allocator &allocator, Collector &collector, size_t size, size_t alignment)
{
	auto collectableHandle = allocator.AllocOwnedObject<AlignedCollectable>(
		allocator.AllocOwnedAligned<uint8_t>(size, alignment));
	void *ptr = collectableHandle->GetContent().get();
	collector.Register(move(collectableHandle));
	return ptr;
}


void AllocateAlignedCollected(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const size_t size = size_t(frame.GetArg<uint64_t>(0));
	const size_t alignment = size_t(frame.GetArg<uint32_t>(1));
	void *ptr = AllocateAlignedCollected(allocator, collector, size, alignment);
	frame.SetReturnValue(ptr);
}


void AllocateAlignedWithCollector(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	Collector &collector = *frame.GetArg<Collector *>(0);
	const size_t size = size_t(frame.GetArg<uint64_t>(1));
	const size_t alignment = size_t(frame.GetArg<uint32_t>(2));
	void *ptr = AllocateAlignedCollected(allocator, collector, size, alignment);
	frame.SetReturnValue(ptr);
}


void Free(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	void *ptr = frame.GetArg<void *>(0);
	allocator.Free(ptr);
}


void FreeAligned(Bond::StackFrame &frame)
{
	Allocator &allocator = frame.GetVM().GetAllocator();
	void *ptr = frame.GetArg<void *>(0);
	allocator.FreeAligned(ptr);
}


void Memcpy(Bond::StackFrame &frame)
{
	void *dest = frame.GetArg<void *>(0);
	void *src = frame.GetArg<void *>(1);
	size_t size = size_t(frame.GetArg<uint64_t>(2));
	memcpy(dest, src, size);
}

}
