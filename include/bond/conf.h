#ifndef BOND_CONF_H
#define BOND_CONF_H

#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#include <stddef.h>
#include <new>

#ifdef USER_BOND_CONFIG_HEADER

#include USER_BOND_CONFIG_HEADER

#endif // USER_BOND_CONFIG_HEADER

#ifndef BOND_RUNTIME_CHECKS_ENABLED
#define BOND_RUNTIME_CHECKS_ENABLED 1
#endif

#ifndef BOND_PATH_SEPARATOR
#ifdef _WIN32

#define BOND_PATH_SEPARATOR "/"
#define BOND_PATH_SEPARATOR_CHAR '/'

#else

#define BOND_PATH_SEPARATOR "/"
#define BOND_PATH_SEPARATOR_CHAR '/'

#endif // _WIN32
#endif // BOND_PATH_SEPARATOR

#endif // BOND_CONF_H
