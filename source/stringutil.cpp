#include "bond/stringutil.h"

namespace Bond
{

bu32_t StringHash(const char *str, bu32_t seed)
{
	const unsigned char *s = reinterpret_cast<const unsigned char *>(str);
	bu32_t hash = seed;

	while (*s != '\0')
	{
		hash = (hash * 0x01000193u) ^ static_cast<bu32_t>(*s++);
	}

	return hash;
}


bu32_t StringHash(int length, const char *str, bu32_t seed)
{
	const unsigned char *s = reinterpret_cast<const unsigned char *>(str);
	const unsigned char *end = s + length;
	bu32_t hash = seed;

	while (s < end)
	{
		hash = (hash * 0x01000193u) ^ static_cast<bu32_t>(*s++);
	}

	return hash;
}


bool StringEqual(const char *str1, const char *str2, int length)
{
	const char *a = str1;
	const char *b = str2;
	const char *end = str1 + length;
	bool equal = true;
	while (equal && (a < end))
	{
		equal = (*a++ == *b++);
	}
	return equal;
}

}
