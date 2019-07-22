#include "nativestructs.h"


const Bond::NativeFunctionBinding NATIVE_STRUCTS_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Vector3.Dot", Vector3__Dot},
	{"Vector3.X.@get", Vector3__X__get},
	{"Vector3.X.@set", Vector3__X__set},
	{"Vector3.Y.@get", Vector3__Y__get},
	{"Vector3.Y.@set", Vector3__Y__set},
	{"Vector3.Z.@get", Vector3__Z__get},
	{"Vector3.Z.@set", Vector3__Z__set},
	{0, NULL}
};

const Bond::NativeBindingCollection NATIVE_STRUCTS_BINDING_COLLECTION =
{
	NATIVE_STRUCTS_BINDING_COLLECTION_FUNCTIONS,
	7
};

