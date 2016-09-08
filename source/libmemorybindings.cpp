#include "bond/api/libmemory.h"

namespace Bond
{

const Bond::NativeFunctionBinding MEMORY_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.GetCollector", Bond::GetCollector},
	{"Bond.Allocate", Bond::Allocate},
	{"Bond.AllocateCollected", Bond::AllocateCollected},
	{"Bond.AllocateWithCollector", Bond::AllocateWithCollector},
	{"Bond.AllocateAligned", Bond::AllocateAligned},
	{"Bond.AllocateAlignedCollected", Bond::AllocateAlignedCollected},
	{"Bond.AllocateAlignedWithCollector", Bond::AllocateAlignedWithCollector},
	{"Bond.Free", Bond::Free},
	{"Bond.FreeAligned", Bond::FreeAligned},
	{"Bond.Memcpy", Bond::Memcpy},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION =
{
	MEMORY_BINDING_COLLECTION_FUNCTIONS,
	10
};

}
