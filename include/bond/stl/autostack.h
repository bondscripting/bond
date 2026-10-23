#ifndef BOND_STL_AUTOSTACK_H
#define BOND_STL_AUTOSTACK_H

#include "bond/stl/utility.h"
#include "bond/systems/assert.h"
#include "bond/types/types.h"
#include <cassert>

namespace Bond
{

/// A stack whose elements are automatic variables. Pushing returns an Element that must
/// be kept alive for as long as the value should remain on the stack; destroying it pops
/// the value. Elements must be destroyed in reverse order of creation and must not outlive
/// the stack. The stack cannot be copied or moved.
template <typename ValueType>
class AutoStack
{
public:
	class Iterator;
	class ConstIterator;

	class Element
	{
	public:
		explicit Element(AutoStack &stack):
			mValue(),
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		template<typename... Args>
		Element(AutoStack &stack, Args&&... args):
			mValue(Bond::forward<Args>(args)...),
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		~Element()
		{
			// Not BOND_ASSERT: destructors are noexcept, so a throw would terminate anyway.
			assert(mStack.mTop == this);
			mStack.mTop = mNext;
		}

		Element(const Element &other) = delete;
		Element &operator=(const Element &other) = delete;

		Element &operator=(const ValueType &value) { mValue = value; return *this; }
		Element &operator=(ValueType &&value) { mValue = Bond::move(value); return *this; }

		operator const ValueType&() const noexcept { return mValue; }
		operator ValueType&() noexcept { return mValue; }

		const ValueType &GetValue() const noexcept { return mValue; }
		ValueType &GetValue() noexcept { return mValue; }

		void SetValue(const ValueType &value) { mValue = value; }
		void SetValue(ValueType &&value) { mValue = Bond::move(value); }

	protected:
		Element *GetNext() noexcept { return mNext; }
		const Element *GetNext() const noexcept { return mNext; }

		ValueType mValue;

	private:
		friend class AutoStack;
		friend class Iterator;
		friend class ConstIterator;

		void SetNext(Element *next) noexcept { mNext = next; }

		AutoStack &mStack;
		Element *mNext;
	};


	class Iterator
	{
	public:
		Iterator() noexcept: mElement(nullptr) {}

		bool operator==(const Iterator& other) const noexcept { return mElement == other.mElement; }
		bool operator!=(const Iterator& other) const noexcept { return mElement != other.mElement; }

		ValueType &operator*() const
		{
			BOND_ASSERT(mElement != nullptr);
			return mElement->GetValue();
		}

		ValueType *operator->() const
		{
			BOND_ASSERT(mElement != nullptr);
			return &mElement->GetValue();
		}

		Iterator& operator++()
		{
			BOND_ASSERT(mElement != nullptr);
			mElement = mElement->GetNext();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator temp = *this;
			++*this;
			return temp;
		}

	private:
		friend class AutoStack;
		friend class ConstIterator;

		explicit Iterator(Element *element) noexcept: mElement(element) {}

		Element *mElement;
	};


	class ConstIterator
	{
	public:
		ConstIterator() noexcept: mElement(nullptr) {}
		ConstIterator(const Iterator &iterator) noexcept: mElement(iterator.mElement) {}

		// Hidden friends so that mixed Iterator/ConstIterator comparisons work in either order.
		friend bool operator==(const ConstIterator& a, const ConstIterator& b) noexcept { return a.mElement == b.mElement; }
		friend bool operator!=(const ConstIterator& a, const ConstIterator& b) noexcept { return a.mElement != b.mElement; }

		const ValueType &operator*() const
		{
			BOND_ASSERT(mElement != nullptr);
			return mElement->GetValue();
		}

		const ValueType *operator->() const
		{
			BOND_ASSERT(mElement != nullptr);
			return &mElement->GetValue();
		}

		ConstIterator& operator++()
		{
			BOND_ASSERT(mElement != nullptr);
			mElement = mElement->GetNext();
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator temp = *this;
			++*this;
			return temp;
		}

	private:
		friend class AutoStack;

		explicit ConstIterator(const Element *element) noexcept: mElement(element) {}

		const Element *mElement;
	};


	AutoStack() noexcept: mTop(nullptr) {}

	~AutoStack()
	{
		// Live elements would keep a reference to this destroyed stack.
		assert(mTop == nullptr);
	}

	AutoStack(const AutoStack &other) = delete;
	AutoStack &operator=(const AutoStack &other) = delete;


	// Discarding the result destroys the Element immediately, which pops the value again.
	template<typename... Args>
	[[nodiscard]] Element Push(Args&&... args)
	{
		return Element(*this, Bond::forward<Args>(args)...);
	}


	ValueType &GetTop()
	{
		BOND_ASSERT(mTop != nullptr);
		return mTop->GetValue();
	}

	const ValueType &GetTop() const
	{
		BOND_ASSERT(mTop != nullptr);
		return mTop->GetValue();
	}


	void SetTop(const ValueType &value)
	{
		BOND_ASSERT(mTop != nullptr);
		mTop->SetValue(value);
	}


	void SetTop(ValueType &&value)
	{
		BOND_ASSERT(mTop != nullptr);
		mTop->SetValue(Bond::move(value));
	}


	bool IsEmpty() const noexcept { return mTop == nullptr; }


	bool Contains(const ValueType &value) const
	{
		const Element *element = mTop;
		while (element != nullptr)
		{
			if (element->GetValue() == value)
			{
				return true;
			}
			element = element->GetNext();
		}
		return false;
	}

	Iterator begin() noexcept { return Iterator(mTop); }
	ConstIterator begin() const noexcept { return ConstIterator(mTop); }
	ConstIterator cbegin() const noexcept { return ConstIterator(mTop); }
	Iterator end() noexcept { return Iterator(nullptr); }
	ConstIterator end() const noexcept { return ConstIterator(nullptr); }
	ConstIterator cend() const noexcept { return ConstIterator(nullptr); }

private:
	void PushElement(Element *element) noexcept
	{
		// Store pointer to local Element object. Safe because the Element's destructor
		// restores mTop before the pointer is accessed after the Element's lifetime ends.
		// This is an intentional RAII pattern where the Element manages its own stack
		// membership via constructor/destructor.
#if defined(__clang__)
# if defined(__has_warning)
#  if __has_warning("-Wdangling-pointer")
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdangling-pointer"
#  endif
# endif
#elif defined(__GNUC__) && (__GNUC__ >= 12)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif
		element->SetNext(mTop);
		mTop = element;
#if defined(__clang__)
# if defined(__has_warning)
#  if __has_warning("-Wdangling-pointer")
#   pragma clang diagnostic pop
#  endif
# endif
#elif defined(__GNUC__) && (__GNUC__ >= 12)
# pragma GCC diagnostic pop
#endif
	}


	Element *mTop;
};


typedef AutoStack<bool> BoolStack;
typedef AutoStack<int32_t> IntStack;
typedef AutoStack<uint32_t> UIntStack;
typedef AutoStack<size_t> SizeStack;

}

#endif
