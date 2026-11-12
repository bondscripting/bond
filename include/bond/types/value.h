#ifndef BOND_TYPES_VALUE_H
#define BOND_TYPES_VALUE_H

#include "bond/types/types.h"
#include <limits>

namespace Bond
{

/// \addtogroup types
/// @{

/// \brief Union of all value types representable by a Token in the Bond compiler.
union Value
{
	/// \brief Constructs a Value object whose mInt member is 0.
	Value(): mInt(0) {}

	/// \brief Constructs a Value object holding a bool in mBool.
	explicit Value(bool value): mBool(value) {}

	/// \brief Constructs a Value object holding an int32_t in mInt.
	explicit Value(int32_t value): mInt(value) {}

	/// \brief Constructs a Value object holding a uint32_t in mUInt.
	explicit Value(uint32_t value): mUInt(value) {}

	/// \brief Constructs a Value object holding an int64_t in mLong.
	explicit Value(int64_t value): mLong(value) {}

	/// \brief Constructs a Value object holding a uint64_t in mULong.
	explicit Value(uint64_t value): mULong(value) {}

	/// \brief Constructs a Value object holding a float in mFloat.
	explicit Value(float value): mFloat(value) {}

	/// \brief Constructs a Value object holding a double in mDouble.
	explicit Value(double value): mDouble(value) {}

	/// \brief The value as a bool.
	bool mBool;

	/// \brief The value as a signed 32-bit integer.
	int32_t mInt;

	/// \brief The value as an unsigned 32-bit integer.
	uint32_t mUInt;

	/// \brief The value as a signed 64-bit integer.
	int64_t mLong;

	/// \brief The value as an unsigned 64-bit integer.
	uint64_t mULong;

	/// \brief The value as a float.
	float mFloat;

	/// \brief The value as a double.
	double mDouble;

	/// \brief The value as a string, which is not null terminated.
	struct
	{
		/// \brief Pointer to the characters of the string.
		const char *buffer;

		/// \brief The number of characters in the string.
		size_t length;
	} mString;
};


/// \brief Union of all 16 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value16
{
	/// \brief Constructs a Value16 object whose value is 0.
	Value16(): mShort(0) {}

	/// \brief Constructs a Value16 object holding an int16_t.
	explicit Value16(int16_t value): mShort(value) {}

	/// \brief Constructs a Value16 object holding a uint16_t.
	explicit Value16(uint16_t value): mUShort(value) {}

	/// \brief Constructs a Value16 object holding an int32_t truncated to 16 bits.
	explicit Value16(int32_t value): mShort(static_cast<int16_t>(value)) {}

	/// \brief Constructs a Value16 object holding a uint32_t truncated to 16 bits.
	explicit Value16(uint32_t value): mUShort(static_cast<uint16_t>(value)) {}

	/// \brief Constructs a Value16 object from the first 2 bytes of the given array, in memory
	/// order.
	explicit Value16(const uint8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
	}

	/// \brief The value as a signed 16-bit integer.
	int16_t mShort;

	/// \brief The value as an unsigned 16-bit integer.
	uint16_t mUShort;

	/// \brief The bytes of the value in memory order.
	unsigned char mBytes[2];
};


/// \brief Union of all 32 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value32
{
	/// \brief Constructs a Value32 object whose value is 0.
	Value32(): mInt(0) {}

	/// \brief Constructs a Value32 object holding an int32_t.
	explicit Value32(int32_t value): mInt(value) {}

	/// \brief Constructs a Value32 object holding a uint32_t.
	explicit Value32(uint32_t value): mUInt(value) {}

	/// \brief Constructs a Value32 object holding a float.
	explicit Value32(float value): mFloat(value) {}

	/// \brief Constructs a Value32 object from the first 4 bytes of the given array, in memory
	/// order.
	explicit Value32(const uint8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
		mBytes[2] = value[2];
		mBytes[3] = value[3];
	}

	/// \brief The value as a signed 32-bit integer.
	int32_t mInt;

	/// \brief The value as an unsigned 32-bit integer.
	uint32_t mUInt;

	/// \brief The value as a float.
	float mFloat;

	/// \brief The bytes of the value in memory order.
	uint8_t mBytes[4];
};


/// \brief Union of all 64 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value64
{
	/// \brief Constructs a Value64 object whose value is 0.
	Value64(): mLong(0) {}

	/// \brief Constructs a Value64 object holding an int64_t.
	explicit Value64(int64_t value): mLong(value) {}

	/// \brief Constructs a Value64 object holding a uint64_t.
	explicit Value64(uint64_t value): mULong(value) {}

	/// \brief Constructs a Value64 object holding a double.
	explicit Value64(double value): mDouble(value) {}

	/// \brief Constructs a Value64 object from the first 8 bytes of the given array, in memory
	/// order.
	explicit Value64(const uint8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
		mBytes[2] = value[2];
		mBytes[3] = value[3];
		mBytes[4] = value[4];
		mBytes[5] = value[5];
		mBytes[6] = value[6];
		mBytes[7] = value[7];
	}

	/// \brief The value as a signed 64-bit integer.
	int64_t mLong;

	/// \brief The value as an unsigned 64-bit integer.
	uint64_t mULong;

	/// \brief The value as a double.
	double mDouble;

	/// \brief The bytes of the value in memory order.
	uint8_t mBytes[8];
};

static_assert(sizeof(Value16) == 2, "sizeof(Value16) is not 2");
static_assert(sizeof(Value32) == 4, "sizeof(Value32) is not 4");
static_assert(sizeof(Value64) == 8, "sizeof(Value64) is not 8");
static_assert(std::numeric_limits<float>::is_iec559, "The representation of type 'float' does not appear to be in IEEE binary32 format.");
static_assert(std::numeric_limits<double>::is_iec559, "The representation of type 'double' does not appear to be in IEEE binary64 format.");


/// \brief Returns the result of the expression: <code>a.mUShort < b.mUShort</code>.
inline bool operator <(const Value16 &a, const Value16 &b) { return a.mUShort < b.mUShort; }

/// \brief Returns the result of the expression: <code>a.mUInt < b.mUInt</code>.
inline bool operator <(const Value32 &a, const Value32 &b) { return a.mUInt < b.mUInt; }

/// \brief Returns the result of the expression: <code>a.mULong < b.mULong</code>.
inline bool operator <(const Value64 &a, const Value64 &b) { return a.mULong < b.mULong; }

/// @}

}

#endif
