#ifndef BOND_VM_COLLECTOR_H
#define BOND_VM_COLLECTOR_H

#include "bond/systems/allocator.h"

namespace Bond
{

class Collectable;

/// \addtogroup vm
/// @{

/// \brief An owning pointer to a dynamically allocated Collectable.
typedef Allocator::ObjectHandle<Collectable> CollectableHandle;


/// \brief Base class for objects tracked by a Collector.
class Collectable
{
public:
	virtual ~Collectable()
	{
		// Destroy the list of objects iteratively to avoid a recursive stack overflow.
		CollectableHandle curr = Bond::move(mNext);
		while (curr != nullptr)
		{
			CollectableHandle next = Bond::move(curr->mNext);
			curr = Bond::move(next);
		}
	}

	friend class Collector;

private:
	CollectableHandle mNext;
};


/// \brief A Collectable wrapper for arbitrary content.
///
/// This class can be used to register non-Collectable objects with a Collector by storing them
/// as wrapped content.
template <typename T>
class WrapperCollectable: public Collectable
{
public:
	typedef T ContentType;

	/// \brief Constructs the wrapped content in place.
	/// \param args Arguments forwarded to the wrapped content constructor.
	template<typename... Args>
	explicit WrapperCollectable(Args&&... args):
		mContent(Bond::forward<Args>(args)...)
	{}

	virtual ~WrapperCollectable() {}

	const T &GetContent() const { return mContent; }
	T &GetContent() { return mContent; }

private:
	T mContent;
};


/// \brief Tracks and destroys Collectable objects.
///
/// All tracked Collectable objects are destroyed when the Collector is destroyed.
class Collector
{
public:
	/// \brief Registers a Collectable object for later destruction.
	/// \param object An owning handle to the Collectable object.
	void Register(CollectableHandle &&object)
	{
		if (object != nullptr)
		{
			Collectable *tail = object.get();
			while (tail->mNext != nullptr)
			{
				tail = tail->mNext.get();
			}
			tail->mNext = Bond::move(mHead);
			mHead = Bond::move(object);
		}
	}

private:
	CollectableHandle mHead;
};

/// @}

}

#endif
