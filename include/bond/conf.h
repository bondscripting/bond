#ifndef BOND_CONF_H
#define BOND_CONF_H

#include <cstddef>
#include <new>

#ifdef USER_BOND_CONFIG_HEADER
#include USER_BOND_CONFIG_HEADER
#endif

#ifndef BOND_RUNTIME_CHECKS_ENABLED
#define BOND_RUNTIME_CHECKS_ENABLED 1
#endif

#if !defined(BOND_LITTLE_ENDIAN) && !defined(BOND_BIG_ENDIAN)
#define BOND_LITTLE_ENDIAN
#endif

#if !defined(BOND_USE_32BIT_POINTERS) && !defined(BOND_USE_64BIT_POINTERS)
#define BOND_USE_32BIT_POINTERS
#endif

#ifndef BOND_PATH_SEPARATOR
#ifdef _WIN32

#define BOND_PATH_SEPARATOR "\\"
#define BOND_PATH_SEPARATOR_CHAR '\\'

#else

#define BOND_PATH_SEPARATOR "/"
#define BOND_PATH_SEPARATOR_CHAR '/'

#endif // _WIN32
#endif // BOND_PATH_SEPARATOR

#ifndef BOND_FILESYSTEM_PREFIX
#define BOND_FILESYSTEM_PREFIX ""
#endif

namespace Bond
{
	void ValidateConfiguration();
}

#endif // BOND_CONF_H
