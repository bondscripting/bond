#ifndef BOND_STREAMPOS_H
#define BOND_STREAMPOS_H

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

	StreamPos(int index, int line, int column):
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

	int index;
	int line;
	int column;
};

}

#endif
