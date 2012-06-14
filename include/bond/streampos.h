#ifndef BOND_STREAMPOS_H
#define BOND_STREAMPOS_H

#include "bond/types.h"

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

	StreamPos(size_t index, size_t line, size_t column):
		index(index),
		line(line),
		column(column)
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
