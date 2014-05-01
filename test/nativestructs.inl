#include "nativestructs.h"


const Bond::NativeFunctionBinding NATIVE_STRUCTS_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x690280b8, Vector3__Dot},
	{0x101f5b05, Vector3__X__get},
	{0x10014629, Vector3__X__set},
	{0xc4b2c150, Vector3__Y__get},
	{0xb4949294, Vector3__Y__set},
	{0x9e922db3, Vector3__Z__get},
	{0x8ec3b117, Vector3__Z__set},
	{0, NULL}
};

const Bond::NativeBindingCollection NATIVE_STRUCTS_BINDING_COLLECTION =
{
	NATIVE_STRUCTS_BINDING_COLLECTION_FUNCTIONS,
	7
};

