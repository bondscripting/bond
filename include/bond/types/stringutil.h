#ifndef BOND_TYPES_STRINGUTIL_H
#define BOND_TYPES_STRINGUTIL_H

#include "bond/types/types.h"

namespace Bond
{

class SimpleString;
class OutputStream;

size_t StringLength(const char *str);

bool StringEqual(const char *str1, const char *str2, size_t length);
inline bool StringEqual(const char *str1, size_t length1, const char *str2, size_t length2)
{
	return
		(length1 == length2) &&
		((str1 == str2) ||
		 StringEqual(str1, str2, length1));
}

int StringCompare(const char *str1, const char *str2, size_t length);
int StringCompare(const char *str1, size_t length1, const char *str2, size_t length2);

}

#endif
