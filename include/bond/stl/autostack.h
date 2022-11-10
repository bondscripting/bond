#ifndef BOND_STL_AUTOSTACK_H
#define BOND_STL_AUTOSTACK_H

#include "bond/stl/utility.h"
#include "bond/types/types.h"

namespace Bond
{

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
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		template<typename... Args>
		Element(AutoStack &stack, Args&&... args):
			mValue(forward<Args>(args)...),
			mStack(stack),
			mNext(nullptr)
		{
			mStack.PushElement(this);
		}

		~Element()
		{
			mStack.PopElement();
		}

		Element(const Element &other) = delete;
		Element &operator=(const Element &other) = delete;

		Element &operator=(const ValueType &value) { mValue = value; return *this; }
		Element &operator=(ValueType &&value) { mValue = move(value); return *this; }

		operator const ValueType&() const { return mValue; }
		operator ValueType&() { return mValue; }

		const ValueType &GetValue() const  { return mValue; }
		ValueType &GetValue() { return mValue; }

		void SetValue(const ValueType &value) { mValue = value; }
		void SetValue(ValueType &&value) { mValue = move(value); }

	protected:
		Element *GetNext() { return mNext; }
		const Element *GetNext() const { return mNext; }

		ValueType mValue;

	private:
		friend class AutoStack;
		friend class Iterator;
		friend class ConstIterator;

		void SetNext(Element *next) { mNext = next; }

		AutoStack &mStack;
		Element *mNext;
	};


	class Iterator
	{
	public:
		Iterator(): mElement(nullptr) {}
		explicit Iterator(Element *element): mElement(element) {}

		bool operator==(const Iterator& other) const { return mElement == other.mElement; }
		bool operator!=(const Iterator& other) const { return mElement != other.mElement; }
		ValueType &operator*() const { return mElement->GetValue(); }
		ValueType *operator->() const { return &mElement->GetValue(); }

		Iterator& operator++()
		{
			mElement = mElement->GetNext();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator temp = *this;
			mElement = mElement->GetNext();
			return temp;
		}

	private:
		Element *mElement;
	};


	class ConstIterator
	{
	public:
		ConstIterator(): mElement(nullptr) {}
		explicit ConstIterator(const Element *element): mElement(element) {}
		explicit ConstIterator(const Iterator &iterator): mElement(iterator.mElement) {}

		bool operator==(const ConstIterator& other) const { return mElement == other.mElement; }
		bool operator!=(const ConstIterator& other) const { return mElement != other.mElement; }
		const ValueType &operator*() const { return mElement->GetValue(); }
		const ValueType *operator->() const { return &mElement->GetValue(); }

		ConstIterator& operator++()
		{
			mElement = mElement->GetNext();
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator temp = *this;
			mElement = mElement->GetNext();
			return temp;
		}

	private:
		const Element *mElement;
	};


	AutoStack(): mTop(nullptr) {}


	template<typename... Args>
	Element Push(Args&&... args)
	{
		return Element(*this, forward<Args>(args)...);
	}


	ValueType &GetTop() { return mTop->GetValue(); }
	const ValueType &GetTop() const { return mTop->GetValue(); }


	void SetTop(const ValueType &value)
	{
		if (mTop != nullptr)
		{
			mTop->SetValue(value);
		}
	}


	void SetTop(ValueType &&value)
	{
		if (mTop != nullptr)
		{
			mTop->SetValue(move(value));
		}
	}


	bool IsEmpty() const { return mTop == nullptr; }


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

	Iterator begin() { return Iterator(mTop); }
	ConstIterator begin() const { return ConstIterator(mTop); }
	ConstIterator cbegin() const { return ConstIterator(mTop); }
	Iterator end() { return Iterator(nullptr); }
	ConstIterator end() const { return ConstIterator(nullptr); }
	ConstIterator cend() const { return ConstIterator(nullptr); }

private:
	void PushElement(Element *element)
	{
		element->SetNext(mTop);
		mTop = element;
	}


	void PopElement()
	{
		if (mTop != nullptr)
		{
			mTop = mTop->GetNext();
		}
	}


	Element *mTop;
};


typedef AutoStack<bool> BoolStack;
typedef AutoStack<int32_t> IntStack;
typedef AutoStack<uint32_t> UIntStack;
typedef AutoStack<size_t> SizeStack;

}

#endif
