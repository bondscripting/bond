#ifndef BOND_TYPES_VALUE_H
#define BOND_TYPES_VALUE_H

#include "bond/types/types.h"

namespace Bond
{

/// \addtogroup types
/// @{

/// \brief Union of all value types representable by a Token in the Bond compiler.
union Value
{
	Value(): mInt(0) {}
	explicit Value(bool value): mBool(value) {}
	explicit Value(int32_t value): mInt(value) {}
	explicit Value(uint32_t value): mUInt(value) {}
	explicit Value(int64_t value): mLong(value) {}
	explicit Value(uint64_t value): mULong(value) {}
	explicit Value(float value): mFloat(value) {}
	explicit Value(double value): mDouble(value) {}

	bool mBool;
	int32_t mInt;
	uint32_t mUInt;
	int64_t mLong;
	uint64_t mULong;
	float mFloat;
	double mDouble;

	struct
	{
		const char *buffer;
		size_t length;
	} mString;
};


/// \brief Union of all 16 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value16
{
	Value16(): mShort(0) {}
	explicit Value16(int16_t value): mShort(value) {}
	explicit Value16(uint16_t value): mUShort(value) {}
	explicit Value16(int32_t value): mShort(static_cast<int16_t>(value)) {}
	explicit Value16(uint32_t value): mUShort(static_cast<uint16_t>(value)) {}

	explicit Value16(const uint8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
	}

	int16_t mShort;
	uint16_t mUShort;
	unsigned char mBytes[2];
};


/// \brief Union of all 32 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value32
{
	Value32(): mInt(0) {}
	explicit Value32(int32_t value): mInt(value) {}
	explicit Value32(uint32_t value): mUInt(value) {}
	explicit Value32(float value): mFloat(value) {}

	explicit Value32(const uint8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
		mBytes[2] = value[2];
		mBytes[3] = value[3];
	}

	int32_t mInt;
	uint32_t mUInt;
	float mFloat;
	uint8_t mBytes[4];
};


/// \brief Union of all 64 bit value types used within the Bond compiler, the CBO file format,
/// and the Bond Virtual Machine.
union Value64
{
	Value64(): mLong(0) {}
	explicit Value64(int64_t value): mLong(value) {}
	explicit Value64(uint64_t value): mULong(value) {}
	explicit Value64(double value): mDouble(value) {}

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

	int64_t mLong;
	uint64_t mULong;
	double mDouble;
	uint8_t mBytes[8];
};


/// \brief Returns the result of the expression: <code>a.mUShort < b.mUShort</code>.
inline bool operator <(const Value16 &a, const Value16 &b) { return a.mUShort < b.mUShort; }

/// \brief Returns the result of the expression: <code>a.mUInt < b.mUInt</code>.
inline bool operator <(const Value32 &a, const Value32 &b) { return a.mUInt < b.mUInt; }

/// \brief Returns the result of the expression: <code>a.mULong < b.mULong</code>.
inline bool operator <(const Value64 &a, const Value64 &b) { return a.mULong < b.mULong; }

/// @}

}

#endif
