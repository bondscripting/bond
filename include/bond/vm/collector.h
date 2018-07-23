#ifndef BOND_VM_COLLECTOR_H
#define BOND_VM_COLLECTOR_H

#include "bond/systems/allocator.h"

namespace Bond
{

class Collectable;

/// \addtogroup vm
/// @{

typedef Allocator::ObjectHandle<Collectable> CollectableHandle;

class Collectable
{
public:
	virtual ~Collectable()
	{
		// Destroy the list of objects iteratively to avoid a recursive stack overflow.
		CollectableHandle curr = move(mNext);
		while (curr != nullptr)
		{
			CollectableHandle next = move(curr->mNext);
			curr = move(next);
		}
	}

	CollectableHandle mNext;
};


template <typename T>
class WrapperCollectable: public Collectable
{
public:
	typedef T ContentType;

	template<typename... Args>
	explicit WrapperCollectable(Args&&... args):
		mContent(forward<Args>(args)...)
	{}

	virtual ~WrapperCollectable() {}

	const T &GetContent() const { return mContent; }
	T &GetContent() { return mContent; }

private:
	T mContent;
};


class Collector
{
public:
	void Register(CollectableHandle &&object)
	{
		if (object != nullptr)
		{
			Collectable *tail = object.get();
			while (tail->mNext != nullptr)
			{
				tail = tail->mNext.get();
			}
			tail->mNext = move(mHead);
			mHead = move(object);
		}
	}

private:
	CollectableHandle mHead;
};

/// @}

}

#endif
