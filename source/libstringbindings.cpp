#include "bond/api/libstring.h"

namespace Bond
{

const Bond::NativeFunctionBinding STRING_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x476d4300, Bond::Strlen},
	{0, NULL}
};

const Bond::NativeBindingCollection STRING_BINDING_COLLECTION =
{
	STRING_BINDING_COLLECTION_FUNCTIONS,
	1
};

}
