#ifndef BOND_VALUE_H
#define BOND_VALUE_H

#include "bond/conf.h"

namespace Bond
{

union Value
{
	Value(): mInt(0) {}
	//Value(int_t value): mInt(value) {}
	//Value(uint_t value): mUInt(value) {}
	//Value(float_t value): mFloat(value) {}
	int_t mInt;
	uint_t mUInt;
	float_t mFloat;
};

}

#endif

