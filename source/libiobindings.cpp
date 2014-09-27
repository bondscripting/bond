#include "bond/api/libio.h"

namespace Bond
{

const Bond::NativeFunctionBinding IO_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x23e70614, Bond::PrintStr},
	{0x2a835323, Bond::PrintB},
	{0x2a835322, Bond::PrintC},
	{0x2a835328, Bond::PrintI},
	{0x20bbfa95, Bond::PrintUI},
	{0x2a83532d, Bond::PrintL},
	{0x20bbfa90, Bond::PrintUL},
	{0x2a835327, Bond::PrintF},
	{0x2a835325, Bond::PrintD},
	{0, nullptr}
};

const Bond::NativeBindingCollection IO_BINDING_COLLECTION =
{
	IO_BINDING_COLLECTION_FUNCTIONS,
	9
};

}
