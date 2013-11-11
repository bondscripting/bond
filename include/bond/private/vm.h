#ifndef BOND_PRIVATE_VM_H
#define BOND_PRIVATE_VM_H

#include "bond/systems/math.h"

namespace Bond
{

template<typename T>
bool ValidateSignatureType(size_t size, SignatureType signatureType)
{
	return
		(sizeof(T)  == size) &&
		((signatureType == SIG_POINTER) || (signatureType == SIG_STRUCT));
}


template<>
inline bool ValidateSignatureType<bool>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bi8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bu8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bi16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateSignatureType<bu16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateSignatureType<bi32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateSignatureType<bu32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateSignatureType<bi64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateSignatureType<bu64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateSignatureType<bf32_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_FLOAT;
}


template<>
inline bool ValidateSignatureType<bf64_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_DOUBLE;
}


template <typename ArgType>
inline const ArgType &VM::CalleeStackFrame::GetArg(size_t index) const
{
	return GetArgRef<ArgType>(index);
}


template <typename ReturnType>
inline void VM::CalleeStackFrame::SetReturnValue(const ReturnType &returnValue)
{
	AssertValidReturnAssignmentType<ReturnType>();
	*reinterpret_cast<ReturnType*>(mReturnPointer) = returnValue;
}


template <>
inline void VM::CalleeStackFrame::SetReturnValue<bool>(const bool &returnValue)
{
	AssertValidReturnAssignmentType<bool>();
	*reinterpret_cast<bi32_t*>(mReturnPointer) = returnValue ? bi32_t(1) : bi32_t(0);
}


template <>
inline void VM::CalleeStackFrame::SetReturnValue<bi8_t>(const bi8_t &returnValue)
{
	AssertValidReturnAssignmentType<bi8_t>();
	*reinterpret_cast<bi32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void VM::CalleeStackFrame::SetReturnValue<bu8_t>(const bu8_t &returnValue)
{
	AssertValidReturnAssignmentType<bu8_t>();
	*reinterpret_cast<bu32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void VM::CalleeStackFrame::SetReturnValue<bi16_t>(const bi16_t &returnValue)
{
	AssertValidReturnAssignmentType<bi16_t>();
	*reinterpret_cast<bi32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <>
inline void VM::CalleeStackFrame::SetReturnValue<bu16_t>(const bu16_t &returnValue)
{
	AssertValidReturnAssignmentType<bu16_t>();
	*reinterpret_cast<bu32_t*>(mReturnPointer) = bi32_t(returnValue);
}


template <typename ArgType>
inline ArgType &VM::CalleeStackFrame::GetArgRef(size_t index) const
{
	const ParamListSignature &paramListSignature = mFunction->mParamListSignature;

#if BOND_RUNTIME_CHECKS_ENABLED
	if (index >= paramListSignature.mParamCount)
	{
		mVm.RaiseError("Attempt to index argument out of range.");
	}
#endif

	const ParamSignature &param = paramListSignature.mParamSignatures[index];

#if BOND_RUNTIME_CHECKS_ENABLED
	if (!ValidateSignatureType<ArgType>(size_t(param.mSize), SignatureType(param.mType)))
	{
		mVm.RaiseError("Attempt to access argument using wrong type.");
	}
#endif

	return *reinterpret_cast<ArgType *>(mFramePointer + param.mFramePointerOffset);
}


template <typename ReturnType>
inline void VM::CalleeStackFrame::AssertValidReturnAssignmentType() const
{
#if BOND_RUNTIME_CHECKS_ENABLED
	const ReturnSignature &ret = mFunction->mReturnSignature;
	if (!ValidateSignatureType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
	{
		mVm.RaiseError("Attempt to access return value using wrong type.");
	}
#endif
}


template<typename ArgType>
void VM::CallerStackFrame::PushArg(const ArgType &arg)
{
	GetValue().GetArgRef<ArgType>(mNextArg) = arg;
	++mNextArg;
}

}

#endif
