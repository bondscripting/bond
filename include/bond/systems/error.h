#ifndef BOND_SYSTEMS_ERROR_H
#define BOND_SYSTEMS_ERROR_H

/// \file
/// \brief Runtime error detection macros that are always enabled.
///
/// - BOND_CHECK(condition), BOND_CHECK_MESSAGE(condition, message) and
///   BOND_CHECK_FORMAT(condition, format, ...) throw a Bond::Exception when the condition is false.
/// - BOND_ERROR(), BOND_ERROR_MESSAGE(message) and BOND_ERROR_FORMAT(format, ...) unconditionally
///   throw a Bond::Exception.
///
/// These macros are intended for conditions that depend on program input, such as malformed CBO
/// files, unresolved names or files that fail to open, and are never compiled out. For catching
/// logic errors during development, use the debugging asserts in bond/systems/assert.h instead.
///
/// The default implementation can be replaced by defining USER_BOND_ERROR_HEADER.

#include "bond/conf.h"

#ifdef USER_BOND_ERROR_HEADER

#include USER_BOND_ERROR_HEADER

#else

#include "bond/private/error.h"

#endif

#endif
