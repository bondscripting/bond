#ifndef BOND_PRIVATE_ASSERT_H
#define BOND_PRIVATE_ASSERT_H

#include "bond/systems/exception.h"

#define BOND_ASSERT(condition) \
  do                           \
  {                            \
    if (!(condition))          \
    {                          \
      throw Bond::Exception(); \
    }                          \
  } while (false)


#define BOND_ASSERT_MESSAGE(condition, message) \
  do                                            \
  {                                             \
    if (!(condition))                           \
    {                                           \
      throw Bond::Exception(message);           \
    }                                           \
  } while (false)


#define BOND_ASSERT_FORMAT(condition, ...)      \
  do                                            \
  {                                             \
    if (!(condition))                           \
    {                                           \
      throw Bond::Exception(__VA_ARGS__);       \
    }                                           \
  } while (false)


#define BOND_FAIL() throw Bond::Exception()
#define BOND_FAIL_MESSAGE(message) throw Bond::Exception("%s", (message))
#define BOND_FAIL_FORMAT(...) throw Bond::Exception(__VA_ARGS__)

#endif
