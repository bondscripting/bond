#ifndef BOND_SYSTEMS_ALLOCATOR_H
#define BOND_SYSTEMS_ALLOCATOR_H

#include "bond/conf.h"
#include "bond/stl/memory.h"
#include "bond/stl/utility.h"

namespace Bond
{

/// \brief Abstract base class defining the interface through which all components of Bond perform
/// memory and object allocations.
///
/// The Allocator provides a variety of methods to allocate memory and to allocate and construct
/// objects, all of which can either be unaligned or aligned. Additionally, it provides methods to
/// destroy objects and free memory, as well as handles that derive from the standard C++ unique_ptr
/// to ensure that resources are released reliably.
///
/// Concrete derived classes need to provide implementations for the Allocator::Allocate,
/// Allocator::AllocateAligned, Allocator::Free and Allocator::FreeAligned methods. All other
/// functionality provided by this class is built on top of those four methods.
///
/// \ingroup systems
class Allocator
{
public:
	/// \brief A custom unique_ptr deleter that deallocates memory by calling Allocator::Free rather
	/// than the delete operator. The memory is only freed; no object destruction is performed.
	template <typename T>
	struct Deallocator
	{
		explicit Deallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->Free(ptr); }
		Allocator *mAllocator;
	};

	/// \brief An owning memory handle that derives from unique_ptr that deallocates memory using a
	/// Deallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class Handle: public unique_ptr<T, Deallocator<T> >
	{
	public:
		Handle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, Deallocator<T> >(ptr, Deallocator<T>(allocator))
		{}

		Handle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, Deallocator<T> >(ptr, Deallocator<T>(&allocator))
		{}

		Handle(Handle &&other):
			unique_ptr<T, Deallocator<T> >(move(other))
		{}

		Handle &operator=(Handle &&other)
		{
			unique_ptr<T, Deallocator<T> >::operator=(move(other));
			return *this;
		}

		template <typename U>
		operator Handle<U>() &&
		{
			return Handle<U>(this->get_deleter().mAllocator, this->release());
		}
	};


