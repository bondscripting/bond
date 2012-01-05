#ifndef BOND_VALUE_H
#define BOND_VALUE_H

#include "bond/conf.h"

namespace Bond
{

union Value
{
	Value(): mInt(0) {}
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

}

#endif

