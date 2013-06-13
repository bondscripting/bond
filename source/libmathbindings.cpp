#include "bond/api/libmath.h"

namespace Bond
{

const Bond::NativeFunctionBinding MATH_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0xde43ac9c, Bond::Sin},
	{0x8088b9f2, Bond::Sinf},
	{0xf81cc603, Bond::Cos},
	{0x984bb6df, Bond::Cosf},
	{0, NULL}
};

const Bond::NativeBindingCollection MATH_BINDING_COLLECTION =
{
	MATH_BINDING_COLLECTION_FUNCTIONS,
	4
};

}
