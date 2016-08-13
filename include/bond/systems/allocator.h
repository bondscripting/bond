#ifndef BOND_SYSTEMS_ALLOCATOR_H
#define BOND_SYSTEMS_ALLOCATOR_H

#include "bond/conf.h"
#include "bond/stl/memory.h"
#include "bond/stl/utility.h"

namespace Bond
{

class Allocator
{
public:
	template <typename T>
	struct Deallocator
	{
		Deallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->Free(ptr); }
		Allocator *mAllocator;
	};

	// Stores a pointer to memory.
	template <typename T>
	class Handle: public unique_ptr<T, Deallocator<T> >
	{
	public:
		Handle():
			unique_ptr<T, Deallocator<T> >(nullptr, Deallocator<T>(nullptr))
		{}

		Handle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, Deallocator<T> >(ptr, Deallocator<T>(&allocator))
		{}

		Handle(Handle &&other):
			unique_ptr<T, Deallocator<T> >(forward<Handle>(other))
		{}

		Handle &operator=(Handle &&other)
		{
			unique_ptr<T, Deallocator<T> >::operator=(forward<Handle>(other));
			return *this;
		}
	};


	template <typename T>
	struct AlignedDeallocator
	{
		AlignedDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->FreeAligned(ptr); }
		Allocator *mAllocator;
	};

	// Stores a pointer to aligned memory.
	template <typename T>
	class AlignedHandle: public unique_ptr<T, AlignedDeallocator<T> >
	{
	public:
		AlignedHandle():
			unique_ptr<T, AlignedDeallocator<T> >(nullptr, AlignedDeallocator<T>(nullptr))
		{}

		AlignedHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(&allocator))
		{}

		AlignedHandle(AlignedHandle &&other):
			unique_ptr<T, AlignedDeallocator<T> >(forward<AlignedHandle>(other))
		{}

		AlignedHandle &operator=(AlignedHandle &&other)
		{
			unique_ptr<T, AlignedDeallocator<T> >::operator=(forward<AlignedHandle>(other));
			return *this;
		}
	};


	template <typename T>
	struct ObjectDeallocator
	{
		ObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != nullptr)
			{
				ptr->~T();
				mAllocator->Free(ptr);
			}
		}

		Allocator *mAllocator;
	};

	// Stores a pointer to an object which is destroyed when deallocated.
	template <typename T>
	class ObjectHandle: public unique_ptr<T, ObjectDeallocator<T> >
	{
	public:
		ObjectHandle():
			unique_ptr<T, ObjectDeallocator<T> >(nullptr, ObjectDeallocator<T>(nullptr))
		{}

		ObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(&allocator))
		{}

		ObjectHandle(ObjectHandle &&other):
			unique_ptr<T, ObjectDeallocator<T> >(forward<ObjectHandle>(other))
		{}

		ObjectHandle &operator=(ObjectHandle &&other)
		{
			unique_ptr<T, ObjectDeallocator<T> >::operator=(forward<ObjectHandle>(other));
			return *this;
		}
	};


	template <typename T>
	struct AlignedObjectDeallocator
	{
		AlignedObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		void operator()(T *ptr)
		{
			if (mAllocator != nullptr)
			{
				ptr->~T();
				mAllocator->FreeAligned(ptr);
			}
		}

		Allocator *mAllocator;
	};

	// Stores a pointer to an aligned object which is destroyed when deallocated.
	template <typename T>
	class AlignedObjectHandle: public unique_ptr<T, AlignedObjectDeallocator<T> >
	{
	public:
		AlignedObjectHandle():
			unique_ptr<T, AlignedObjectDeallocator<T> >(nullptr, AlignedObjectDeallocator<T>(nullptr))
		{}

		AlignedObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(&allocator))
		{}

		AlignedObjectHandle(AlignedObjectHandle &&other):
			unique_ptr<T, AlignedObjectDeallocator<T> >(forward<AlignedObjectHandle>(other))
		{}

		AlignedObjectHandle &operator=(AlignedObjectHandle &&other)
		{
			unique_ptr<T, AlignedObjectDeallocator<T> >::operator=(forward<AlignedObjectHandle>(other));
			return *this;
		}
	};

