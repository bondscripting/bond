#include "bond/api/libdebug.h"

namespace Bond
{

const Bond::NativeFunctionBinding DEBUG_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.Debug.Break", Bond::Debug::Break},
	{"Bond.Debug.IsDebuggerPresent", Bond::Debug::IsDebuggerPresent},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection DEBUG_BINDING_COLLECTION =
{
	DEBUG_BINDING_COLLECTION_FUNCTIONS,
	2
};

}
