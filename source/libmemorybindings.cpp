#include "bond/api/libmemory.h"

namespace Bond
{

const Bond::NativeFunctionBinding MEMORY_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.Allocate", Bond::Allocate},
	{"Bond.AllocateAligned", Bond::AllocateAligned},
	{"Bond.Free", Bond::Free},
	{"Bond.FreeAligned", Bond::FreeAligned},
	{"Bond.Memcpy", Bond::Memcpy},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION =
{
	MEMORY_BINDING_COLLECTION_FUNCTIONS,
	5
};

}
