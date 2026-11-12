#ifndef BOND_API_LIBRUNTIME_H
#define BOND_API_LIBRUNTIME_H

#include "bond/conf.h"

namespace Bond
{

class CboLoader;

/// \addtogroup api
/// @{

/// \brief Adds all libraries of the Bond Standard Library to the given CboLoader.
///
/// Equivalent to calling LoadLibIo(), LoadLibMath(), LoadLibMemory(), LoadLibString() and
/// LoadLibType().
/// \param cboLoader The CboLoader to which the libraries are added.
BOND_API void LoadAllLibs(CboLoader &cboLoader);

/// \brief Adds the compiled code and native bindings of the io library to the given CboLoader.
/// \param cboLoader The CboLoader to which the library is added.
BOND_API void LoadLibIo(CboLoader &cboLoader);

/// \brief Adds the compiled code and native bindings of the math library to the given CboLoader.
/// \param cboLoader The CboLoader to which the library is added.
BOND_API void LoadLibMath(CboLoader &cboLoader);

/// \brief Adds the compiled code and native bindings of the memory library to the given
/// CboLoader.
/// \param cboLoader The CboLoader to which the library is added.
BOND_API void LoadLibMemory(CboLoader &cboLoader);

/// \brief Adds the compiled code and native bindings of the string library to the given
/// CboLoader.
/// \param cboLoader The CboLoader to which the library is added.
BOND_API void LoadLibString(CboLoader &cboLoader);

/// \brief Adds the compiled code and native bindings of the type library to the given CboLoader.
/// \param cboLoader The CboLoader to which the library is added.
BOND_API void LoadLibType(CboLoader &cboLoader);

/// @}

}

#endif
