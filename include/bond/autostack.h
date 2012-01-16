#ifndef BOND_AUTOSTACK_H
#define BOND_AUTOSTACK_H

#include "bond/types.h"

namespace Bond
{

template<typename ElementType>
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

		Element(AutoStack &stack, const Element &other):
			mValue(other.mValue),
			mStack(stack),
			mNext(NULL)
		{
			mStack.Push(this);
		}

		Element(const Element &other):
			mValue(other.mValue),
			mStack(other.mStack),
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

		const ElementType &GetValue() const  { return mValue; }
		void SetValue(const ElementType &value) { mValue = value; }

	private:
		friend class AutoStack;

		Element *GetNext() { return mNext; }
		const Element *GetNext() const { return mNext; }
		void SetNext(Element *next) { mNext = next; }

		ElementType mValue;
		AutoStack &mStack;
		Element *mNext;
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

private:

	Element *mTop;
};


typedef AutoStack<bool> BoolStack;
typedef AutoStack<bi32_t> IntStack;

}

#endif
