#ifndef BOND_PRIVATE_CSTDLIB_H
#define BOND_PRIVATE_CSTDLIB_H

#include "bond/types/types.h"
#include <cerrno>
#include <cmath>
#include <cstdlib>

namespace Bond
{

static_assert(sizeof(long long) == sizeof(int64_t), "long long is not 64 bits");


inline int64_t ParseInt64(const char *str, const char **end, int base, bool *outOfRange)
{
	char *parseEnd = nullptr;
	errno = 0;
	const long long value = std::strtoll(str, &parseEnd, base);
	if (end != nullptr)
	{
		*end = parseEnd;
	}
	if (outOfRange != nullptr)
	{
		*outOfRange = errno == ERANGE;
	}
	return int64_t(value);
}


inline uint64_t ParseUInt64(const char *str, const char **end, int base, bool *outOfRange)
{
	char *parseEnd = nullptr;
	errno = 0;
	const unsigned long long value = std::strtoull(str, &parseEnd, base);
	if (end != nullptr)
	{
		*end = parseEnd;
	}
	if (outOfRange != nullptr)
	{
		*outOfRange = errno == ERANGE;
	}
	return uint64_t(value);
}


inline float ParseFloat(const char *str, const char **end, bool *outOfRange)
{
	char *parseEnd = nullptr;
	errno = 0;
	const float value = std::strtof(str, &parseEnd);
	if (end != nullptr)
	{
		*end = parseEnd;
	}
	if (outOfRange != nullptr)
	{
		// Underflow also sets ERANGE, but yields a value of small magnitude.
		*outOfRange = (errno == ERANGE) && (std::fabs(value) == HUGE_VALF);
	}
	return value;
}


inline double ParseDouble(const char *str, const char **end, bool *outOfRange)
{
	char *parseEnd = nullptr;
	errno = 0;
	const double value = std::strtod(str, &parseEnd);
	if (end != nullptr)
	{
		*end = parseEnd;
	}
	if (outOfRange != nullptr)
	{
		// Underflow also sets ERANGE, but yields a value of small magnitude.
		*outOfRange = (errno == ERANGE) && (std::fabs(value) == HUGE_VAL);
	}
	return value;
}

}

#endif
