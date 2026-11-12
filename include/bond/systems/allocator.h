#ifndef BOND_SYSTEMS_ALLOCATOR_H
#define BOND_SYSTEMS_ALLOCATOR_H

#include "bond/conf.h"
#include "bond/stl/memory.h"
#include "bond/stl/typetraits.h"
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
class BOND_API Allocator
{
public:
	/// \brief A custom unique_ptr deleter that deallocates memory by calling Allocator::Free rather
	/// than the delete operator. The memory is only freed; no object destruction is performed.
	template <typename T>
	struct Deallocator
	{
		/// \brief Constructs a Deallocator object.
		/// \param allocator The allocator that frees the memory. If null, the memory is not freed.
		explicit Deallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		/// \brief Frees the given memory by calling Allocator::Free.
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->Free(ptr); }

		/// \brief The allocator that frees the memory.
		Allocator *mAllocator;
	};

	/// \brief An owning memory handle that derives from unique_ptr that deallocates memory using a
	/// Deallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class Handle: public unique_ptr<T, Deallocator<T> >
	{
	public:
		/// \brief Constructs a Handle object.
		/// \param allocator The allocator that frees the memory, or null if it is not to be freed.
		/// \param ptr Pointer to the owned memory, which must have been allocated by \p allocator.
		Handle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, Deallocator<T> >(ptr, Deallocator<T>(allocator))
		{}

		/// \brief Constructs a Handle object.
		/// \param allocator The allocator that frees the memory.
		/// \param ptr Pointer to the owned memory, which must have been allocated by \p allocator.
		Handle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, Deallocator<T> >(ptr, Deallocator<T>(&allocator))
		{}

		/// \brief Constructs a Handle object that takes ownership of the memory owned by another.
		Handle(Handle &&other):
			unique_ptr<T, Deallocator<T> >(Bond::move(other))
		{}

		/// \brief Frees the currently owned memory and takes ownership of the memory owned by
		/// another Handle.
		Handle &operator=(Handle &&other)
		{
			unique_ptr<T, Deallocator<T> >::operator=(Bond::move(other));
			return *this;
		}

		/// \brief Transfers ownership of the memory to a Handle of another pointer type.
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
		/// \brief Constructs an AlignedDeallocator object.
		/// \param allocator The allocator that frees the memory. If null, the memory is not freed.
		explicit AlignedDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		/// \brief Frees the given memory by calling Allocator::FreeAligned.
		void operator()(T *ptr) { if (mAllocator != nullptr) mAllocator->FreeAligned(ptr); }

		/// \brief The allocator that frees the memory.
		Allocator *mAllocator;
	};

	/// \brief An owning memory handle that derives from unique_ptr that deallocates memory using an
	/// AlignedDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class AlignedHandle: public unique_ptr<T, AlignedDeallocator<T> >
	{
	public:
		/// \brief Constructs an AlignedHandle object.
		/// \param allocator The allocator that frees the memory, or null if it is not to be freed.
		/// \param ptr Pointer to the owned memory, which must have been allocated by \p allocator
		///        with AllocateAligned.
		AlignedHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(allocator))
		{}

		/// \brief Constructs an AlignedHandle object.
		/// \param allocator The allocator that frees the memory.
		/// \param ptr Pointer to the owned memory, which must have been allocated by \p allocator
		///        with AllocateAligned.
		AlignedHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedDeallocator<T> >(ptr, AlignedDeallocator<T>(&allocator))
		{}

		/// \brief Constructs an AlignedHandle object that takes ownership of the memory owned by
		/// another.
		AlignedHandle(AlignedHandle &&other):
			unique_ptr<T, AlignedDeallocator<T> >(Bond::move(other))
		{}

		/// \brief Frees the currently owned memory and takes ownership of the memory owned by
		/// another AlignedHandle.
		AlignedHandle &operator=(AlignedHandle &&other)
		{
			unique_ptr<T, AlignedDeallocator<T> >::operator=(Bond::move(other));
			return *this;
		}

		/// \brief Transfers ownership of the memory to an AlignedHandle of another pointer type.
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
		/// \brief Constructs an ObjectDeallocator object.
		/// \param allocator The allocator that frees the memory. If null, the object is neither
		///        destroyed nor freed.
		explicit ObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		/// \brief Destroys the given object and frees its memory by calling Allocator::Free.
		void operator()(T *ptr)
		{
			if ((mAllocator != nullptr) && (ptr != nullptr))
			{
				ptr->~T();
				mAllocator->Free(ptr);
			}
		}

		/// \brief The allocator that frees the memory.
		Allocator *mAllocator;
	};

	/// \brief An owning object handle that derives from unique_ptr that destroys an object using an
	/// ObjectDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class ObjectHandle: public unique_ptr<T, ObjectDeallocator<T> >
	{
	public:
		/// \brief Constructs an ObjectHandle object.
		/// \param allocator The allocator that frees the memory, or null if the object is not to
		///        be destroyed.
		/// \param ptr Pointer to the owned object, whose memory must have been allocated by
		///        \p allocator.
		ObjectHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(allocator))
		{}

		/// \brief Constructs an ObjectHandle object.
		/// \param allocator The allocator that frees the memory.
		/// \param ptr Pointer to the owned object, whose memory must have been allocated by
		///        \p allocator.
		ObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, ObjectDeallocator<T> >(ptr, ObjectDeallocator<T>(&allocator))
		{}

		/// \brief Constructs an ObjectHandle object that takes ownership of the object owned by
		/// another.
		ObjectHandle(ObjectHandle &&other):
			unique_ptr<T, ObjectDeallocator<T> >(Bond::move(other))
		{}

		/// \brief Destroys the currently owned object and takes ownership of the object owned by
		/// another ObjectHandle.
		ObjectHandle &operator=(ObjectHandle &&other)
		{
			unique_ptr<T, ObjectDeallocator<T> >::operator=(Bond::move(other));
			return *this;
		}

		/// \brief Transfers ownership of the object to an ObjectHandle of another type, which must
		/// be T or have a virtual destructor so that the object is destroyed correctly.
		template <typename U, typename Bond::enable_if<Bond::is_same<U, T>::value || Bond::has_virtual_destructor<U>::value, int>::type = 0>
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
		/// \brief Constructs an AlignedObjectDeallocator object.
		/// \param allocator The allocator that frees the memory. If null, the object is neither
		///        destroyed nor freed.
		explicit AlignedObjectDeallocator(Allocator *allocator = nullptr): mAllocator(allocator) {}

		/// \brief Destroys the given object and frees its memory by calling Allocator::FreeAligned.
		void operator()(T *ptr)
		{
			if ((mAllocator != nullptr) && (ptr != nullptr))
			{
				ptr->~T();
				mAllocator->FreeAligned(ptr);
			}
		}

		/// \brief The allocator that frees the memory.
		Allocator *mAllocator;
	};

	/// \brief An owning object handle that derives from unique_ptr that destroys an aligned object using
	/// an AlignedObjectDeallocator rather than std::default_delete when it goes out of scope.
	template <typename T>
	class AlignedObjectHandle: public unique_ptr<T, AlignedObjectDeallocator<T> >
	{
	public:
		/// \brief Constructs an AlignedObjectHandle object.
		/// \param allocator The allocator that frees the memory, or null if the object is not to
		///        be destroyed.
		/// \param ptr Pointer to the owned object, whose memory must have been allocated by
		///        \p allocator with AllocateAligned.
		AlignedObjectHandle(Allocator *allocator = nullptr, T *ptr = nullptr):
			unique_ptr<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(allocator))
		{}

		/// \brief Constructs an AlignedObjectHandle object.
		/// \param allocator The allocator that frees the memory.
		/// \param ptr Pointer to the owned object, whose memory must have been allocated by
		///        \p allocator with AllocateAligned.
		AlignedObjectHandle(Allocator &allocator, T *ptr = nullptr):
			unique_ptr<T, AlignedObjectDeallocator<T> >(ptr, AlignedObjectDeallocator<T>(&allocator))
		{}

		/// \brief Constructs an AlignedObjectHandle object that takes ownership of the object owned
		/// by another.
		AlignedObjectHandle(AlignedObjectHandle &&other):
			unique_ptr<T, AlignedObjectDeallocator<T> >(Bond::move(other))
		{}

		/// \brief Destroys the currently owned object and takes ownership of the object owned by
		/// another AlignedObjectHandle.
		AlignedObjectHandle &operator=(AlignedObjectHandle &&other)
		{
			unique_ptr<T, AlignedObjectDeallocator<T> >::operator=(Bond::move(other));
			return *this;
		}

		/// \brief Transfers ownership of the object to an AlignedObjectHandle of another type,
		/// which must be T or have a virtual destructor so that the object is destroyed correctly.
		template <typename U, typename Bond::enable_if<Bond::is_same<U, T>::value || Bond::has_virtual_destructor<U>::value, int>::type = 0>
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

	/// \brief Destroys the Allocator.
	virtual ~Allocator() {}

	/// \brief Allocates and returns a pointer to a block of uninitialized memory of the given size.
	///
	/// Implementations are expected to report failure by throwing an exception, such as
	/// std::bad_alloc, or by returning null.
	/// \param size The size of the block in bytes.
	virtual void *Allocate(size_t size) = 0;

	/// \brief Allocates and returns a pointer to a block of uninitialized memory with the given size and alignment.
	///
	/// Implementations are expected to report failure by throwing an exception, such as
	/// std::bad_alloc, or by returning null.
	/// \param size The size of the block in bytes.
	/// \param align The alignment of the block in bytes.
	virtual void *AllocateAligned(size_t size, size_t align) = 0;

	/// \brief Frees the memory pointed to by the given pointer. The memory must have been allocated by
	/// this allocator's Allocate method. Does nothing if the pointer is null.
	/// \param buffer Pointer to the memory to be freed.
	virtual void Free(void *buffer) = 0;

	/// \brief Frees the aligned memory pointed to by the given pointer. The memory must have been
	/// allocated by this allocator's AllocateAligned method. Does nothing if the pointer is null.
	/// \param buffer Pointer to the memory to be freed.
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

	/// \brief Allocates and returns a pointer to a block of uninitialized aligned memory large enough
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
		new (memHandle.get()) T(Bond::forward<Args>(args)...);
		return memHandle.release();
	}

	/// \brief Allocates, constructs and returns an owning handle to an instance of type T.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	ObjectHandle<T> AllocOwnedObject(Args&&... args)
	{
		return ObjectHandle<T>(this, AllocObject<T>(Bond::forward<Args>(args)...));
	}

	/// \brief Allocates, constructs and returns a pointer to an aligned instance of type T.
	/// \param align The alignment requirements for the allocated memory.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	T *AllocAlignedObject(size_t align, Args&&... args)
	{
		auto memHandle = AllocOwnedAligned<T>(align);
		new (memHandle.get()) T(Bond::forward<Args>(args)...);
		return memHandle.release();
	}

	/// \brief Allocates, constructs and returns an owning handle to an aligned instance of type T.
	/// \param align The alignment requirements for the allocated memory.
	/// \param args A list of arguments forwarded to the constructor of the instance of type T.
	template<typename T, typename... Args>
	AlignedObjectHandle<T> AllocOwnedAlignedObject(size_t align, Args&&... args)
	{
		return AlignedObjectHandle<T>(this, AllocAlignedObject<T>(align, Bond::forward<Args>(args)...));
	}
};

}

#endif
