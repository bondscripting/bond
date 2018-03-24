#include "bond/api/libtype.h"

namespace Bond
{

const Bond::NativeFunctionBinding TYPE_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.IsAlnum", Bond::IsAlnum},
	{"Bond.IsAlpha", Bond::IsAlpha},
	{"Bond.IsBlank", Bond::IsBlank},
	{"Bond.IsCntrl", Bond::IsCntrl},
	{"Bond.IsDigit", Bond::IsDigit},
	{"Bond.IsGraph", Bond::IsGraph},
	{"Bond.IsLower", Bond::IsLower},
	{"Bond.IsPrint", Bond::IsPrint},
	{"Bond.IsPunct", Bond::IsPunct},
	{"Bond.IsSpace", Bond::IsSpace},
	{"Bond.IsUpper", Bond::IsUpper},
	{"Bond.IsXDigit", Bond::IsXDigit},
	{"Bond.ToLower", Bond::ToLower},
	{"Bond.ToUpper", Bond::ToUpper},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection TYPE_BINDING_COLLECTION =
{
	TYPE_BINDING_COLLECTION_FUNCTIONS,
	14
};

}
