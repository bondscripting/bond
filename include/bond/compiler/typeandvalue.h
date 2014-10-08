#ifndef BOND_COMPILER_TYPEANDVALUE_H
#define BOND_COMPILER_TYPEANDVALUE_H

#include "bond/types/hashedstring.h"
#include "bond/types/value.h"

namespace Bond
{

class TypeDescriptor;

class TypeAndValue
{
public:
	TypeAndValue(): mTypeDescriptor(nullptr), mValueDefined(false), mResolved(false) {}

	TypeAndValue(TypeDescriptor *descriptor):
		mTypeDescriptor(descriptor),
		mValueDefined(false),
		mResolved(false)
	{}

	TypeAndValue(TypeDescriptor *descriptor, const Value &value):
		mValue(value),
		mTypeDescriptor(descriptor),
		mValueDefined(true),
		mResolved(true)
	{}

	TypeAndValue &operator=(const TypeAndValue &other)
	{
		mValue = other.mValue;
		mTypeDescriptor = other.mTypeDescriptor;
		mValueDefined = other.mValueDefined;
		mResolved = other.mResolved;
		return *this;
	}

	bool IsTypeDefined() const;

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	void SetTypeDescriptor(TypeDescriptor *descriptor) { mTypeDescriptor = descriptor; }

	bool IsValueDefined() const { return mValueDefined; }

	const Value &GetValue() const { return mValue; }
	void SetValue(const Value &value) { mValue = value; mValueDefined = true; }

	bool GetBoolValue() const { return mValue.mBool; }
	void SetBoolValue(bool value) { mValue.mBool = value; mValueDefined = true; }

	float GetFloatValue() const { return mValue.mFloat; }
	void SetFloatValue(float value) { mValue.mFloat = value; mValueDefined = true; }

	double GetDoubleValue() const { return mValue.mDouble; }
	void SetDoubleValue(double value) { mValue.mDouble = value; mValueDefined = true; }

	bi32_t GetIntValue() const { return mValue.mInt; }
	void SetIntValue(bi32_t value) { mValue.mInt = value; mValueDefined = true; }

	bu32_t GetUIntValue() const { return mValue.mUInt; }
	void SetUIntValue(bu32_t value) { mValue.mUInt = value; mValueDefined = true; }

	bi64_t GetLongValue() const { return mValue.mLong; }
	void SetLongValue(bi64_t value) { mValue.mLong = value; mValueDefined = true; }
	bi64_t AsLongValue() const;

	bu64_t GetULongValue() const { return mValue.mULong; }
	void SetULongValue(bu64_t value) { mValue.mULong = value; mValueDefined = true; }
	bu64_t AsULongValue() const;

	const SimpleString GetStringValue() const { return SimpleString(mValue.mString.buffer, mValue.mString.length); }
	const HashedString GetHashedStringValue() const { return HashedString(mValue.mString.buffer, mValue.mString.length); }

	bool IsResolved() const { return mResolved; }
	void Resolve() { mResolved = true; }

private:
	Value mValue;
	TypeDescriptor *mTypeDescriptor;
	bool mValueDefined;
	bool mResolved;
};

}

#endif
