#ifndef BOND_API_LIBRUNTIME_H
#define BOND_API_LIBRUNTIME_H

#include "bond/conf.h"

namespace Bond
{

class CboLoader;

void LoadAllLibs(CboLoader &cboLoader);
void LoadLibIo(CboLoader &cboLoader);
void LoadLibMath(CboLoader &cboLoader);
void LoadLibMemory(CboLoader &cboLoader);
void LoadLibString(CboLoader &cboLoader);
void LoadLibType(CboLoader &cboLoader);

}

#endif
