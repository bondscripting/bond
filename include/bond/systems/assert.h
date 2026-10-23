#ifndef BOND_SYSTEMS_ASSERT_H
#define BOND_SYSTEMS_ASSERT_H

/// \file
/// \brief Debugging asserts for catching logic errors during development.
///
/// - BOND_ASSERT(condition), BOND_ASSERT_MESSAGE(condition, message) and
///   BOND_ASSERT_FORMAT(condition, format, ...) report the failure to stderr and abort the program
///   when the condition is false.
/// - BOND_ASSERT_FAIL() unconditionally reports a failure and aborts, e.g. for unreachable code.
///
/// These macros are only enabled when BOND_ASSERTS_ENABLED is non-zero (the default) and compile to
/// nothing otherwise. They do not throw, so they are safe to use in destructors and noexcept
/// functions. Conditions that depend on program input must instead be validated with the always
/// enabled macros in bond/systems/error.h.
///
/// The default implementation can be replaced by defining USER_BOND_ASSERT_HEADER.

#include "bond/conf.h"

#ifdef USER_BOND_ASSERT_HEADER

#include USER_BOND_ASSERT_HEADER

#else

#include "bond/private/assert.h"

#endif

#endif
