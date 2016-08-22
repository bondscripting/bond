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
	cboLoader.AddCboFile(IO_CBO.mData, IO_CBO.mLength);
}


void LoadLibMath(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(MATH_BINDING_COLLECTION);
	cboLoader.AddCboFile(MATH_CBO.mData, MATH_CBO.mLength);
}


void LoadLibMemory(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(MEMORY_BINDING_COLLECTION);
	cboLoader.AddCboFile(MEMORY_CBO.mData, MEMORY_CBO.mLength);
}


void LoadLibString(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(STRING_BINDING_COLLECTION);
	cboLoader.AddCboFile(STRING_CBO.mData, STRING_CBO.mLength);
}


void LoadLibType(CboLoader &cboLoader)
{
	cboLoader.AddNativeBinding(TYPE_BINDING_COLLECTION);
	cboLoader.AddCboFile(TYPE_CBO.mData, TYPE_CBO.mLength);
}

}
