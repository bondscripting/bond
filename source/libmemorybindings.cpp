#include "bond/api/libmemory.h"

namespace Bond
{

const Bond::NativeFunctionBinding MEMORY_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x8f000ab7, Bond::Allocate},
	{0xf5081d35, Bond::AllocateAligned},
	{0x8cdf3dac, Bond::Free},
	{0x97f0b018, Bond::FreeAligned},
	{0, nullptr}
};

const Bond::NativeBindingCollection MEMORY_BINDING_COLLECTION =
{
	MEMORY_BINDING_COLLECTION_FUNCTIONS,
	4
};

}
