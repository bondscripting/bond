#ifndef BOND_ALLOCATOR_H
#define BOND_ALLOCATOR_H

#include "bond/conf.h"

namespace Bond
{

class Allocator
{
public:
	// The following Handle classes implement functionality similar to std::auto_ptr except that
	// they free memory by returning it to an allocator rather than calling the delete operator.
	// Note that they only free memory; they do not call destructors. They also come with the
	// same limitations that caused auto_ptr to become deprecated. At the time these classes
	// were implemented, C++11 unique_ptr was not universally supported, and the limited
	// functionality of these classes was sufficient.
	template<typename T>
	struct HandleProxy
	{
		explicit HandleProxy(Allocator *allocator, T *ptr): mAllocator(allocator), mPtr(ptr) {}
		Allocator *mAllocator;
		T *mPtr;
	};

	template<typename T>
	class HandleBase
	{
	public:
		Allocator &GetAllocator() const { return mAllocator; }
		T *Get() const { return mPtr; }
		T &operator*() const { return *mPtr; }
		T *operator->() const { return mPtr; }
		operator HandleProxy<T>() { return HandleProxy<T>(mAllocator, Release()); }

		T *Release()
		{
			T *ptr = mPtr;
			mPtr = 0;
			return ptr;
		}

	protected:
		HandleBase(Allocator &allocator, T *ptr = NULL): mAllocator(&allocator), mPtr(ptr) {}
		HandleBase(HandleBase<T>& other): mAllocator(other.mAllocator), mPtr(other.Release()) {}
		HandleBase(const HandleProxy<T>& proxy): mAllocator(proxy.mAllocator), mPtr(proxy.mPtr) {}
		~HandleBase() {}

		Allocator *mAllocator;
		T *mPtr;
	};


	template<typename T>
	class Handle: public HandleBase<T>
	{
	public:
		Handle(Allocator &allocator, T *ptr = NULL): HandleBase<T>(allocator, ptr) {}
		Handle(Handle<T>& other): HandleBase<T>(other) {}
		Handle(const HandleProxy<T>& proxy): HandleBase<T>(proxy) {}
		~Handle() { Reset(); }

		Handle &operator=(Handle<T>& other)
		{
			Reset(other.mAllocator, other.Release());
			return *this;
		}

		void Reset(T *ptr = 0)
		{
			if (ptr != this->mPtr)
			{
				this->mAllocator->Free(this->mPtr);
				this->mPtr = ptr;
			}
		}

		private:
		void Reset(Allocator *allocator, T *ptr = 0)
		{
			if (ptr != this->mPtr)
			{
				this->mAllocator->Free(this->mPtr);
				this->mPtr = ptr;
			}
			this->mAllocator = allocator;
		}
	};


	template<typename T>
	class AlignedHandle: public HandleBase<T>
	{
	public:
		AlignedHandle(Allocator &allocator, T *ptr = NULL): HandleBase<T>(allocator, ptr) {}
		AlignedHandle(AlignedHandle<T>& other): HandleBase<T>(other) {}
		AlignedHandle(const HandleProxy<T>& proxy): HandleBase<T>(proxy) {}
		~AlignedHandle() { Reset(); }

		AlignedHandle &operator=(AlignedHandle<T>& other)
		{
			Reset(other.mAllocator, other.Release());
			return *this;
		}

		void Reset(T *ptr = 0)
		{
			if (ptr != this->mPtr)
			{
				this->mAllocator->FreeAligned(this->mPtr);
				this->mPtr = ptr;
			}
		}

		private:
		void Reset(Allocator *allocator, T *ptr = 0)
		{
			if (ptr != this->mPtr)
			{
				this->mAllocator->FreeAligned(this->mPtr);
				this->mPtr = ptr;
			}
			this->mAllocator = allocator;
		}
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
