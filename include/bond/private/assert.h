#ifndef PRIVATE_BOND_ASSERT_H
#define PRIVATE_BOND_ASSERT_H

#include "bond/systems/exception.h"

#define BOND_ASSERT(condition) \
  if (!(condition))            \
  {                            \
    throw Bond::Exception();   \
  }                            \


#define BOND_ASSERT_MESSAGE(condition, message) \
  if (!(condition))                             \
  {                                             \
    throw Bond::Exception(message);             \
  }                                             \


#define BOND_ASSERT_FORMAT(condition, format)   \
  if (!(condition))                             \
  {                                             \
    throw Bond::Exception format;               \
  }                                             \


#define BOND_FAIL() throw Bond::Exception()
#define BOND_FAIL_MESSAGE(message) throw Bond::Exception(message)
#define BOND_FAIL_FORMAT(format) throw Bond::Exception format

#endif
