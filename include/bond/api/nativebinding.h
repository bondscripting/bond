#ifndef BOND_API_NATIVEBINDING_H
#define BOND_API_NATIVEBINDING_H

#include "bond/conf.h"
#include "bond/types/types.h"

namespace Bond
{

class StackFrame;

/// \addtogroup api
/// @{

/// \brief A C++ function that implements a function declared native in Bond.
///
/// The function reads its arguments from, and sets its return value through, the StackFrame of
/// its invocation.
/// \sa StackFrame::GetArg, StackFrame::SetReturnValue
typedef void (*NativeFunction)(StackFrame &frame);


/// \brief Binds a native Bond function to its C++ implementation.
struct NativeFunctionBinding
{
	/// \brief The fully qualified name of the Bond function with its elements separated by the '.'
	/// character, e.g. "Namespace.Function".
	const char *mName;

	/// \brief The C++ implementation of the function.
	NativeFunction mFunction;
};


/// \brief A collection of NativeFunctionBindings, typically one per library, passed to
/// CboLoader::AddNativeBinding.
///
/// Collections for Bond source files that declare native functions can be generated with a
/// NativeBindingGenerator.
struct NativeBindingCollection
{
	/// \brief Pointer to the array of function bindings.
	const NativeFunctionBinding *mFunctionBindings;

	/// \brief Number of entries in the array of function bindings.
	uint32_t mFunctionBindingCount;
};

/// @}

}

#endif
