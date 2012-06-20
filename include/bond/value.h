#ifndef BOND_VALUE_H
#define BOND_VALUE_H

#include "bond/types.h"
#include "bond/simplestring.h"

namespace Bond
{

union Value
{
	Value(): mInt(0) {}
	explicit Value(bool value): mBool(value) {}
	explicit Value(bi32_t value): mInt(value) {}
	explicit Value(bu32_t value): mUInt(value) {}
	explicit Value(bi64_t value): mLong(value) {}
	explicit Value(bu64_t value): mULong(value) {}
	explicit Value(bf32_t value): mFloat(value) {}
	explicit Value(bf64_t value): mDouble(value) {}

	bool mBool;
	bi32_t mInt;
	bu32_t mUInt;
	bi64_t mLong;
	bu64_t mULong;
	bf32_t mFloat;
	bf64_t mDouble;

	struct
	{
		const char *buffer;
		size_t length;
	} mString;
};


union Value16
{
	Value16(): mShort(0) {}
	explicit Value16(bi16_t value): mShort(value) {}
	explicit Value16(bu16_t value): mUShort(value) {}
	explicit Value16(bi32_t value): mShort(static_cast<bi16_t>(value)) {}
	explicit Value16(bu32_t value): mUShort(static_cast<bu16_t>(value)) {}

	explicit Value16(const bu8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
	}

	bi16_t mShort;
	bu16_t mUShort;
	unsigned char mBytes[2];
};


union Value32
{
	Value32(): mInt(0) {}
	explicit Value32(bi32_t value): mInt(value) {}
	explicit Value32(bu32_t value): mUInt(value) {}
	explicit Value32(bf32_t value): mFloat(value) {}

	explicit Value32(const bu8_t value[])
	{
		mBytes[0] = value[0];
		mBytes[1] = value[1];
		mBytes[2] = value[2];
		mBytes[3] = value[3];
	}

	bi32_t mInt;
	bu32_t mUInt;
	bf32_t mFloat;
	bu8_t mBytes[4];
};


union Value64
{
	Value64(): mLong(0) {}
	explicit Value64(bi64_t value): mLong(value) {}
	explicit Value64(bu64_t value): mULong(value) {}
	explicit Value64(bf64_t value): mDouble(value) {}

	explicit Value64(const bu8_t value[])
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

	bi64_t mLong;
	bu64_t mULong;
	bf64_t mDouble;
	bu8_t mBytes[8];
};


inline bool operator <(const Value16 &a, const Value16 &b) { return a.mUShort < b.mUShort; }
inline bool operator <(const Value32 &a, const Value32 &b) { return a.mUInt < b.mUInt; }
inline bool operator <(const Value64 &a, const Value64 &b) { return a.mULong < b.mULong; }

}

#endif
