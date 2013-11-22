#ifndef BOND_SYSTEMS_RESOURCEHANDLE_H
#define BOND_SYSTEMS_RESOURCEHANDLE_H

#include "bond/conf.h"

namespace Bond
{

// The following Handle classes implement functionality similar to std::unique_ptr, however,
// since this codebase does not depend on C++11 features, it needs a replacement. Since C++11
// features are not used, these handle classes suffer from the some of the same limitations
// that std::auto_ptr does (i.e. no move semantics). Like std::unique_ptr, these handles use a
// functor to deallocate the resource rather than operator delete, meaning that they can be
// customized to free memory to custom allocators, deallocate arrays and deallocate resources
// other than memory. Unlike std::unique_ptr, a ResourceHandle object can wrap any object
// that implements a default constructor, copy constructor and == operator.
template <typename ResourceType, typename DeallocatorType>
struct ResourceHandleProxy
{
	ResourceHandleProxy(const ResourceType &resource, const DeallocatorType &deallocator):
		mResource(resource),
		mDeallocator(deallocator)
	{}

	ResourceType mResource;
	DeallocatorType mDeallocator;
};


template <typename ResourceType, typename DeallocatorType>
class ResourceHandle
{
public:
	typedef ResourceHandle<ResourceType, DeallocatorType> Type;
	typedef ResourceHandleProxy<ResourceType, DeallocatorType> ProxyType;

	ResourceHandle(const ResourceType &resource = ResourceType(), const DeallocatorType &deallocator = DeallocatorType()):
		mResource(resource),
		mDeallocator(deallocator)
	{}

	ResourceHandle(Type &other):
		mResource(other.Release()),
		mDeallocator(other.mDeallocator)
	{}

	ResourceHandle(const ProxyType &proxy):
		mResource(proxy.mResource),
		mDeallocator(proxy.mDeallocator)
	{}

	~ResourceHandle() { Reset(); }

	Type &operator=(Type &other)
	{
		Reset(other.Release());
		this->mDeallocator = other.mDeallocator;
		return *this;
	}

	Type &operator=(const ProxyType &proxy)
	{
		Reset(proxy.mResource);
		this->mDeallocator = proxy.mDeallocator;
		return *this;
	}

	operator ProxyType()
	{
		return ProxyType(Release(), this->mDeallocator);
	}

	void Reset(const ResourceType &resource = ResourceType())
	{
		if (!(resource == this->mResource))
		{
			this->mDeallocator(this->mResource);
			this->mResource = resource;
		}
	}

	ResourceType &Get() { return this->mResource; }
	const ResourceType &Get() const { return this->mResource; }

	ResourceType Release()
	{
		ResourceType resource = this->mResource;
		this->mResource = ResourceType();
		return resource;
	}

protected:
	ResourceType mResource;
	DeallocatorType mDeallocator;
};


template <typename ResourceType, typename DeallocatorType>
class PointerHandle: public ResourceHandle<ResourceType *, DeallocatorType>
{
public:
	typedef ResourceType *PointerType;
	typedef PointerHandle<ResourceType, DeallocatorType> Type;
	typedef ResourceHandle<PointerType, DeallocatorType> BaseType;
	typedef ResourceHandleProxy<PointerType, DeallocatorType> ProxyType;

	PointerHandle(PointerType ptr = PointerType(), const DeallocatorType &deallocator = DeallocatorType()):
		BaseType(ptr, deallocator)
	{}

	PointerHandle(BaseType &other):
		BaseType(other)
	{}

	PointerHandle(const ProxyType &proxy):
		BaseType(proxy)
	{}

	PointerHandle &operator=(const ProxyType &proxy) { BaseType::operator=(proxy); return *this; }

	PointerType operator->() const { return this->mResource; }
	ResourceType &operator*() const { return *this->mResource; }
	ResourceType &operator[](int index) const { return this->mResource[index]; }
	ResourceType &operator[](size_t index) const { return this->mResource[index]; }
};

}

#endif
