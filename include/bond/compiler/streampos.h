#ifndef BOND_COMPILER_STREAMPOS_H
#define BOND_COMPILER_STREAMPOS_H

#include "bond/types/types.h"

namespace Bond
{

/// \brief A POD struct to contain the index, line and column of a token in source code.
///
/// \sa CharStream
/// \ingroup compiler
struct StreamPos
{
	StreamPos(size_t i = 0, size_t l = 1, size_t c = 1): index(i), line(l), column(c) {}
	StreamPos(const StreamPos &other) = default;

	size_t index;
	size_t line;
	size_t column;
};

}

#endif
