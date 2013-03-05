#ifndef BOND_STREAMPOS_H
#define BOND_STREAMPOS_H

#include "bond/types/types.h"

namespace Bond
{

struct StreamPos
{
	StreamPos():
		index(0),
		line(1),
		column(1)
	{
	}

	StreamPos(size_t i, size_t l, size_t c):
		index(i),
		line(l),
		column(c)
	{
	}

	StreamPos(const StreamPos &other):
		index(other.index),
		line(other.line),
		column(other.column)
	{
	}

	size_t index;
	size_t line;
	size_t column;
};

}

#endif
