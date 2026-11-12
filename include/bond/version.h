#ifndef BOND_VERSION_H
#define BOND_VERSION_H

/// \file
/// \brief Version numbers of Bond and of the CBO file format.

#include "bond/types/types.h"

/// \brief The major version of Bond.
#define BOND_MAJOR_VERSION 0

/// \brief The minor version of Bond.
#define BOND_MINOR_VERSION 9

/// \brief The patch version of Bond.
#define BOND_PATCH_VERSION 0

/// \brief Implementation detail of BOND_STRINGIFY.
#define BOND_STRINGIFY_DETAIL(x) #x

/// \brief Expands the given macro argument and converts the result to a string literal.
#define BOND_STRINGIFY(x) BOND_STRINGIFY_DETAIL(x)

/// \brief The version of Bond as a string literal of the form "major.minor.patch".
#define BOND_VERSION_STRING_LITERAL BOND_STRINGIFY(BOND_MAJOR_VERSION) "." BOND_STRINGIFY(BOND_MINOR_VERSION) "." BOND_STRINGIFY(BOND_PATCH_VERSION)

namespace Bond
{

/// \brief The number with which every CBO file begins.
constexpr uint32_t MAGIC_NUMBER = 0x57EDFA57;

/// \brief The major version of Bond.
constexpr uint16_t MAJOR_VERSION = BOND_MAJOR_VERSION;

/// \brief The minor version of Bond.
constexpr uint16_t MINOR_VERSION = BOND_MINOR_VERSION;

/// \brief The patch version of Bond.
constexpr uint16_t PATCH_VERSION = BOND_PATCH_VERSION;

/// \brief The major version of the CBO file format written by the CodeGenerator.
constexpr uint16_t CBO_MAJOR_VERSION = 1;

/// \brief The minor version of the CBO file format written by the CodeGenerator.
constexpr uint16_t CBO_MINOR_VERSION = 0;

/// \brief The major version of the oldest supported CBO file format.
///
/// CBO files whose major version is between CBO_MIN_SUPPORTED_MAJOR_VERSION and
/// CBO_MAX_SUPPORTED_MAJOR_VERSION can be loaded, regardless of their minor version.
/// \sa IsCBOFormatLoadable, IsCBOFormatSupported
constexpr uint16_t CBO_MIN_SUPPORTED_MAJOR_VERSION = 1;

/// \brief The minor version of the oldest fully supported CBO file format.
/// \sa IsCBOFormatSupported
constexpr uint16_t CBO_MIN_SUPPORTED_MINOR_VERSION = 0;

/// \brief The major version of the newest supported CBO file format.
/// \sa IsCBOFormatLoadable, IsCBOFormatSupported
constexpr uint16_t CBO_MAX_SUPPORTED_MAJOR_VERSION = 1;

/// \brief The minor version of the newest fully supported CBO file format.
/// \sa IsCBOFormatSupported
constexpr uint16_t CBO_MAX_SUPPORTED_MINOR_VERSION = 0;

/// \brief The version of Bond as a string of the form "major.minor.patch".
inline constexpr const char *VERSION_STRING = BOND_VERSION_STRING_LITERAL;

}

#endif
