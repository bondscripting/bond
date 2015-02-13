#ifndef BOND_PRIVATE_VM_H
#define BOND_PRIVATE_VM_H

#include "bond/systems/math.h"

namespace Bond
{

template <typename T>
bool ValidateSignatureType(size_t size, SignatureType signatureType)
{
	return
		(sizeof(T)  == size) &&
		((signatureType == SIG_POINTER) || (signatureType == SIG_AGGREGATE));
}


template <>
inline bool ValidateSignatureType<bool>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template <>
inline bool ValidateSignatureType<int8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template <>
inline bool ValidateSignatureType<uint8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template <>
inline bool ValidateSignatureType<int16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template <>
inline bool ValidateSignatureType<uint16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template <>
inline bool ValidateSignatureType<int32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateSignatureType<uint32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateSignatureType<int64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateSignatureType<uint64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateSignatureType<float>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_FLOAT;
}


template <>
inline bool ValidateSignatureType<double>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_DOUBLE;
}


template <typename T>
bool ValidateReturnType(size_t size, SignatureType signatureType)
{
	return
		(sizeof(T)  == size) &&
		((signatureType == SIG_POINTER) || (signatureType == SIG_AGGREGATE));
}


template <>
inline bool ValidateReturnType<bool>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<int8_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<uint8_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<int16_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<uint16_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<int32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR) ||
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT) ||
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateReturnType<uint32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR) ||
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT) ||
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateReturnType<int64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateReturnType<uint64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateReturnType<float>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_FLOAT;
}


template <>
inline bool ValidateReturnType<double>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_DOUBLE;
}


template <typename ArgType>
inline const ArgType &CalleeStackFrame::GetArg(size_t index) const
{
	return GetArgRef<ArgType>(index);
}


template <typename ReturnType>
inline void CalleeStackFrame::SetReturnValue(const ReturnType &returnValue)
{
	AssertValidReturnAssignmentType<ReturnType>();
	*reinterpret_cast<ReturnType*>(mReturnPointer) = returnValue;
}


template <>
inline void CalleeStackFrame::SetReturnValue<bool>(const bool &returnValue)
{
	AssertValidReturnAssignmentType<bool>();
	*reinterpret_cast<int32_t*>(mReturnPointer) = returnValue ? int32_t(1) : int32_t(0);
}


template <>
inline void CalleeStackFrame::SetReturnValue<int8_t>(const int8_t &returnValue)
{
	AssertValidReturnAssignmentType<int8_t>();
	*reinterpret_cast<int32_t*>(mReturnPointer) = int32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<uint8_t>(const uint8_t &returnValue)
{
	AssertValidReturnAssignmentType<uint8_t>();
	*reinterpret_cast<uint32_t*>(mReturnPointer) = int32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<int16_t>(const int16_t &returnValue)
{
	AssertValidReturnAssignmentType<int16_t>();
	*reinterpret_cast<int32_t*>(mReturnPointer) = int32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<uint16_t>(const uint16_t &returnValue)
{
	AssertValidReturnAssignmentType<uint16_t>();
	*reinterpret_cast<uint32_t*>(mReturnPointer) = int32_t(returnValue);
}


template <typename ArgType>
inline ArgType &CalleeStackFrame::GetArgRef(size_t index) const
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
inline void CalleeStackFrame::AssertValidReturnAssignmentType() const
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


inline CallerStackFrame::CallerStackFrame(VM &vm, const QualifiedName &functionName):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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


inline CallerStackFrame::CallerStackFrame(VM &vm, const char *functionName):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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


inline CallerStackFrame::CallerStackFrame(VM &vm, const Function &function):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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
inline CallerStackFrame::CallerStackFrame(VM &vm, const QualifiedName &functionName, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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
inline CallerStackFrame::CallerStackFrame(VM &vm, const char *functionName, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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
inline CallerStackFrame::CallerStackFrame(VM &vm, const Function &function, ReturnType *returnPointer):
	StackFrames::Element(vm.mStackFrames, CalleeStackFrame(vm)),
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
inline void CallerStackFrame::PushArg(const ArgType &arg)
{
	GetValue().GetArgRef<ArgType>(mNextArg) = arg;
	++mNextArg;
}


template <typename Arg1, typename... Args>
inline void CallerStackFrame::PushArgs(Arg1 arg1, Args... args)
{
	PushArg(arg1);
	PushArgs(args...);
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const QualifiedName &functionName, ReturnType *returnAddress, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const char *functionName, ReturnType *returnAddress, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename ReturnType, typename... Args>
inline void VM::CallFunction(const Function &function, ReturnType *returnAddress, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, function, returnAddress);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const QualifiedName &functionName, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const char *functionName, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}


template <typename... Args>
inline void VM::CallVoidFunction(const Function &function, Args... args)
{
	Bond::CallerStackFrame stackFrame(*this, function);
	stackFrame.PushArgs(args...);
	stackFrame.Call();
}

}

#endif
