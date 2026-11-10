#ifndef BOND_PRIVATE_VM_H
#define BOND_PRIVATE_VM_H

#include "bond/stl/typetraits.h"
#include "bond/systems/math.h"

namespace Bond
{

inline bool IsIntegerSignatureType(SignatureType signatureType)
{
	switch (signatureType)
	{
		case SIG_BOOL:
		case SIG_CHAR:
		case SIG_UCHAR:
		case SIG_SHORT:
		case SIG_USHORT:
		case SIG_INT:
		case SIG_UINT:
		case SIG_LONG:
		case SIG_ULONG:
			return true;
		default:
			return false;
	}
}


/// \brief Returns whether a value of native type T can be stored in, or read from, a Bond value
/// with the given signature type and size. Integers match any integer signature type of the same
/// size, regardless of signedness. Unsupported types are rejected at compile time.
template <typename T>
bool ValidateSignatureType(size_t size, SignatureType signatureType)
{
	if constexpr (Bond::is_enum_v<T>)
	{
		return ValidateSignatureType<Bond::underlying_type_t<T>>(size, signatureType);
	}
	else
	{
		static_assert(
			Bond::is_integral_v<T> || Bond::is_same_v<T, float> || Bond::is_same_v<T, double> || Bond::is_pointer_v<T> ||
			((Bond::is_class_v<T> || Bond::is_union_v<T>) && Bond::is_trivially_copyable_v<T>),
			"Type cannot be passed to or returned from Bond.");

		bool typeMatches;
		if constexpr (Bond::is_integral_v<T>)
		{
			typeMatches = IsIntegerSignatureType(signatureType);
		}
		else if constexpr (Bond::is_same_v<T, float>)
		{
			typeMatches = signatureType == SIG_FLOAT;
		}
		else if constexpr (Bond::is_same_v<T, double>)
		{
			typeMatches = signatureType == SIG_DOUBLE;
		}
		else if constexpr (Bond::is_pointer_v<T>)
		{
			typeMatches = signatureType == SIG_POINTER;
		}
		else
		{
			typeMatches = signatureType == SIG_AGGREGATE;
		}
		return typeMatches && (sizeof(T) == size);
	}
}


/// \brief Returns whether a variable of native type T can receive the return value of a function
/// with the given return signature. Integers smaller than 32 bits are returned widened to 32 bits,
/// so they require a 32-bit return variable.
template <typename T>
bool ValidateReturnType(size_t size, SignatureType signatureType)
{
	if constexpr (Bond::is_enum_v<T>)
	{
		return ValidateReturnType<Bond::underlying_type_t<T>>(size, signatureType);
	}
	else if constexpr (Bond::is_integral_v<T> && (sizeof(T) == sizeof(int32_t)))
	{
		return IsIntegerSignatureType(signatureType) && (size <= sizeof(T));
	}
	else
	{
		static_assert(!Bond::is_integral_v<T> || (sizeof(T) > sizeof(int32_t)),
			"Integer return values smaller than 32 bits are widened to 32 bits. Use int32_t or uint32_t.");
		return ValidateSignatureType<T>(size, signatureType);
	}
}


/// \brief Widens an integer smaller than 32 bits to 32 bits as the VM does for a value of the
/// given signature type.
template <typename T>
int32_t WidenSmallInteger(T value, SignatureType signatureType)
{
	switch (signatureType)
	{
		case SIG_BOOL: return (value != T(0)) ? int32_t(1) : int32_t(0);
		case SIG_CHAR: return int32_t(int8_t(value));
		case SIG_UCHAR: return int32_t(uint8_t(value));
		case SIG_SHORT: return int32_t(int16_t(value));
		default: return int32_t(uint16_t(value));
	}
}


template <typename ArgType>
inline const ArgType &StackFrame::GetArg(size_t index) const
{
	return GetArgRef<ArgType>(index);
}


template <typename ReturnType>
inline void StackFrame::SetReturnValue(const ReturnType &returnValue)
{
	if constexpr (Bond::is_enum_v<ReturnType>)
	{
		SetReturnValue(Bond::underlying_type_t<ReturnType>(returnValue));
	}
	else if constexpr (Bond::is_null_pointer_v<ReturnType>)
	{
		SetReturnValue(static_cast<void *>(nullptr));
	}
	else
	{
		ValidateReturnAssignmentType<ReturnType>();
		if constexpr (Bond::is_integral_v<ReturnType> && (sizeof(ReturnType) < sizeof(int32_t)))
		{
			// The operand stack holds small integers widened according to their Bond type.
			const SignatureType signatureType = SignatureType(mFunction->mReturnSignature.mType);
			*reinterpret_cast<int32_t *>(mReturnPointer) = WidenSmallInteger(returnValue, signatureType);
		}
		else
		{
			*reinterpret_cast<ReturnType *>(mReturnPointer) = returnValue;
		}
	}
}


template <typename ArgType>
inline ArgType &StackFrame::GetArgRef(size_t index) const
{
	const ParamListSignature &paramListSignature = mFunction->mParamListSignature;

#if BOND_RUNTIME_CHECKS_ENABLED
	if (index >= paramListSignature.mParamCount)
	{
		mVm.RaiseError("Attempt to index argument %" BOND_PRIu32 " out of range.", uint32_t(index));
	}
#endif

	const ParamSignature &param = paramListSignature.mParamSignatures[index];

#if BOND_RUNTIME_CHECKS_ENABLED
	if (!ValidateSignatureType<ArgType>(size_t(param.mSize), SignatureType(param.mType)))
	{
		char buffer[64];
		mVm.RaiseError(
			"Attempt to access argument %" BOND_PRIu32 " using wrong type. Expected '%s'.",
			uint32_t(index),
			ExpandApiTypeMnemonic(buffer, sizeof(buffer), SignatureType(param.mType), param.mSize));
	}
#endif

	return *reinterpret_cast<ArgType *>(mFramePointer + param.mFramePointerOffset);
}


template <typename ReturnType>
inline void StackFrame::ValidateReturnAssignmentType() const
{
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mFunction->mReturnSignature;
	if (!ValidateSignatureType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
	{
		char buffer[64];
		mVm.RaiseError("Attempt to return a value of the wrong type. Expected '%s'.",
			ExpandApiTypeMnemonic(buffer, sizeof(buffer), SignatureType(ret.mType), ret.mSize));
	}
#endif
}


inline InvocationStackFrame::InvocationStackFrame(VM &vm, const QualifiedName &functionName):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, functionName, nullptr);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (ret.mType != SIG_VOID)
	{
		vm.RaiseError("Attempt to call a non-void function without a return address.");
	}
#endif
}


