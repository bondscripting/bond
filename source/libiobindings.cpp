#include "bond/api/libio.h"

namespace Bond
{

const Bond::NativeFunctionBinding IO_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x0ff20243, Bond::OutputStream__PrintStr},
	{0x72987014, Bond::OutputStream__PrintB},
	{0x72987015, Bond::OutputStream__PrintC},
	{0x7298701f, Bond::OutputStream__PrintI},
	{0x68f854f0, Bond::OutputStream__PrintUI},
	{0x7298701a, Bond::OutputStream__PrintL},
	{0x68f854f5, Bond::OutputStream__PrintUL},
	{0x72987010, Bond::OutputStream__PrintF},
	{0x72987012, Bond::OutputStream__PrintD},
	{0x0d3c922a, Bond::OutputStream__GetPosition},
	{0x5b2b6cfe, Bond::OutputStream__SetPosition},
	{0xaacb2e19, Bond::OutputStream__SetPositionFromEnd},
	{0x4a10a64f, Bond::OutputStream__AddOffset},
	{0x2d28d58b, Bond::StdOut},
	{0x4240fb86, Bond::StdErr},
	{0, nullptr}
};

const Bond::NativeBindingCollection IO_BINDING_COLLECTION =
{
	IO_BINDING_COLLECTION_FUNCTIONS,
	15
};

}
