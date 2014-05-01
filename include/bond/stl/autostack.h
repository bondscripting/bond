#ifndef BOND_STL_AUTOSTACK_H
#define BOND_STL_AUTOSTACK_H

#include "bond/types/types.h"

namespace Bond
{

template <typename ElementType>
class AutoStackIterator
{
};

template <typename ElementType>
class AutoStack
{
public:
	class Element
	{
	public:
		explicit Element(AutoStack &stack):
			mStack(stack),
			mNext(NULL)
		{
			mStack.Push(this);
		}

		Element(AutoStack &stack, const ElementType &value):
			mValue(value),
			mStack(stack),
			mNext(NULL)
		{
			mStack.Push(this);
		}

		~Element()
		{
			mStack.Pop();
		}

		Element &operator=(const ElementType &value) { mValue = value; return *this; }

		operator const ElementType&() const { return mValue; }

		ElementType &GetValue() { return mValue; }
		const ElementType &GetValue() const  { return mValue; }
		void SetValue(const ElementType &value) { mValue = value; }

	protected:
		Element *GetNext() { return mNext; }
		const Element *GetNext() const { return mNext; }

		ElementType mValue;

	private:
		Element(const Element &other);
		friend class AutoStack;

		void SetNext(Element *next) { mNext = next; }

		AutoStack &mStack;
		Element *mNext;
	};

	class Iterator
	{
	public:
		Iterator(): mElement(NULL) {}
		explicit Iterator(Element *element): mElement(element) {}

		bool operator==(const Iterator& other) const { return mElement == other.mElement; }
		bool operator!=(const Iterator& other) const { return mElement != other.mElement; }
		ElementType &operator*() const { return mElement->GetValue(); }
		ElementType *operator->() const { return &mElement->GetValue(); }

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
		ConstIterator(): mElement(NULL) {}
		explicit ConstIterator(const Element *element): mElement(element) {}
		ConstIterator(const Iterator &iterator): mElement(iterator.mElement) {}

		bool operator==(const ConstIterator& other) const { return mElement == other.mElement; }
		bool operator!=(const ConstIterator& other) const { return mElement != other.mElement; }
		const ElementType &operator*() const { return mElement->GetValue(); }
		const ElementType *operator->() const { return &mElement->GetValue(); }

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


	AutoStack(): mTop(NULL) {}


	void Push(Element *element)
	{
		element->SetNext(mTop);
		mTop = element;
	}


	void Pop()
	{
		if (mTop != NULL)
		{
			mTop = mTop->GetNext();
		}
	}


	ElementType &GetTop() { return mTop->GetValue(); }
	const ElementType &GetTop() const { return mTop->GetValue(); }


	void SetTop(const ElementType &value)
	{
		if (mTop != NULL)
		{
			mTop->SetValue(value);
		}
	}


	bool IsEmpty() const { return mTop == NULL; }


	bool Contains(const ElementType &value) const
	{
		const Element *element = mTop;
		while (element != NULL)
		{
			if (element->GetValue() == value)
			{
				return true;
			}
			element = element->GetNext();
		}
		return false;
	}

	Iterator Begin() { return Iterator(mTop); }
	ConstIterator Begin() const { return ConstIterator(mTop); }
	Iterator End() { return Iterator(NULL); }
	ConstIterator End() const { return ConstIterator(NULL); }

private:
	Element *mTop;
};


typedef AutoStack<bool> BoolStack;
typedef AutoStack<bi32_t> IntStack;
typedef AutoStack<bu32_t> UIntStack;
typedef AutoStack<size_t> SizeStack;

}

#endif
