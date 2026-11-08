#ifndef BOND_CONF_H
#define BOND_CONF_H

#include <cstddef>
#include <new>

#if defined(BOND_SHARED) || defined(BOND_BUILD_SHARED)
  #if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(BOND_BUILD_SHARED)
      #define BOND_API __declspec(dllexport)
    #else
      #define BOND_API __declspec(dllimport)
    #endif
    #define BOND_LOCAL
  #else
    #if defined(__GNUC__) && __GNUC__ >= 4
      #define BOND_API   __attribute__((visibility("default")))
      #define BOND_LOCAL __attribute__((visibility("hidden")))
    #else
      #define BOND_API
      #define BOND_LOCAL
    #endif
  #endif
#else
  #define BOND_API
  #define BOND_LOCAL
#endif

#if defined(_MSC_VER)
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#endif

#ifdef USER_BOND_CONFIG_HEADER
#include USER_BOND_CONFIG_HEADER
#endif

#ifndef BOND_RUNTIME_CHECKS_ENABLED
#define BOND_RUNTIME_CHECKS_ENABLED 1
#endif

#ifndef BOND_ASSERTS_ENABLED
#define BOND_ASSERTS_ENABLED 1
#endif

// Bond only supports little endian targets. C++17 has no standard way to detect the byte order at
// compile time, so rely on compiler-specific predefined macros.
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
  #if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    #error Bond only supports little endian targets.
  #endif
  #if defined(__FLOAT_WORD_ORDER__) && (__FLOAT_WORD_ORDER__ != __ORDER_LITTLE_ENDIAN__)
    #error Bond requires floating point values to be stored in little endian byte order.
  #endif
#elif defined(_MSC_VER)
  // All targets supported by MSVC are little endian.
#else
  #error Unable to determine the target byte order. Bond only supports little endian targets.
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

#endif // BOND_CONF_H
