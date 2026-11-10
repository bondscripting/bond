#ifndef BOND_STL_CSTDLIB_H
#define BOND_STL_CSTDLIB_H

/// \file
/// \brief Conversions from text to numbers, used by the lexer and Bond's string standard library.
///
/// The default implementation forwards to strtoll, strtoull, strtof and strtod from the C++
/// standard library, which depend on the C locale (LC_NUMERIC). It can be replaced, e.g. with a
/// locale-independent implementation, by defining USER_BOND_CSTDLIB_HEADER. A replacement must
/// declare the following functions in the Bond namespace with the semantics described here:
///
/// - int64_t ParseInt64(const char *str, const char **end, int base, bool *outOfRange)
/// - uint64_t ParseUInt64(const char *str, const char **end, int base, bool *outOfRange)
/// - float ParseFloat(const char *str, const char **end, bool *outOfRange)
/// - double ParseDouble(const char *str, const char **end, bool *outOfRange)
///
/// Each function parses the longest prefix of str that forms a number, as the corresponding strto*
/// function does. If end is not null, *end is set to the first character that was not parsed, or to
/// str if no number was parsed. If outOfRange is not null, *outOfRange is set to whether the
/// magnitude of the number is too large to be represented, in which case the integer functions
/// return the closest representable value and the floating point ones return a signed infinity.
/// Floating point underflow is not reported.

#include "bond/conf.h"

#ifdef USER_BOND_CSTDLIB_HEADER

#include USER_BOND_CSTDLIB_HEADER

#else

#include "bond/private/cstdlib.h"

#endif

#endif
