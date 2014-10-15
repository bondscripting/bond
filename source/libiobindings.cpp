#include "bond/api/libio.h"

namespace Bond
{

const Bond::NativeFunctionBinding IO_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0x4472030e, Bond::InputStream__ScanStr},
	{0x9957fe31, Bond::InputStream__ScanB},
	{0x9957fe30, Bond::InputStream__ScanC},
	{0x9957fe3a, Bond::InputStream__ScanI},
	{0x8b85159b, Bond::InputStream__ScanUI},
	{0x9957fe3f, Bond::InputStream__ScanL},
	{0x8b85159e, Bond::InputStream__ScanUL},
	{0x9957fe35, Bond::InputStream__ScanF},
	{0x9957fe37, Bond::InputStream__ScanD},
	{0x76ee18e6, Bond::InputStream__Get},
	{0x940a80fa, Bond::InputStream__Read},
	{0x1dd167df, Bond::InputStream__GetFlags},
	{0x1b962cd3, Bond::InputStream__SetFlags},
	{0xdacf9f6e, Bond::InputStream__UnsetFlags},
	{0x1e219b0a, Bond::InputStream__ClearFlags},
	{0x376c59df, Bond::InputStream__GetPosition},
	{0x2aee0ad3, Bond::InputStream__SetPosition},
	{0x75ee2fe2, Bond::InputStream__SetPositionFromEnd},
	{0x1842b34e, Bond::InputStream__AddOffset},
	{0x0ff20243, Bond::OutputStream__PrintStr},
	{0x72987014, Bond::OutputStream__PrintB},
	{0x72987015, Bond::OutputStream__PrintC},
	{0x7298701f, Bond::OutputStream__PrintI},
	{0x68f854f0, Bond::OutputStream__PrintUI},
	{0x7298701a, Bond::OutputStream__PrintL},
	{0x68f854f5, Bond::OutputStream__PrintUL},
	{0x72987010, Bond::OutputStream__PrintF},
	{0x72987012, Bond::OutputStream__PrintD},
	{0x9697003c, Bond::OutputStream__Put},
	{0x12bc0486, Bond::OutputStream__Write},
	{0x94cf4f40, Bond::OutputStream__GetFlags},
	{0x794e91b4, Bond::OutputStream__SetFlags},
	{0xa24af331, Bond::OutputStream__UnsetFlags},
	{0x28215b61, Bond::OutputStream__ClearFlags},
	{0x0423bd89, Bond::OutputStream__GetPrecision},
	{0x84c207fd, Bond::OutputStream__SetPrecision},
	{0x5f387ab1, Bond::OutputStream__GetWidth},
	{0x0a673635, Bond::OutputStream__SetWidth},
	{0x0d3c922a, Bond::OutputStream__GetPosition},
	{0x5b2b6cfe, Bond::OutputStream__SetPosition},
	{0xaacb2e19, Bond::OutputStream__SetPositionFromEnd},
	{0x4a10a64f, Bond::OutputStream__AddOffset},
	{0x68f556ec, Bond::StdIn},
	{0x2d28d58b, Bond::StdOut},
	{0x4240fb86, Bond::StdErr},
	{0, nullptr}
};

const Bond::NativeBindingCollection IO_BINDING_COLLECTION =
{
	IO_BINDING_COLLECTION_FUNCTIONS,
	45
};

}
