#ifndef BOND_STL_AUTOSTACK_H
#define BOND_STL_AUTOSTACK_H

#include "bond/stl/utility.h"
#include "bond/systems/assert.h"
#include "bond/types/types.h"

namespace Bond
{

/// \brief A stack whose elements are automatic variables.
///
/// Pushing a value onto the stack returns an Element that must be kept alive for as long as the
/// value should remain on the stack; destroying the Element pops the value. Elements must be
/// destroyed in reverse order of creation and must not outlive the stack. The stack cannot be
/// copied or moved.
///
/// \tparam ValueType The type of the values held by the stack.
template <typename ValueType>
class AutoStack
{
public:
	class Iterator;
	class ConstIterator;

	/// \brief A single value on an AutoStack, owned by the scope that pushed it.
	///
	/// An Element is pushed onto its stack when it is constructed and popped when it is destroyed.
	/// An Element cannot be copied or moved.
	class Element
	{
	public:
		/// \brief Constructs an Element holding a default constructed value and pushes it onto the
		///        given stack.
		/// \param stack The stack onto which the Element is pushed. It must outlive the Element.
		explicit Element(AutoStack &stack):
			mValue(),
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		/// \brief Constructs an Element holding a value constructed from the given arguments and
		///        pushes it onto the given stack.
		/// \param stack The stack onto which the Element is pushed. It must outlive the Element.
		/// \param args The arguments forwarded to the constructor of the value.
		template<typename... Args>
		Element(AutoStack &stack, Args&&... args):
			mValue(Bond::forward<Args>(args)...),
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		/// \brief Pops the Element from its stack. The Element must be at the top of the stack.
		~Element()
		{
			BOND_ASSERT(mStack.mTop == this);
			mStack.mTop = mNext;
		}

		Element(const Element &other) = delete;
		Element &operator=(const Element &other) = delete;

		/// \brief Assigns a copy of the given value to the Element.
		/// \param value The value to be copied.
		/// \returns A reference to this Element.
		Element &operator=(const ValueType &value) { mValue = value; return *this; }

		/// \brief Moves the given value into the Element.
		/// \param value The value to be moved.
		/// \returns A reference to this Element.
		Element &operator=(ValueType &&value) { mValue = Bond::move(value); return *this; }

		/// \brief Implicitly converts the Element to a const reference to its value.
		operator const ValueType&() const noexcept { return mValue; }

		/// \brief Implicitly converts the Element to a reference to its value.
		operator ValueType&() noexcept { return mValue; }

		/// \brief Returns a const reference to the value held by the Element.
		const ValueType &GetValue() const noexcept { return mValue; }

		/// \brief Returns a reference to the value held by the Element.
		ValueType &GetValue() noexcept { return mValue; }

		/// \brief Assigns a copy of the given value to the Element.
		/// \param value The value to be copied.
		void SetValue(const ValueType &value) { mValue = value; }

		/// \brief Moves the given value into the Element.
		/// \param value The value to be moved.
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


	/// \brief An iterator that visits the values of an AutoStack from the top to the bottom.
	class Iterator
	{
	public:
		/// \brief Constructs an Iterator that does not refer to any value.
		Iterator() noexcept: mElement(nullptr) {}

		/// \brief Returns whether two Iterators refer to the same value.
		bool operator==(const Iterator& other) const noexcept { return mElement == other.mElement; }

		/// \brief Returns whether two Iterators refer to different values.
		bool operator!=(const Iterator& other) const noexcept { return mElement != other.mElement; }

		/// \brief Returns a reference to the value to which the Iterator refers.
		ValueType &operator*() const
		{
			BOND_ASSERT(mElement != nullptr);
			return mElement->GetValue();
		}

		/// \brief Returns a pointer to the value to which the Iterator refers.
		ValueType *operator->() const
		{
			BOND_ASSERT(mElement != nullptr);
			return &mElement->GetValue();
		}

		/// \brief Advances the Iterator to the next value down the stack.
		/// \returns A reference to this Iterator.
		Iterator& operator++()
		{
			BOND_ASSERT(mElement != nullptr);
			mElement = mElement->GetNext();
			return *this;
		}

		/// \brief Advances the Iterator to the next value down the stack.
		/// \returns A copy of the Iterator from before it was advanced.
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


	/// \brief An iterator that visits the values of a const AutoStack from the top to the bottom.
	class ConstIterator
	{
	public:
		/// \brief Constructs a ConstIterator that does not refer to any value.
		ConstIterator() noexcept: mElement(nullptr) {}

		/// \brief Constructs a ConstIterator that refers to the same value as the given Iterator.
		/// \param iterator The Iterator from which the ConstIterator is constructed.
		ConstIterator(const Iterator &iterator) noexcept: mElement(iterator.mElement) {}

