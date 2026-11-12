#ifndef BOND_VM_COLLECTOR_H
#define BOND_VM_COLLECTOR_H

#include "bond/conf.h"
#include "bond/systems/allocator.h"

namespace Bond
{

class Collectable;

/// \addtogroup vm
/// @{

/// \brief An owning pointer to a dynamically allocated Collectable.
typedef Allocator::ObjectHandle<Collectable> CollectableHandle;


/// \brief Base class for objects tracked by a Collector.
///
/// Each Collectable holds an owning handle to the next object in the Collector's list, so
/// destroying a Collectable also destroys all objects registered with the Collector before it.
class BOND_API Collectable
{
public:
	/// \brief Destroys the Collectable and the remainder of the list that follows it.
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
/// \tparam T The type of the wrapped content.
template <typename T>
class WrapperCollectable: public Collectable
{
public:
	/// \brief The type of the wrapped content.
	typedef T ContentType;

	/// \brief Constructs a WrapperCollectable object, constructing the wrapped content in place.
	/// \param args Arguments forwarded to the wrapped content constructor.
	template<typename... Args>
	explicit WrapperCollectable(Args&&... args):
		mContent(Bond::forward<Args>(args)...)
	{}

	/// \brief Destroys the WrapperCollectable and its wrapped content.
	virtual ~WrapperCollectable() {}

	/// \brief Returns the wrapped content.
	const T &GetContent() const { return mContent; }

	/// \brief Returns the wrapped content.
	T &GetContent() { return mContent; }

private:
	T mContent;
};


/// \brief Tracks and destroys Collectable objects.
///
/// All tracked Collectable objects are destroyed when the Collector is destroyed, most recently
/// registered first. Each StackFrame has a Collector, which allows native functions to tie the
/// lifetime of objects they create to that of a function call.
///
/// \sa StackFrame::GetCollector
class BOND_API Collector
{
public:
	/// \brief Registers a Collectable object for later destruction.
	/// \param object An owning handle to the Collectable object. The Collector takes ownership of
	///        the object, and of any objects already chained to it. Nothing is registered if the
	///        handle is null.
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