/* TODO: These are no longer used internally. See if they need to make a comeback and
	 be updated to use unique_ptr.

	template <typename T>
	struct ArrayDeallocator
	{
		ArrayDeallocator(Allocator *allocator = nullptr, size_t numElements = 0):
			mAllocator(allocator),
			mNumElements(0)
		{}

		void operator()(T *ptr)
		{
			if (mAllocator != nullptr)
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
			PointerHandle<T, ArrayDeallocator<T> >(nullptr, ArrayDeallocator<T>(nullptr))
		{}

		ArrayHandle(Allocator &allocator, T *ptr = nullptr):
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
		AlignedArrayDeallocator(Allocator *allocator = nullptr, size_t numElements = 0):
			mAllocator(allocator),
			mNumElements(0)
		{}

		void operator()(T *ptr)
		{
			if (mAllocator != nullptr)
			{
				for (size_t i = 0; i < mNumElements; ++i)
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
			PointerHandle<T, AlignedArrayDeallocator<T> >(nullptr, AlignedArrayDeallocator<T>(nullptr))
		{}

		AlignedArrayHandle(Allocator &allocator, T *ptr = nullptr):
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
*/

	virtual ~Allocator() {}
	virtual void *Allocate(size_t size) = 0;
	virtual void *AllocateAligned(size_t size, size_t align) = 0;
	virtual void Free(void *buffer) = 0;
	virtual void FreeAligned(void *buffer) = 0;

	void Free(const void *buffer) { Free(const_cast<void *>(buffer)); }
	void FreeAligned(const void *buffer) { FreeAligned(const_cast<void *>(buffer)); }

	template <typename T>
	T *Alloc()
	{
		return static_cast<T *>(Allocate(sizeof(T)));
	}

	template <typename T>
	T *Alloc(size_t numElements)
	{
		return static_cast<T *>(Allocate(sizeof(T) * numElements));
	}

	template <typename T>
	Handle<T> AllocOwned()
	{
		return Handle<T>(*this, Alloc<T>());
	}

	template <typename T>
	Handle<T> AllocOwned(size_t numElements)
	{
		return Handle<T>(*this, Alloc<T>(numElements));
	}

	template <typename T>
	T *AllocAligned(size_t align)
	{
		return static_cast<T *>(AllocateAligned(sizeof(T), align));
	}

	template <typename T>
	T *AllocAligned(size_t numElements, size_t align)
	{
		return static_cast<T *>(AllocateAligned(sizeof(T) * numElements, align));
	}

	template <typename T>
	AlignedHandle<T> AllocOwnedAligned(size_t align)
	{
		return AlignedHandle<T>(*this, AllocAligned<T>(align));
	}

	template <typename T>
	AlignedHandle<T> AllocOwnedAligned(size_t numElements, size_t align)
	{
		return AlignedHandle<T>(*this, AllocAligned<T>(numElements, align));
	}

	template<typename T, typename... Args>
	T *AllocObject(Args&&... args)
	{
		auto memHandle = AllocOwned<T>();
		new (memHandle.get()) T(forward<Args>(args)...);
		return memHandle.release();
	}

	template<typename T, typename... Args>
	ObjectHandle<T> AllocOwnedObject(Args&&... args)
	{
		return ObjectHandle<T>(*this, AllocObject<T>(forward<Args>(args)...));
	}

	template<typename T, typename... Args>
	T *AllocAlignedObject(size_t align, Args&&... args)
	{
		auto memHandle = AllocOwnedAligned<T>(align);
		new (memHandle.get()) T(forward<Args>(args)...);
		return memHandle.release();
	}

	template<typename T, typename... Args>
	AlignedObjectHandle<T> AllocOwnedAlignedObject(size_t align, Args&&... args)
	{
		return AlignedObjectHandle<T>(*this, AllocAlignedObject<T>(align, forward<Args>(args)...));
	}
};

}

#endif
