#ifndef BOND_COMPILER_STREAMPOS_H
#define BOND_COMPILER_STREAMPOS_H

#include "bond/types/types.h"

namespace Bond
{

/// \brief A POD struct to contain the index, line and column of a token in source code.
///
/// The index is a byte offset into the UTF-8 source. Lines and columns are 1-based, and columns
/// count code points rather than bytes, so they match what an editor shows for most text. Stray
/// UTF-8 continuation bytes, which are invalid, do not advance the column.
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
