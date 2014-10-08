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
		((signatureType == SIG_POINTER) || (signatureType == SIG_STRUCT));
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
inline bool ValidateSignatureType<bi8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template <>
inline bool ValidateSignatureType<bu8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template <>
inline bool ValidateSignatureType<bi16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template <>
inline bool ValidateSignatureType<bu16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template <>
inline bool ValidateSignatureType<bi32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateSignatureType<bu32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template <>
inline bool ValidateSignatureType<bi64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateSignatureType<bu64_t>(size_t size, SignatureType signatureType)
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
		((signatureType == SIG_POINTER) || (signatureType == SIG_STRUCT));
}


template <>
inline bool ValidateReturnType<bool>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<bi8_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<bu8_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<bi16_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<bu16_t>(size_t size, SignatureType signatureType)
{
	return false;
}


template <>
inline bool ValidateReturnType<bi32_t>(size_t size, SignatureType signatureType)
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
inline bool ValidateReturnType<bu32_t>(size_t size, SignatureType signatureType)
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
inline bool ValidateReturnType<bi64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template <>
inline bool ValidateReturnType<bu64_t>(size_t size, SignatureType signatureType)
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
	*reinterpret_cast<bi32_t*>(mReturnPointer) = returnValue ? bi32_t(1) : bi32_t(0);
}


template <>
inline void CalleeStackFrame::SetReturnValue<bi8_t>(const bi8_t &returnValue)
{
	AssertValidReturnAssignmentType<bi8_t>();
	*reinterpret_cast<bi32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<bu8_t>(const bu8_t &returnValue)
{
	AssertValidReturnAssignmentType<bu8_t>();
	*reinterpret_cast<bu32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<bi16_t>(const bi16_t &returnValue)
{
	AssertValidReturnAssignmentType<bi16_t>();
	*reinterpret_cast<bi32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void CalleeStackFrame::SetReturnValue<bu16_t>(const bu16_t &returnValue)
{
	AssertValidReturnAssignmentType<bu16_t>();
	*reinterpret_cast<bu32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <typename ArgType>
inline ArgType &CalleeStackFrame::GetArgRef(size_t index) const
{
	const ParamListSignature &paramListSignature = mFunction->mParamListSignature;

#if BOND_RUNTIME_CHECKS_ENABLED
	if (index >= paramListSignature.mParamCount)
	{
		mVm.RaiseError("Attempt to index argument %" BOND_PRIu32 " out of range.", bu32_t(index));
	}
#endif

	const ParamSignature &param = paramListSignature.mParamSignatures[index];

#if BOND_RUNTIME_CHECKS_ENABLED
	if (!ValidateSignatureType<ArgType>(size_t(param.mSize), SignatureType(param.mType)))
	{
		char buffer[64];
		mVm.RaiseError(
			"Attempt to access argument %" BOND_PRIu32 " using wrong type. Expected '%s'.",
			bu32_t(index),
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


inline CallerStackFrame::CallerStackFrame(VM &vm, const HashedString &functionName):
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


template <typename ReturnType>
inline CallerStackFrame::CallerStackFrame(VM &vm, const HashedString &functionName, ReturnType *returnPointer):
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


template <typename ArgType>
inline void CallerStackFrame::PushArg(const ArgType &arg)
{
	GetValue().GetArgRef<ArgType>(mNextArg) = arg;
	++mNextArg;
}


template <typename ReturnType>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.Call();
}


inline void VM::CallVoidFunction(const HashedString &functionName)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.Call();
}


template <typename ReturnType, typename ArgType0>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress, ArgType0 a0)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArg(a0);
	stackFrame.Call();
}


template <typename ArgType0>
inline void VM::CallVoidFunction(const HashedString &functionName, ArgType0 a0)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArg(a0);
	stackFrame.Call();
}


template <typename ReturnType, typename ArgType0, typename ArgType1>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress,
	ArgType0 a0, ArgType1 a1)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.Call();
}


template <typename ArgType0, typename ArgType1>
inline void VM::CallVoidFunction(const HashedString &functionName, ArgType0 a0, ArgType1 a1)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.Call();
}


template <typename ReturnType, typename ArgType0, typename ArgType1, typename ArgType2>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress,
	ArgType0 a0, ArgType1 a1, ArgType2 a2)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.Call();
}


template <typename ArgType0, typename ArgType1, typename ArgType2>
inline void VM::CallVoidFunction(const HashedString &functionName, ArgType0 a0,
	ArgType1 a1, ArgType2 a2)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.Call();
}


template <typename ReturnType, typename ArgType0, typename ArgType1,
	typename ArgType2, typename ArgType3>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress,
	ArgType0 a0, ArgType1 a1, ArgType2 a2, ArgType3 a3)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.PushArg(a3);
	stackFrame.Call();
}


template <typename ArgType0, typename ArgType1, typename ArgType2, typename ArgType3>
inline void VM::CallVoidFunction(const HashedString &functionName, ArgType0 a0,
	ArgType1 a1, ArgType2 a2, ArgType3 a3)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.PushArg(a3);
	stackFrame.Call();
}


template <typename ReturnType, typename ArgType0, typename ArgType1,
	typename ArgType2, typename ArgType3, typename ArgType4>
inline void VM::CallFunction(const HashedString &functionName, ReturnType *returnAddress,
	ArgType0 a0, ArgType1 a1, ArgType2 a2, ArgType3 a3, ArgType4 a4)
{
	Bond::CallerStackFrame stackFrame(*this, functionName, returnAddress);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.PushArg(a3);
	stackFrame.PushArg(a4);
	stackFrame.Call();
}


template <typename ArgType0, typename ArgType1, typename ArgType2,
	typename ArgType3, typename ArgType4>
inline void VM::CallVoidFunction(const HashedString &functionName, ArgType0 a0,
	ArgType1 a1, ArgType2 a2, ArgType3 a3, ArgType4 a4)
{
	Bond::CallerStackFrame stackFrame(*this, functionName);
	stackFrame.PushArg(a0);
	stackFrame.PushArg(a1);
	stackFrame.PushArg(a2);
	stackFrame.PushArg(a3);
	stackFrame.PushArg(a4);
	stackFrame.Call();
}

}

#endif
