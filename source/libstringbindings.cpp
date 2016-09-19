#include "bond/api/libstring.h"

namespace Bond
{

const Bond::NativeFunctionBinding STRING_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.Strlen", Bond::Strlen},
	{"Bond.Strcmp", Bond::Strcmp},
	{"Bond.TryParseInt", Bond::TryParseInt},
	{"Bond.TryParseLong", Bond::TryParseLong},
	{"Bond.TryParseFloat", Bond::TryParseFloat},
	{"Bond.TryParseDouble", Bond::TryParseDouble},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection STRING_BINDING_COLLECTION =
{
	STRING_BINDING_COLLECTION_FUNCTIONS,
	6
};

}
