#ifndef BOND_API_LIBRUMTIME_H
#define BOND_API_LIBRUNTIME_H

#include "bond/api/libmath.h"
#include "bond/api/libstring.h"
#include "bond/io/filedata.h"

namespace Bond
{

class CboLoader;

#if defined(BOND_USE_32BIT_POINTERS)
#include "bond/private/libruntime32_embedded.h"
#else
#include "bond/private/libruntime64_embedded.h"
#endif

void LoadAllLibs(CboLoader &cboLoader);
void LoadLibMath(CboLoader &cboLoader);
void LoadLibString(CboLoader &cboLoader);

}

#endif