		// Hidden friends so that mixed Iterator/ConstIterator comparisons work in either order.

		/// \brief Returns whether two ConstIterators refer to the same value.
		friend bool operator==(const ConstIterator& a, const ConstIterator& b) noexcept { return a.mElement == b.mElement; }

		/// \brief Returns whether two ConstIterators refer to different values.
		friend bool operator!=(const ConstIterator& a, const ConstIterator& b) noexcept { return a.mElement != b.mElement; }

		/// \brief Returns a const reference to the value to which the ConstIterator refers.
		const ValueType &operator*() const
		{
			BOND_ASSERT(mElement != nullptr);
			return mElement->GetValue();
		}

		/// \brief Returns a const pointer to the value to which the ConstIterator refers.
		const ValueType *operator->() const
		{
			BOND_ASSERT(mElement != nullptr);
			return &mElement->GetValue();
		}

		/// \brief Advances the ConstIterator to the next value down the stack.
		/// \returns A reference to this ConstIterator.
		ConstIterator& operator++()
		{
			BOND_ASSERT(mElement != nullptr);
			mElement = mElement->GetNext();
			return *this;
		}

		/// \brief Advances the ConstIterator to the next value down the stack.
		/// \returns A copy of the ConstIterator from before it was advanced.
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


	/// \brief Constructs an empty AutoStack.
	AutoStack() noexcept: mTop(nullptr) {}

	/// \brief Destroys the AutoStack. All Elements must have been destroyed beforehand.
	~AutoStack()
	{
		// Live elements would keep a reference to this destroyed stack.
		BOND_ASSERT(mTop == nullptr);
	}

	AutoStack(const AutoStack &other) = delete;
	AutoStack &operator=(const AutoStack &other) = delete;


	/// \brief Pushes a value onto the top of the stack.
	/// \param args The arguments forwarded to the constructor of the value.
	/// \returns An Element that keeps the value on the stack for as long as it is alive. Discarding
	///        the result destroys the Element immediately, which pops the value again.
	template<typename... Args>
	[[nodiscard]] Element Push(Args&&... args)
	{
		return Element(*this, Bond::forward<Args>(args)...);
	}


	/// \brief Returns a reference to the value at the top of the stack. The stack must not be empty.
	ValueType &GetTop()
	{
		BOND_ASSERT(mTop != nullptr);
		return mTop->GetValue();
	}

	/// \brief Returns a const reference to the value at the top of the stack. The stack must not be
	///        empty.
	const ValueType &GetTop() const
	{
		BOND_ASSERT(mTop != nullptr);
		return mTop->GetValue();
	}


	/// \brief Assigns a copy of the given value to the value at the top of the stack. The stack
	///        must not be empty.
	/// \param value The value to be copied.
	void SetTop(const ValueType &value)
	{
		BOND_ASSERT(mTop != nullptr);
		mTop->SetValue(value);
	}


	/// \brief Moves the given value into the value at the top of the stack. The stack must not be
	///        empty.
	/// \param value The value to be moved.
	void SetTop(ValueType &&value)
	{
		BOND_ASSERT(mTop != nullptr);
		mTop->SetValue(Bond::move(value));
	}


	/// \brief Returns whether the stack contains no values.
	bool IsEmpty() const noexcept { return mTop == nullptr; }


	/// \brief Returns whether any value on the stack compares equal to the given value.
	/// \param value The value to search for.
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

	/// \brief Returns an Iterator referring to the value at the top of the stack.
	Iterator begin() noexcept { return Iterator(mTop); }

	/// \brief Returns a ConstIterator referring to the value at the top of the stack.
	ConstIterator begin() const noexcept { return ConstIterator(mTop); }

	/// \brief Returns a ConstIterator referring to the value at the top of the stack.
	ConstIterator cbegin() const noexcept { return ConstIterator(mTop); }

	/// \brief Returns an Iterator referring to the position past the value at the bottom of the stack.
	Iterator end() noexcept { return Iterator(nullptr); }

	/// \brief Returns a ConstIterator referring to the position past the value at the bottom of the stack.
	ConstIterator end() const noexcept { return ConstIterator(nullptr); }

	/// \brief Returns a ConstIterator referring to the position past the value at the bottom of the stack.
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


/// \brief An AutoStack of bool values.
typedef AutoStack<bool> BoolStack;

/// \brief An AutoStack of int32_t values.
typedef AutoStack<int32_t> IntStack;

/// \brief An AutoStack of uint32_t values.
typedef AutoStack<uint32_t> UIntStack;

/// \brief An AutoStack of size_t values.
typedef AutoStack<size_t> SizeStack;

}

#endif