inline InvocationStackFrame::InvocationStackFrame(VM &vm, const char *functionName):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, functionName, nullptr);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (ret.mType != SIG_VOID)
	{
		vm.RaiseError("Attempt to call a non-void function without a return address.");
	}
#endif
}


inline InvocationStackFrame::InvocationStackFrame(VM &vm, const Function &function):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, function, nullptr);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (ret.mType != SIG_VOID)
	{
		vm.RaiseError("Attempt to call a non-void function without a return address.");
	}
#endif
}


template <typename ReturnType>
inline InvocationStackFrame::InvocationStackFrame(VM &vm, const QualifiedName &functionName, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, functionName, returnPointer);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (!ValidateReturnType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
	{
		char buffer[64];
		vm.RaiseError("Attempt to call a function with incorrect return address type. Expected '%s'.",
			ExpandApiTypeMnemonic(buffer, sizeof(buffer), SignatureType(ret.mType), ret.mSize));
	}
#endif
}


template <typename ReturnType>
inline InvocationStackFrame::InvocationStackFrame(VM &vm, const char *functionName, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, functionName, returnPointer);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (!ValidateReturnType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
	{
		char buffer[64];
		vm.RaiseError("Attempt to call a function with incorrect return address type. Expected '%s'.",
			ExpandApiTypeMnemonic(buffer, sizeof(buffer), SignatureType(ret.mType), ret.mSize));
	}
#endif
}


template <typename ReturnType>
inline InvocationStackFrame::InvocationStackFrame(VM &vm, const Function &function, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, vm),
	mNextArg(0)
{
	Initialize(vm, function, returnPointer);
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mValue.mFunction->mReturnSignature;
	if (!ValidateReturnType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
	{
		char buffer[64];
		vm.RaiseError("Attempt to call a function with incorrect return address type. Expected '%s'.",
			ExpandApiTypeMnemonic(buffer, sizeof(buffer), SignatureType(ret.mType), ret.mSize));
	}
#endif
}


template <typename ArgType>
inline void InvocationStackFrame::PushArg(const ArgType &arg)
{
	if constexpr (Bond::is_enum_v<ArgType>)
	{
		PushArg(Bond::underlying_type_t<ArgType>(arg));
	}
	else if constexpr (Bond::is_null_pointer_v<ArgType>)
	{
		PushArg(static_cast<void *>(nullptr));
	}
	else
	{
		StackFrame &frame = GetValue();

#if BOND_RUNTIME_CHECKS_ENABLED
		if (mNextArg >= frame.mFunction->mParamListSignature.mParamCount)
		{
			frame.mVm.RaiseError("Attempt to call function with too many arguments.");
		}
#endif

		ArgType &destination = frame.GetArgRef<ArgType>(mNextArg);
		if constexpr (Bond::is_integral_v<ArgType> && (sizeof(ArgType) == BOND_BOOL_SIZE))
		{
			// Store bools as 0 or 1 regardless of the type used to pass them.
			const bool isBool = frame.mFunction->mParamListSignature.mParamSignatures[mNextArg].mType == SIG_BOOL;
			destination = isBool ? ArgType(arg != ArgType(0)) : arg;
		}
		else
		{
			destination = arg;
		}
		++mNextArg;
	}
}


template <typename... Args>
inline void InvocationStackFrame::PushArgs(Args... args)
{
	(PushArg(args), ...);
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const QualifiedName &functionName, ReturnType *returnAddress, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const char *functionName, ReturnType *returnAddress, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const Function &function, ReturnType *returnAddress, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, function, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const QualifiedName &functionName, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, functionName);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const char *functionName, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, functionName);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const Function &function, Args... args)
{
	Bond::InvocationStackFrame stackFrame(*this, function);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}

}

#endif
