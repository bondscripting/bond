#ifndef BOND_API_LIBRUNTIME_H
#define BOND_API_LIBRUNTIME_H

#include "bond/api/libio.h"
#include "bond/api/libmath.h"
#include "bond/api/libmemory.h"
#include "bond/api/libstring.h"
#include "bond/api/libtype.h"
#include "bond/io/filedata.h"

namespace Bond
{

class CboLoader;

#include "bond/private/libruntime_embedded.h"

void LoadAllLibs(CboLoader &cboLoader);
void LoadLibIo(CboLoader &cboLoader);
void LoadLibMath(CboLoader &cboLoader);
void LoadLibMemory(CboLoader &cboLoader);
void LoadLibString(CboLoader &cboLoader);
void LoadLibType(CboLoader &cboLoader);

}

#endif
