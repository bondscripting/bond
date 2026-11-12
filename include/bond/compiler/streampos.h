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
	/// \brief Constructs a StreamPos object.
	/// \param i The byte offset into the source.
	/// \param l The 1-based line number.
	/// \param c The 1-based column number.
	StreamPos(size_t i = 0, size_t l = 1, size_t c = 1): index(i), line(l), column(c) {}

	/// \brief Constructs a copy of another StreamPos.
	StreamPos(const StreamPos &other) = default;

	/// \brief The byte offset into the source.
	size_t index;

	/// \brief The 1-based line number.
	size_t line;

	/// \brief The 1-based column number, counted in code points.
	size_t column;
};

}

#endif
