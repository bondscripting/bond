#ifndef BOND_VALUE_H
#define BOND_VALUE_H

#include "bond/conf.h"

namespace Bond
{

union Value
{
	Value(): mInt(0) {}
	bool mBool;
	int_t mInt;
	uint_t mUInt;
	float_t mFloat;
};

}

#endif

