#ifndef BOND_API_LIBRUNTIME_H
#define BOND_API_LIBRUNTIME_H

#include "bond/conf.h"

namespace Bond
{

class CboLoader;

BOND_API void LoadAllLibs(CboLoader &cboLoader);
BOND_API void LoadLibIo(CboLoader &cboLoader);
BOND_API void LoadLibMath(CboLoader &cboLoader);
BOND_API void LoadLibMemory(CboLoader &cboLoader);
BOND_API void LoadLibString(CboLoader &cboLoader);
BOND_API void LoadLibType(CboLoader &cboLoader);

}

#endif
