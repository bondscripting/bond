#ifndef BOND_PRIVATE_ERROR_H
#define BOND_PRIVATE_ERROR_H

#include "bond/systems/exception.h"

#define BOND_CHECK(condition)  \
  do                           \
  {                            \
    if (!(condition))          \
    {                          \
      throw Bond::Exception(); \
    }                          \
  } while (false)


#define BOND_CHECK_MESSAGE(condition, message)  \
  do                                            \
  {                                             \
    if (!(condition))                           \
    {                                           \
      throw Bond::Exception("%s", (message));   \
    }                                           \
  } while (false)


#define BOND_CHECK_FORMAT(condition, ...)       \
  do                                            \
  {                                             \
    if (!(condition))                           \
    {                                           \
      throw Bond::Exception(__VA_ARGS__);       \
    }                                           \
  } while (false)


#define BOND_ERROR() throw Bond::Exception()
#define BOND_ERROR_MESSAGE(message) throw Bond::Exception("%s", (message))
#define BOND_ERROR_FORMAT(...) throw Bond::Exception(__VA_ARGS__)

#endif
