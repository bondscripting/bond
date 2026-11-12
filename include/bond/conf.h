#ifndef BOND_CONF_H
#define BOND_CONF_H

/// \file
/// \brief Compile-time configuration of Bond.
///
/// The configuration macros below can be overridden by defining them on the command line, or in a
/// header named by the USER_BOND_CONFIG_HEADER macro, which conf.h includes before giving them
/// their default values. Bond only supports little endian targets.

#include <cstddef>
#include <new>

/// \def BOND_API
/// \brief Marks a class or function as part of the public interface of the Bond library.
///
/// When Bond is built as a shared library (BOND_BUILD_SHARED is defined), it exports the symbol.
/// When Bond is used as a shared library (BOND_SHARED is defined), it imports the symbol on
/// Windows. Otherwise, it expands to nothing.

/// \def BOND_LOCAL
/// \brief Marks a class or function as internal to the Bond library. It hides the symbol when
/// Bond is built or used as a shared library with GCC or Clang, and expands to nothing otherwise.

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
/// \brief Whether the VM validates the use of its API at runtime, such as the types and number
/// of arguments passed to and read from functions. Violations throw a Bond::Exception. Defaults
/// to 1.
#define BOND_RUNTIME_CHECKS_ENABLED 1
#endif

#ifndef BOND_ASSERTS_ENABLED
/// \brief Whether the BOND_ASSERT* macros are enabled. Defaults to 1.
/// \sa assert
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

/// \brief The separator between the elements of a file path, as a string literal. Defaults to
/// "\\" on Windows and "/" elsewhere. If it is overridden, BOND_PATH_SEPARATOR_CHAR must be
/// overridden as well.
#define BOND_PATH_SEPARATOR "\\"

/// \brief The separator between the elements of a file path, as a character literal.
#define BOND_PATH_SEPARATOR_CHAR '\\'

#else

#define BOND_PATH_SEPARATOR "/"
#define BOND_PATH_SEPARATOR_CHAR '/'

#endif // _WIN32
#endif // BOND_PATH_SEPARATOR

#ifndef BOND_FILESYSTEM_PREFIX
/// \brief A string literal prepended to every file name opened by a StdioStreamFactory. Defaults
/// to "".
#define BOND_FILESYSTEM_PREFIX ""
#endif

#ifndef BOND_MAX_PATH_LENGTH
/// \brief Maximum length of a file path in bytes, including the terminating null. Longer paths
/// fail to open. Defaults to 1024.
#define BOND_MAX_PATH_LENGTH 1024
#endif

#endif // BOND_CONF_H
