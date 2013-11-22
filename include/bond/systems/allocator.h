#ifndef BOND_SYSTEMS_ALLOCATOR_H
#define BOND_SYSTEMS_ALLOCATOR_H

#include "bond/systems/resourcehandle.h"

namespace Bond
{

class Allocator
{
public:
	template <typename T>
	struct Deallocator
	{
		Deallocator(Allocator *allocator = NULL): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != NULL) mAllocator->Free(ptr); }
		Allocator *mAllocator;
	};

	// Stores a pointer to memory.
	template <typename T>
	class Handle: public PointerHandle<T, Deallocator<T> >
	{
	public:
		Handle():
			PointerHandle<T, Deallocator<T> >(NULL, Deallocator<T>(NULL))
		{}

		Handle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, Deallocator<T> >(ptr, Deallocator<T>(&allocator))
		{}

		Handle(Handle &other):
			PointerHandle<T, Deallocator<T> >(other)
		{}

		Handle(const ResourceHandleProxy<T *, Deallocator<T> > &proxy):
			PointerHandle<T, Deallocator<T> >(proxy)
		{}

		Handle &operator=(const ResourceHandleProxy<T *, Deallocator<T> > &proxy)
		{
			PointerHandle<T, Deallocator<T> >::operator=(proxy);
			return *this;
		}
	};


	template <typename T>
	struct AlignedDeallocator
	{
		AlignedDeallocator(Allocator *allocator = NULL): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != NULL) mAllocator->FreeAligned(ptr); }
		Allocator *mAllocator;
	};

	// Stores a pointer to aligned memory.
	template <typename T>
	class AlignedHandle: public PointerHandle<T, AlignedDeallocator<T> >
	{
	public:
		AlignedHandle():
			PointerHandle<T, AlignedDeallocator<T> >(NULL, AlignedDeallocator<T>(NULL))
		{}

		AlignedHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(&allocator))
		{}

		AlignedHandle(AlignedHandle &other):
			PointerHandle<T, AlignedDeallocator<T> >(other)
		{}

		AlignedHandle(const ResourceHandleProxy<T *, AlignedDeallocator<T> > &proxy):
			PointerHandle<T, AlignedDeallocator<T> >(proxy)
		{}

		AlignedHandle &operator=(const ResourceHandleProxy<T *, AlignedDeallocator<T> > &proxy)
		{
			PointerHandle<T, AlignedDeallocator<T> >::operator=(proxy);
			return *this;
		}
	};


	template <typename T>
	struct ObjectDeallocator
	{
		ObjectDeallocator(Allocator *allocator = NULL): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				ptr->~T();
				mAllocator->Free(ptr);
			}
		}

		Allocator *mAllocator;
	};

	// Stores a pointer to an object which is destroyed when deallocated.
	template <typename T>
	class ObjectHandle: public PointerHandle<T, ObjectDeallocator<T> >
	{
	public:
		ObjectHandle():
			PointerHandle<T, ObjectDeallocator<T> >(NULL, ObjectDeallocator<T>(NULL))
		{}

		ObjectHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(&allocator))
		{}

		ObjectHandle(ObjectHandle &other):
			PointerHandle<T, ObjectDeallocator<T> >(other)
		{}

		ObjectHandle(const ResourceHandleProxy<T *, ObjectDeallocator<T> > &proxy):
			PointerHandle<T, ObjectDeallocator<T> >(proxy)
		{}

		ObjectHandle &operator=(const ResourceHandleProxy<T *, ObjectDeallocator<T> > &proxy)
		{
			PointerHandle<T, ObjectDeallocator<T> >::operator=(proxy);
			return *this;
		}
	};


	template <typename T>
	struct AlignedObjectDeallocator
	{
		AlignedObjectDeallocator(Allocator *allocator = NULL): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				ptr->~T();
				mAllocator->FreeAligned(ptr);
			}
		}

		Allocator *mAllocator;
	};

	// Stores a pointer to an aligned object which is destroyed when deallocated.
	template <typename T>
	class AlignedObjectHandle: public PointerHandle<T, AlignedObjectDeallocator<T> >
	{
	public:
		AlignedObjectHandle():
			PointerHandle<T, AlignedObjectDeallocator<T> >(NULL, AlignedObjectDeallocator<T>(NULL))
		{}

		AlignedObjectHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(&allocator))
		{}

		AlignedObjectHandle(AlignedObjectHandle &other):
			PointerHandle<T, AlignedObjectDeallocator<T> >(other)
		{}

		AlignedObjectHandle(const ResourceHandleProxy<T *, AlignedObjectDeallocator<T> > &proxy):
			PointerHandle<T, AlignedObjectDeallocator<T> >(proxy)
		{}

		AlignedObjectHandle &operator=(const ResourceHandleProxy<T *, AlignedObjectDeallocator<T> > &proxy)
		{
			PointerHandle<T, AlignedObjectDeallocator<T> >::operator=(proxy);
			return *this;
		}
	};


	template <typename T>
	struct ArrayDeallocator
	{
		ArrayDeallocator(Allocator *allocator = NULL, size_t numElements = 0):
			mAllocator(allocator),
			mNumElements(0)
		{}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				for (size_t i = 0; i < mNumElements; ++i)
				{
					ptr[i].~T();
				}
				mAllocator->Free(ptr);
			}
		}

		Allocator *mAllocator;
		size_t mNumElements;
	};

	// Stores a pointer to an array of objects which are destroyed when deallocated.
	template <typename T>
	class ArrayHandle: public PointerHandle<T, ArrayDeallocator<T> >
	{
	public:
		ArrayHandle():
			PointerHandle<T, ArrayDeallocator<T> >(NULL, ArrayDeallocator<T>(NULL))
		{}

		ArrayHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, ArrayDeallocator<T> >(ptr, ArrayDeallocator<T>(&allocator))
		{}

		ArrayHandle(ArrayHandle &other):
			PointerHandle<T, ArrayDeallocator<T> >(other)
		{}

		ArrayHandle(const ResourceHandleProxy<T *, ArrayDeallocator<T> > &proxy):
			PointerHandle<T, ArrayDeallocator<T> >(proxy)
		{}

		ArrayHandle &operator=(const ResourceHandleProxy<T *, ArrayDeallocator<T> > &proxy)
		{
			PointerHandle<T, ArrayDeallocator<T> >::operator=(proxy);
			return *this;
		}

		size_t GetNumElements() const { return this->mDeallocator.mNumElements; }
		void SetNumElements(size_t numElements) { this->mDeallocator.mNumElements = numElements; }
	};


	template <typename T>
	class AlignedArrayDeallocator
	{
	public:
		AlignedArrayDeallocator(Allocator *allocator = NULL, size_t numElements = 0):
			mAllocator(allocator),
			mNumElements(0)
		{}

		void operator()(T *ptr)
		{
			if (mAllocator != NULL)
			{
				for (size_t i = 0; i < mNumElements; +i)
				{
					ptr[i].~T();
				}
				mAllocator->FreeAligned(ptr);
			}
		}

	private:
		Allocator *mAllocator;
		size_t mNumElements;
	};

	// Stores a pointer to an array of objects which are destroyed when deallocated.
	template <typename T>
	class AlignedArrayHandle: public PointerHandle<T, AlignedArrayDeallocator<T> >
	{
	public:
		AlignedArrayHandle():
			PointerHandle<T, AlignedArrayDeallocator<T> >(NULL, AlignedArrayDeallocator<T>(NULL))
		{}

		AlignedArrayHandle(Allocator &allocator, T *ptr = NULL):
			PointerHandle<T, AlignedArrayDeallocator<T> >(ptr, AlignedArrayDeallocator<T>(&allocator))
		{}

		AlignedArrayHandle(AlignedArrayHandle &other):
			PointerHandle<T, AlignedArrayDeallocator<T> >(other)
		{}

		AlignedArrayHandle(const ResourceHandleProxy<T *, AlignedArrayDeallocator<T> > &proxy):
			PointerHandle<T, AlignedArrayDeallocator<T> >(proxy)
		{}

		AlignedArrayHandle &operator=(const ResourceHandleProxy<T *, AlignedArrayDeallocator<T> > &proxy)
		{
			PointerHandle<T, AlignedArrayDeallocator<T> >::operator=(proxy);
			return *this;
		}

		size_t GetNumElements() const { return this->mDeallocator.mNumElements; }
		void SetNumElements(size_t numElements) { this->mDeallocator.mNumElements = numElements; }
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
