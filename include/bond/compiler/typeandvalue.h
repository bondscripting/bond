#ifndef BOND_COMPILER_TYPEANDVALUE_H
#define BOND_COMPILER_TYPEANDVALUE_H

#include "bond/conf.h"
#include "bond/stl/stringview.h"
#include "bond/types/value.h"

namespace Bond
{

class TypeDescriptor;

/// \brief Contains the information regarding the type and value of an expression as it is
/// evaluated by the compiler.
///
/// The value is only meaningful when it is defined, i.e. when the expression is a compile-time
/// constant. The getters and setters for specific value types access the corresponding member of
/// the Value union without any conversion, so they must match the type described by the
/// TypeDescriptor.
///
/// \ingroup compiler
class BOND_API TypeAndValue
{
public:
	/// \brief Constructs a TypeAndValue object with an undefined value that is not yet resolved.
	/// \param descriptor The type of the expression. It is not copied, so it must outlive the
	///        TypeAndValue.
	explicit TypeAndValue(TypeDescriptor &descriptor):
		mTypeDescriptor(descriptor),
		mValueDefined(false),
		mResolved(false)
	{}

	/// \brief Constructs a TypeAndValue object with a defined value that is already resolved.
	/// \param descriptor The type of the expression. It is not copied, so it must outlive the
	///        TypeAndValue.
	/// \param value The value of the expression.
	TypeAndValue(TypeDescriptor &descriptor, const Value &value):
		mValue(value),
		mTypeDescriptor(descriptor),
		mValueDefined(true),
		mResolved(true)
	{}

	/// \brief Returns whether the type is defined, as reported by TypeDescriptor::IsDefined().
	bool IsTypeDefined() const;

	/// \brief Returns the type of the expression.
	TypeDescriptor &GetTypeDescriptor() { return mTypeDescriptor; }

	/// \brief Returns the type of the expression.
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Returns whether the value of the expression is known at compile time.
	bool IsValueDefined() const { return mValueDefined; }

	/// \brief Returns the value of the expression.
	const Value &GetValue() const { return mValue; }

	/// \brief Sets the value of the expression and marks it as defined.
	void SetValue(const Value &value) { mValue = value; mValueDefined = true; }

	/// \brief Returns the value as a bool.
	bool GetBoolValue() const { return mValue.mBool; }

	/// \brief Sets the value to a bool and marks it as defined.
	void SetBoolValue(bool value) { mValue.mBool = value; mValueDefined = true; }

	/// \brief Returns the value as a float.
	float GetFloatValue() const { return mValue.mFloat; }

	/// \brief Sets the value to a float and marks it as defined.
	void SetFloatValue(float value) { mValue.mFloat = value; mValueDefined = true; }

	/// \brief Returns the value as a double.
	double GetDoubleValue() const { return mValue.mDouble; }

	/// \brief Sets the value to a double and marks it as defined.
	void SetDoubleValue(double value) { mValue.mDouble = value; mValueDefined = true; }

	/// \brief Returns the value as a signed 32-bit integer.
	int32_t GetIntValue() const { return mValue.mInt; }

	/// \brief Sets the value to a signed 32-bit integer and marks it as defined.
	void SetIntValue(int32_t value) { mValue.mInt = value; mValueDefined = true; }

	/// \brief Returns the value as an unsigned 32-bit integer.
	uint32_t GetUIntValue() const { return mValue.mUInt; }

	/// \brief Sets the value to an unsigned 32-bit integer and marks it as defined.
	void SetUIntValue(uint32_t value) { mValue.mUInt = value; mValueDefined = true; }

	/// \brief Returns the value as a signed 64-bit integer.
	int64_t GetLongValue() const { return mValue.mLong; }

	/// \brief Sets the value to a signed 64-bit integer and marks it as defined.
	void SetLongValue(int64_t value) { mValue.mLong = value; mValueDefined = true; }

	/// \brief Returns the value converted from its type to a signed 64-bit integer.
	int64_t AsLongValue() const;

	/// \brief Returns the value as an unsigned 64-bit integer.
	uint64_t GetULongValue() const { return mValue.mULong; }

	/// \brief Sets the value to an unsigned 64-bit integer and marks it as defined.
	void SetULongValue(uint64_t value) { mValue.mULong = value; mValueDefined = true; }

	/// \brief Returns the value converted from its type to an unsigned 64-bit integer.
	uint64_t AsULongValue() const;

	/// \brief Returns the value as a string, which is not null terminated.
	const StringView GetStringValue() const { return StringView(mValue.mString.buffer, mValue.mString.length); }

	/// \brief Returns whether the semantic analyzer has finished evaluating the type and value.
	bool IsResolved() const { return mResolved; }

	/// \brief Marks the type and value as resolved.
	void Resolve() { mResolved = true; }

private:
	Value mValue;
	TypeDescriptor &mTypeDescriptor;
	bool mValueDefined;
	bool mResolved;
};

}

#endif
