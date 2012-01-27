#ifndef BOND_VALUE_H
#define BOND_VALUE_H

#include "bond/types.h"

namespace Bond
{

union Value
{
	Value(): mInt(0) {}
	explicit Value(bool value): mBool(value) {}
	explicit Value(bi32_t value): mInt(value) {}
	explicit Value(bu32_t value): mUInt(value) {}
	explicit Value(bf32_t value): mFloat(value) {}
	bool mBool;
	bi32_t mInt;
	bu32_t mUInt;
	bf32_t mFloat;
	struct
	{
		const char *buffer;
		int length;
	} mString;
};


union Value16
{
	explicit Value16(bi16_t value): mShort(value) {}
	explicit Value16(bu16_t value): mUShort(value) {}
	bi16_t mShort;
	bu16_t mUShort;
	unsigned char mBytes[4];
};


union Value32
{
	explicit Value32(bi32_t value): mInt(value) {}
	explicit Value32(bu32_t value): mUInt(value) {}
	explicit Value32(bf32_t value): mFloat(value) {}
	bi32_t mInt;
	bu32_t mUInt;
	bf32_t mFloat;
	unsigned char mBytes[4];
};

}

#endif

