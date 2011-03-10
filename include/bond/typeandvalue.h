#ifndef BOND_TYPEANDVALUE_H
#define BOND_TYPEANDVALUE_H

#include "bond/value.h"

namespace Bond
{

class TypeDescriptor;

class TypeAndValue
{
public:
	TypeAndValue(): mTypeDescriptor(0), mValueDefined(false), mResolved(false) {}

	TypeAndValue(const TypeDescriptor *descriptor):
		mTypeDescriptor(descriptor),
		mValueDefined(false),
		mResolved(false) {}

	TypeAndValue(const TypeDescriptor *descriptor, const Value value):
		mValue(value),
		mTypeDescriptor(descriptor),
		mValueDefined(true),
		mResolved(true)
	{}

	TypeAndValue(const TypeAndValue &other):
		mValue(other.mValue),
		mTypeDescriptor(other.mTypeDescriptor),
		mValueDefined(other.mValueDefined),
		mResolved(other.mResolved)
	{}

	const TypeAndValue &operator=(const TypeAndValue &other)
	{
		mValue = other.mValue;
		mTypeDescriptor = other.mTypeDescriptor;
		mValueDefined = other.mValueDefined;
		mResolved = other.mResolved;
		return *this;
	}

	bool IsTypeDefined() const { return mTypeDescriptor != 0; }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	void SetTypeDescriptor(const TypeDescriptor *descriptor) { mTypeDescriptor = descriptor; }

	bool IsValueDefined() const { return mValueDefined; }

	const Value &GetValue() const { return mValue; }
	void SetValue(const Value &value) { mValue = value; mValueDefined = true; }

	bool GetBoolValue() const { return mValue.mBool; }
	void SetBoolValue(bool value) { mValue.mBool = value; }

	char GetCharValue() const { return mValue.mChar; }
	void SetCharValue(char value) { mValue.mChar = value; }

	bf32_t GetFloatValue() const { return mValue.mFloat; }
	void SetFloatValue(bf32_t value) { mValue.mFloat = value; }

	bi32_t GetIntValue() const { return mValue.mInt; }
	void SetIntValue(bi32_t value) { mValue.mInt = value; }

	bu32_t GetUIntValue() const { return mValue.mUInt; }
	void SetUIntValue(bu32_t value) { mValue.mUInt = value; }

	const char *GetStringValue() const { return mValue.mString.buffer; }
	int GetStringLength() const { return mValue.mString.length; }
	void SetStringValue(const char *buffer, int length);

	bool IsResolved() const { return mResolved; }
	void Resolve() { mResolved = true; }

private:
	Value mValue;
	const TypeDescriptor *mTypeDescriptor;
	bool mValueDefined;
	bool mResolved;
};

}

#endif
