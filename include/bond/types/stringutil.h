#ifndef BOND_TYPES_STRINGUTIL_H
#define BOND_TYPES_STRINGUTIL_H

#include "bond/types/types.h"
#include <cstring>

namespace Bond
{

/// \addtogroup types
/// @{

/// \brief Returns the result of the expression: <code>(str == nullptr) ? 0 : strlen(str)</code>.
inline size_t StringLength(const char *str) { return (str == nullptr) ? 0 : strlen(str); }

/// \brief Returns whether two strings of the same specified length are equal. The strings may
/// have embedded null characters.
bool StringEqual(const char *str1, const char *str2, size_t length);

/// \brief Returns whether two strings of specified lengths are equal. The strings may have
/// embedded null characters.
inline bool StringEqual(const char *str1, size_t length1, const char *str2, size_t length2)
{
	return
		(length1 == length2) &&
		((str1 == str2) || StringEqual(str1, str2, length1));
}

/// \brief Compares two strings of specified lengths and returns a negative value, zero, or
/// positive value if the first string is less than, equal to, or greater than the second.
int StringCompare(const char *str1, size_t length1, const char *str2, size_t length2);

/// @}

}

#endif
