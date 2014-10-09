#include "bond/io/outputstream.h"
#include "bond/types/simplestring.h"
#include "bond/types/stringutil.h"

namespace Bond
{

size_t StringLength(const char *str)
{
	size_t length = 0;
	if (str != nullptr)
	{
		const char *s = str;
		while (*s != '\0')
		{
			++s;
		}
		length = size_t(s - str);
	}
	return length;
}


uint32_t StringHash(const char *str, uint32_t seed)
{
	const unsigned char *s = reinterpret_cast<const unsigned char *>(str);
	uint32_t hash = seed;

	while (*s != '\0')
	{
		hash = (hash * 0x01000193u) ^ uint32_t(*s++);
	}

	return hash;
}


uint32_t StringHash(size_t length, const char *str, uint32_t seed)
{
	const unsigned char *s = reinterpret_cast<const unsigned char *>(str);
	const unsigned char *end = s + length;
	uint32_t hash = seed;

	while (s < end)
	{
		hash = (hash * 0x01000193u) ^ uint32_t(*s++);
	}

	return hash;
}


bool StringEqual(const char *str1, const char *str2, size_t length)
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


int StringCompare(const char *str1, const char *str2, size_t length)
{
	const char *a = str1;
	const char *b = str2;
	const char *end = str1 + length;
	int result = 0;
	while ((result == 0) && (a < end))
	{
		const int c1 = *a++;
		const int c2 = *b++;
		result = c1 - c2;
	}
	return result;
}


int StringCompare(const char *str1, size_t length1, const char *str2, size_t length2)
{
	const char *a = str1;
	const char *b = str2;
	const char *end1 = str1 + length1;
	const char *end2 = str2 + length2;
	int result = 0;
	while ((result == 0) && (a < end1) && (b < end2))
	{
		const int c1 = *a++;
		const int c2 = *b++;
		result = c1 - c2;
	}
	return (result != 0) ? result : ((length1 < length2) ? -1 : (length1 > length2) ? 1 : 0);
}


void PrintString(OutputStream &stream, const SimpleString &str)
{
	const size_t length = str.GetLength();
	const char *s = str.GetString();;
	for (size_t i = 0; i < length; ++i)
	{
		stream.Print("%c", s[i]);
	}
}

}
