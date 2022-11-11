#ifndef BOND_COMPILER_TYPEANDVALUE_H
#define BOND_COMPILER_TYPEANDVALUE_H

#include "bond/stl/stringview.h"
#include "bond/types/value.h"

namespace Bond
{

class TypeDescriptor;

/// \brief Contains the information regarding the type and value of an expression as it is
/// evaluated by the compiler.
///
/// \ingroup compiler
class TypeAndValue
{
public:
	explicit TypeAndValue(TypeDescriptor &descriptor):
		mTypeDescriptor(descriptor),
		mValueDefined(false),
		mResolved(false)
	{}

	TypeAndValue(TypeDescriptor &descriptor, const Value &value):
		mValue(value),
		mTypeDescriptor(descriptor),
		mValueDefined(true),
		mResolved(true)
	{}

	bool IsTypeDefined() const;

	TypeDescriptor &GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

	bool IsValueDefined() const { return mValueDefined; }

	const Value &GetValue() const { return mValue; }
	void SetValue(const Value &value) { mValue = value; mValueDefined = true; }

	bool GetBoolValue() const { return mValue.mBool; }
	void SetBoolValue(bool value) { mValue.mBool = value; mValueDefined = true; }

	float GetFloatValue() const { return mValue.mFloat; }
	void SetFloatValue(float value) { mValue.mFloat = value; mValueDefined = true; }

	double GetDoubleValue() const { return mValue.mDouble; }
	void SetDoubleValue(double value) { mValue.mDouble = value; mValueDefined = true; }

	int32_t GetIntValue() const { return mValue.mInt; }
	void SetIntValue(int32_t value) { mValue.mInt = value; mValueDefined = true; }

	uint32_t GetUIntValue() const { return mValue.mUInt; }
	void SetUIntValue(uint32_t value) { mValue.mUInt = value; mValueDefined = true; }

	int64_t GetLongValue() const { return mValue.mLong; }
	void SetLongValue(int64_t value) { mValue.mLong = value; mValueDefined = true; }
	int64_t AsLongValue() const;

	uint64_t GetULongValue() const { return mValue.mULong; }
	void SetULongValue(uint64_t value) { mValue.mULong = value; mValueDefined = true; }
	uint64_t AsULongValue() const;

	const StringView GetStringValue() const { return StringView(mValue.mString.buffer, mValue.mString.length); }

	bool IsResolved() const { return mResolved; }
	void Resolve() { mResolved = true; }

private:
	Value mValue;
	TypeDescriptor &mTypeDescriptor;
	bool mValueDefined;
	bool mResolved;
};

}

#endif