	/// \brief A custom unique_ptr deleter that deallocates aligned memory by calling Allocator::FreeAligned,
	/// rather than the delete operator. The memory is only freed; no object destruction is performed.
	template <typename T>
	struct AlignedDeallocator
	{
		explicit AlignedDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->FreeAligned(ptr); }
		Allocator *mAllocator;
	};

	/// \brief An owning memory handle that derives from unique_ptr that deallocates memory using an
	/// AlignedDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class AlignedHandle: public unique_ptr<T, AlignedDeallocator<T> >
	{
	public:
		AlignedHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(allocator))
		{}

		AlignedHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(&allocator))
		{}

		AlignedHandle(AlignedHandle &&other):
			unique_ptr<T, AlignedDeallocator<T> >(move(other))
		{}

		AlignedHandle &operator=(AlignedHandle &&other)
		{
			unique_ptr<T, AlignedDeallocator<T> >::operator=(move(other));
			return *this;
		}

		template <typename U>
		operator AlignedHandle<U>() &&
		{
			return AlignedHandle<U>(this->get_deleter().mAllocator, this->release());
		}
	};


	/// \brief A custom unique_ptr deleter that destroys an object by calling its destructor then freeing
	/// its memory by calling Allocator::Free rather than the delete operator.
	template <typename T>
	struct ObjectDeallocator
	{
		explicit ObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

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

	/// \brief An owning object handle that derives from unique_ptr that destroys an object using an
	/// ObjectDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class ObjectHandle: public unique_ptr<T, ObjectDeallocator<T> >
	{
	public:
		ObjectHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(allocator))
		{}

		ObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(&allocator))
		{}

		ObjectHandle(ObjectHandle &&other):
			unique_ptr<T, ObjectDeallocator<T> >(move(other))
		{}

		ObjectHandle &operator=(ObjectHandle &&other)
		{
			unique_ptr<T, ObjectDeallocator<T> >::operator=(move(other));
			return *this;
		}

		template <typename U>
		operator ObjectHandle<U>() &&
		{
			return ObjectHandle<U>(this->get_deleter().mAllocator, this->release());
		}
	};


	/// \brief A custom unique_ptr deleter that destroys an aligned object by calling its destructor then
	/// freeing its memory by calling Allocator::FreeAligned rather than the delete operator.
	template <typename T>
	struct AlignedObjectDeallocator
	{
		explicit AlignedObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

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

	/// \brief An owning object handle that derives from unique_ptr that destroys an aligned object using
	/// an AlignedObjectDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class AlignedObjectHandle: public unique_ptr<T, AlignedObjectDeallocator<T> >
	{
	public:
		AlignedObjectHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(allocator))
		{}

		AlignedObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(&allocator))
		{}

		AlignedObjectHandle(AlignedObjectHandle &&other):
			unique_ptr<T, AlignedObjectDeallocator<T> >(move(other))
		{}

		AlignedObjectHandle &operator=(AlignedObjectHandle &&other)
		{
			unique_ptr<T, AlignedObjectDeallocator<T> >::operator=(move(other));
			return *this;
		}

		template <typename U>
		operator AlignedObjectHandle<U>() &&
		{
			return AlignedObjectHandle<U>(this->get_deleter().mAllocator, this->release());
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

	/// \brief Allocates and returns a pointer to a block of uninitialized memory of the given size.
	virtual void *Allocate(size_t size) = 0;

	/// \brief Allocates and returns a pointer to a block of uninitialized memory with the given size and alignment.
	virtual void *AllocateAligned(size_t size, size_t align) = 0;

	/// \brief Frees the memory pointed to by the given pointer. The memory must have been allocated by
	/// this allocator's Allocate method.
	virtual void Free(void *buffer) = 0;

	/// \brief Frees the aligned memory pointed to by the given pointer. The memory must have been
	/// allocated by this allocator's Allocate method.
	virtual void FreeAligned(void *buffer) = 0;

	/// \brief Calls the non-const form of Free after casting away the constness of the given pointer.
	void Free(const void *buffer) { Free(const_cast<void *>(buffer)); }

	/// \brief Calls the non-const form of FreeAligned after casting away the constness of the given pointer.
	void FreeAligned(const void *buffer) { FreeAligned(const_cast<void *>(buffer)); }

	/// \brief Allocates and returns a pointer to a block of uninitialized memory large enough to
	/// accommodate an object of type T.
	template <typename T>
	T *Alloc()
	{
		return static_cast<T *>(Allocate(sizeof(T)));
	}

	/// \brief Allocates and returns a pointer to a block of uninitialized memory large enough to
	/// accommodate an array of objects of type T.
	/// \param numElements The number of elements in the allocated array.
	template <typename T>
	T *Alloc(size_t numElements)
	{
		return static_cast<T *>(Allocate(sizeof(T) * numElements));
	}

	/// \brief Allocates and returns an owning handle to a block of uninitialized memory large enough
	/// to accommodate an object of type T.
	template <typename T>
	Handle<T> AllocOwned()
	{
		return Handle<T>(this, Alloc<T>());
	}

	/// \brief Allocates and returns an owning handle to a block of uninitialized memory large enough
	/// to accommodate an array of objects of type T.
	/// \param numElements The number of elements in the allocated array.
	template <typename T>
	Handle<T> AllocOwned(size_t numElements)
	{
		return Handle<T>(this, Alloc<T>(numElements));
	}

	/// \brief Allocates and returns a pointer to a block of uninitialized aligned memory large enough
	/// to accommodate an object of type T.
	/// \param align The alignment requirements for the allocated memory.
	template <typename T>
	T *AllocAligned(size_t align)
	{
		return static_cast<T *>(AllocateAligned(sizeof(T), align));
	}

	/// \brief Allocates and returns a pointer to a block of uninitialized aligned  memory large enough
	/// to accommodate an array of objects of type T.
	/// \param numElements The number of elements in the allocated array.
	/// \param align The alignment requirements for the allocated memory.
	template <typename T>
	T *AllocAligned(size_t numElements, size_t align)
	{
		return static_cast<T *>(AllocateAligned(sizeof(T) * numElements, align));
	}

	/// \brief Allocates and returns an owning handle to a block of uninitialized aligned memory large
	/// enough to accommodate an object of type T.
	/// \param align The alignment requirements for the allocated memory.
	template <typename T>
	AlignedHandle<T> AllocOwnedAligned(size_t align)
	{
		return AlignedHandle<T>(this, AllocAligned<T>(align));
	}

	/// \brief Allocates and returns an owning handle to a block of uninitialized aligned memory large
	/// enough to accommodate an array of objects of type T.
	/// \param numElements The number of elements in the allocated array.
	/// \param align The alignment requirements for the allocated memory.
	template <typename T>
	AlignedHandle<T> AllocOwnedAligned(size_t numElements, size_t align)
	{
		return AlignedHandle<T>(this, AllocAligned<T>(numElements, align));
	}

	/// \brief Allocates, constructs and returns a pointer to an instance of type T.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	T *AllocObject(Args&&... args)
	{
		auto memHandle = AllocOwned<T>();
		new (memHandle.get()) T(forward<Args>(args)...);
		return memHandle.release();
	}

	/// \brief Allocates, constructs and returns an owning handle to an instance of type T.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	ObjectHandle<T> AllocOwnedObject(Args&&... args)
	{
		return ObjectHandle<T>(this, AllocObject<T>(forward<Args>(args)...));
	}

	/// \brief Allocates, constructs and returns a pointer to an aligned instance of type T.
	/// \param align The alignment requirements for the allocated memory.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	T *AllocAlignedObject(size_t align, Args&&... args)
	{
		auto memHandle = AllocOwnedAligned<T>(align);
		new (memHandle.get()) T(forward<Args>(args)...);
		return memHandle.release();
	}

	/// \brief Allocates, constructs and returns an owning handle to an aligned instance of type T.
	/// \param align The alignment requirements for the allocated memory.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	AlignedObjectHandle<T> AllocOwnedAlignedObject(size_t align, Args&&... args)
	{
		return AlignedObjectHandle<T>(this, AllocAlignedObject<T>(align, forward<Args>(args)...));
	}
};

}

#endif
