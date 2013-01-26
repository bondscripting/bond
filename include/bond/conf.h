#ifndef BOND_CONF_H
#define BOND_CONF_H

#include <stddef.h>
#include <new>

#ifndef BOND_RUNTIME_CHECKS_ENABLED
#define BOND_RUNTIME_CHECKS_ENABLED 1
#endif

#ifdef USER_BOND_CONFIG_HEADER

#include USER_BOND_CONFIG_HEADER

#endif

#endif
