#ifndef BOND_PRIVATE_ASSERT_H
#define BOND_PRIVATE_ASSERT_H

#include "bond/systems/exception.h"

#define BOND_STATIC_ASSERT(condition, message)                  \
  typedef char assert_failed_ ## message[(condition) ? 1 : -1]; \

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
#define BOND_FAIL_MESSAGE(message) throw Bond::Exception("%s", (message))
#define BOND_FAIL_FORMAT(format) throw Bond::Exception format

#endif
