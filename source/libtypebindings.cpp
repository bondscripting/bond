#include "bond/api/libtype.h"

namespace Bond
{

const Bond::NativeFunctionBinding TYPE_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x14fc8f37, Bond::IsAlnum},
	{0x1e1f2fa6, Bond::IsAlpha},
	{0x362ddf95, Bond::IsCntrl},
	{0xc04c09bd, Bond::IsDigit},
	{0x88d3fcf6, Bond::IsGraph},
	{0xc6fe11bd, Bond::IsLower},
	{0x84c6133f, Bond::IsPrint},
	{0x42ea944e, Bond::IsPunct},
	{0x5385decc, Bond::IsSpace},
	{0x502ed5a2, Bond::IsUpper},
	{0xc8732d81, Bond::IsXDigit},
	{0x24a99ae8, Bond::ToLower},
	{0x815db49f, Bond::ToUpper},
	{0, nullptr}
};

const Bond::NativeBindingCollection TYPE_BINDING_COLLECTION =
{
	TYPE_BINDING_COLLECTION_FUNCTIONS,
	13
};

}
