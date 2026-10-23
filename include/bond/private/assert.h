#ifndef BOND_PRIVATE_ASSERT_H
#define BOND_PRIVATE_ASSERT_H

#include "bond/conf.h"

namespace Bond
{

/// \brief Reports a failed debugging assert to stderr and aborts the program. Invoked by the
/// BOND_ASSERT family of macros.
/// \param file The source file containing the failed assert.
/// \param line The line number of the failed assert.
/// \param condition The text of the failed condition, or nullptr if there is none.
/// \param format An optional printf-style format for an additional message, or nullptr.
[[noreturn]] BOND_API void AssertionFailed(const char *file, int line, const char *condition, const char *format, ...);

}

#if BOND_ASSERTS_ENABLED

#define BOND_ASSERT(condition)                                                \
  do                                                                          \
  {                                                                           \
    if (!(condition))                                                         \
    {                                                                         \
      Bond::AssertionFailed(__FILE__, __LINE__, #condition, nullptr);         \
    }                                                                         \
  } while (false)


#define BOND_ASSERT_MESSAGE(condition, message)                               \
  do                                                                          \
  {                                                                           \
    if (!(condition))                                                         \
    {                                                                         \
      Bond::AssertionFailed(__FILE__, __LINE__, #condition, "%s", (message)); \
    }                                                                         \
  } while (false)


#define BOND_ASSERT_FORMAT(condition, ...)                                    \
  do                                                                          \
  {                                                                           \
    if (!(condition))                                                         \
    {                                                                         \
      Bond::AssertionFailed(__FILE__, __LINE__, #condition, __VA_ARGS__);     \
    }                                                                         \
  } while (false)


#define BOND_ASSERT_FAIL() Bond::AssertionFailed(__FILE__, __LINE__, nullptr, nullptr)

#else

// The condition is kept in an unevaluated sizeof so that it is still type checked.
#define BOND_ASSERT(condition) do { static_cast<void>(sizeof(!(condition))); } while (false)
#define BOND_ASSERT_MESSAGE(condition, message) BOND_ASSERT(condition)
#define BOND_ASSERT_FORMAT(condition, ...) BOND_ASSERT(condition)
#define BOND_ASSERT_FAIL() do {} while (false)

#endif

#endif
