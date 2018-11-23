#include "bond/api/libio.h"
#include "bond/api/libmath.h"
#include "bond/api/libmemory.h"
#include "bond/api/libruntime.h"
#include "bond/api/libstring.h"
#include "bond/api/libtype.h"
#include "bond/types/dataview.h"
#include "bond/vm/cboloader.h"

namespace Bond
{

template<PointerSize size>
typename std::enable_if<size == POINTER_32BIT, const DataView &>::type GetLibIoCbo()
{
#include "private/libruntime32_io_cbo_embedded.h"
	return IO_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_64BIT, const DataView &>::type GetLibIoCbo()
{
#include "private/libruntime64_io_cbo_embedded.h"
	return IO_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_32BIT, const DataView &>::type GetLibMathCbo()
{
#include "private/libruntime32_math_cbo_embedded.h"
	return MATH_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_64BIT, const DataView &>::type GetLibMathCbo()
{
#include "private/libruntime64_math_cbo_embedded.h"
	return MATH_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_32BIT, const DataView &>::type GetLibMemoryCbo()
{
#include "private/libruntime32_memory_cbo_embedded.h"
	return MEMORY_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_64BIT, const DataView &>::type GetLibMemoryCbo()
{
#include "private/libruntime64_memory_cbo_embedded.h"
	return MEMORY_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_32BIT, const DataView &>::type GetLibStringCbo()
{
#include "private/libruntime32_string_cbo_embedded.h"
	return STRING_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_64BIT, const DataView &>::type GetLibStringCbo()
{
#include "private/libruntime64_string_cbo_embedded.h"
	return STRING_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_32BIT, const DataView &>::type GetLibTypeCbo()
{
#include "private/libruntime32_type_cbo_embedded.h"
	return TYPE_CBO;
}


template<PointerSize size>
typename std::enable_if<size == POINTER_64BIT, const DataView &>::type GetLibTypeCbo()
{
#include "private/libruntime64_type_cbo_embedded.h"
	return TYPE_CBO;
}


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
	const auto &cbo = GetLibIoCbo<BOND_NATIVE_POINTER_SIZE>();
	cboLoader.AddNativeBinding(IO_BINDING_COLLECTION);
	cboLoader.AddCboFile(cbo.GetData(), cbo.GetLength());
}


void LoadLibMath(CboLoader &cboLoader)
{
	const auto &cbo = GetLibMathCbo<BOND_NATIVE_POINTER_SIZE>();
	cboLoader.AddNativeBinding(MATH_BINDING_COLLECTION);
	cboLoader.AddCboFile(cbo.GetData(), cbo.GetLength());
}


void LoadLibMemory(CboLoader &cboLoader)
{
	const auto &cbo = GetLibMemoryCbo<BOND_NATIVE_POINTER_SIZE>();
	cboLoader.AddNativeBinding(MEMORY_BINDING_COLLECTION);
	cboLoader.AddCboFile(cbo.GetData(), cbo.GetLength());
}


void LoadLibString(CboLoader &cboLoader)
{
	const auto &cbo = GetLibStringCbo<BOND_NATIVE_POINTER_SIZE>();
	cboLoader.AddNativeBinding(STRING_BINDING_COLLECTION);
	cboLoader.AddCboFile(cbo.GetData(), cbo.GetLength());
}


void LoadLibType(CboLoader &cboLoader)
{
	const auto &cbo = GetLibTypeCbo<BOND_NATIVE_POINTER_SIZE>();
	cboLoader.AddNativeBinding(TYPE_BINDING_COLLECTION);
	cboLoader.AddCboFile(cbo.GetData(), cbo.GetLength());
}

}
