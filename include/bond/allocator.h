#ifndef BOND_ALLOCATOR_H
#define BOND_ALLOCATOR_H

#include "bond/resourcehandle.h"

namespace Bond
{

class Allocator
{
public:
	template<typename T>
	class Deallocator
	{
	public:
		Deallocator(): mAllocator(NULL) {}
		Deallocator(Allocator *allocator): mAllocator(allocator) {}

		void operator()(T *ptr) { if (mAllocator != NULL) mAllocator->Free(ptr); }

	private:
		Allocator *mAllocator;
	};

	// Stores a pointer to memory.
	template<typename T>
	class Handle: public PointerHandle<T, Deallocator<T> >
	{
	public:
		Handle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, Deallocator<T> >(ptr, Deallocator<T>(&allocator))
		{}

		Handle(Handle &other):
			PointerHandle<T, Deallocator<T> >(other)
		{}

		Handle(const ResourceHandleProxy<T *, Deallocator<T> > &proxy):
			PointerHandle<T, Deallocator<T> >(proxy)
		{}
	};


	template<typename T>
	class AlignedDeallocator
	{
	public:
		AlignedDeallocator(): mAllocator(NULL) {}
		AlignedDeallocator(Allocator *allocator): mAllocator(allocator) {}

		void operator()(T *ptr) { if (mAllocator != NULL) mAllocator->FreeAligned(ptr); }

	private:
		Allocator *mAllocator;
	};

	// Stores a pointer to aligned memory.
	template<typename T>
	class AlignedHandle: public PointerHandle<T, AlignedDeallocator<T> >
	{
	public:
		AlignedHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(&allocator))
		{}

		AlignedHandle(AlignedHandle &other):
			PointerHandle<T, AlignedDeallocator<T> >(other)
		{}

		AlignedHandle(const ResourceHandleProxy<T *, AlignedDeallocator<T> > &proxy):
			PointerHandle<T, AlignedDeallocator<T> >(proxy)
		{}
	};


	template<typename T>
	class ObjectDeallocator
	{
	public:
		ObjectDeallocator(): mAllocator(NULL) {}
		ObjectDeallocator(Allocator *allocator): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				ptr->~T();
				mAllocator->Free(ptr);
			}
		}

	private:
		Allocator *mAllocator;
	};

	// Stores a pointer to an object which is destroyed when deallocated.
	template<typename T>
	class ObjectHandle: public PointerHandle<T, ObjectDeallocator<T> >
	{
	public:
		ObjectHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(&allocator))
		{}

		ObjectHandle(ObjectHandle &other):
			PointerHandle<T, ObjectDeallocator<T> >(other)
		{}

		ObjectHandle(const ResourceHandleProxy<T *, ObjectDeallocator<T> > &proxy):
			PointerHandle<T, ObjectDeallocator<T> >(proxy)
		{}
	};


	template<typename T>
	class AlignedObjectDeallocator
	{
	public:
		AlignedObjectDeallocator(): mAllocator(NULL) {}
		AlignedObjectDeallocator(Allocator *allocator): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				ptr->~T();
				mAllocator->FreeAligned(ptr);
			}
		}

	private:
		Allocator *mAllocator;
	};

	// Stores a pointer to an aligned object which is destroyed when deallocated.
	template<typename T>
	class AlignedObjectHandle: public PointerHandle<T, AlignedObjectDeallocator<T> >
	{
	public:
		AlignedObjectHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(&allocator))
		{}

		AlignedObjectHandle(AlignedObjectHandle &other):
			PointerHandle<T, AlignedObjectDeallocator<T> >(other)
		{}

		AlignedObjectHandle(const ResourceHandleProxy<T *, AlignedObjectDeallocator<T> > &proxy):
			PointerHandle<T, AlignedObjectDeallocator<T> >(proxy)
		{}
	};


	virtual ~Allocator() {}
	virtual void *Allocate(size_t size) = 0;
	virtual void *AllocateAligned(size_t size, size_t align) = 0;
	virtual void Free(void *buffer) = 0;
	virtual void FreeAligned(void *buffer) = 0;

	void Free(const void *buffer) { Free(const_cast<void *>(buffer)); }
	void FreeAligned(const void *buffer) { FreeAligned(const_cast<void *>(buffer)); }

	template <typename T> T *Alloc() { return static_cast<T *>(Allocate(sizeof(T))); }
	template <typename T> T *Alloc(size_t numElements) { return static_cast<T *>(Allocate(sizeof(T) * numElements)); }

	template <typename T> T *AllocAligned(size_t align) { return static_cast<T *>(AllocateAligned(sizeof(T), align)); }
	template <typename T> T *AllocAligned(size_t numElements, size_t align) { return static_cast<T *>(AllocateAligned(sizeof(T) * numElements, align)); }
};

}

#endif
