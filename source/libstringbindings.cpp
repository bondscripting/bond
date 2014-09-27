#include "bond/api/libstring.h"

namespace Bond
{

const Bond::NativeFunctionBinding STRING_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x476d4300, Bond::Strlen},
	{0xf8f18409, Bond::TryParseInt},
	{0x1d1a7bd4, Bond::TryParseLong},
	{0xc86cddb0, Bond::TryParseFloat},
	{0xc8b53ac5, Bond::TryParseDouble},
	{0, nullptr}
};

const Bond::NativeBindingCollection STRING_BINDING_COLLECTION =
{
	STRING_BINDING_COLLECTION_FUNCTIONS,
	5
};

}
