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
	LoadLibIo(cboLoader);
	LoadLibMath(cboLoader);
	LoadLibMemory(cboLoader);
	LoadLibString(cboLoader);
	LoadLibType(cboLoader);
}


void LoadLibIo(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(IO_BINDING_COLLECTION);
	cboLoader.AddCboFile(IO_CBO.GetData(), IO_CBO.GetLength());
}


void LoadLibMath(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(MATH_BINDING_COLLECTION);
	cboLoader.AddCboFile(MATH_CBO.GetData(), MATH_CBO.GetLength());
}


void LoadLibMemory(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(MEMORY_BINDING_COLLECTION);
	cboLoader.AddCboFile(MEMORY_CBO.GetData(), MEMORY_CBO.GetLength());
}


void LoadLibString(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(STRING_BINDING_COLLECTION);
	cboLoader.AddCboFile(STRING_CBO.GetData(), STRING_CBO.GetLength());
}


void LoadLibType(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(TYPE_BINDING_COLLECTION);
	cboLoader.AddCboFile(TYPE_CBO.GetData(), TYPE_CBO.GetLength());
}

}
