#include "bond/api/libruntime.h"
#include "bond/vm/cboloader.h"

namespace Bond
{

#if defined(BOND_USE_32BIT_POINTERS)
#include "private/libruntime32_embedded.h"
#else
#include "private/libruntime64_embedded.h"
#endif

void LoadAllLibs(CboLoader &cboLoader)
{
	LoadLibMath(cboLoader);
	LoadLibString(cboLoader);
}


void LoadLibMath(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(MATH_BINDING_COLLECTION);
	cboLoader.AddCboFile(MATH_CBO);
}


void LoadLibString(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(STRING_BINDING_COLLECTION);
	cboLoader.AddCboFile(STRING_CBO);
}

}
