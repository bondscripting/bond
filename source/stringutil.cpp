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

}
