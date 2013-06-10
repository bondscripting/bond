#ifndef BOND_VM_VM_H
#define BOND_VM_VM_H

#include "bond/stl/autostack.h"
#include "bond/systems/assert.h"
#include "bond/types/typeutil.h"
#include "bond/vm/codesegment.h"

namespace Bond
{

class Allocator;
class TextWriter;

class VM
{
public:
	class CallerStackFrame;

	class CalleeStackFrame
	{
	public:
		CalleeStackFrame(VM &vm):
			mVm(vm),
			mFunction(NULL),
			mFramePointer(NULL),
			mStackPointer(NULL),
			mReturnPointer(NULL)
		{}

		CalleeStackFrame(
				VM &vm,
				const Function *function,
				bu8_t *framePointer,
				bu8_t *stackPointer,
				bu8_t *returnPointer):
			mVm(vm),
			mFunction(function),
			mFramePointer(framePointer),
			mStackPointer(stackPointer),
			mReturnPointer(returnPointer)
		{}

		template <typename ArgType>
		const ArgType &GetArg(size_t index) const
		{
			return GetArgRef<ArgType>(index);
		}

		template <typename ReturnType>
		void SetReturnValue(const ReturnType &returnValue) const
		{
			GetArgRef<ReturnType>() = returnValue;
		}

	private:
		template <typename ArgType>
		ArgType &GetArgRef(size_t index) const
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
		ReturnType &GetReturnRef() const
		{
#if BOND_RUNTIME_CHECKS_ENABLED
			const ReturnSignature &ret = mFunction->mReturnSignature;
			if (!ValidateSignatureType<ReturnType>(size_t(ret.mSize), SignatureType(ret.mType)))
			{
				mVm.RaiseError("Attempt to access return value using wrong type.");
			}
#endif
			return *reinterpret_cast<ReturnType*>(mReturnPointer);
		}

		VM &mVm;
		const Function *mFunction;
		bu8_t *mFramePointer;
		bu8_t *mStackPointer;
		bu8_t *mReturnPointer;
		friend class VM;
		friend class CallerStackFrame;
	};

	typedef AutoStack<CalleeStackFrame> StackFrames;

	class CallerStackFrame: private StackFrames::Element
	{
	public:
		CallerStackFrame(VM &vm, const HashedString &functionName, void *returnPointer = NULL);

		template<typename ArgType>
		void PushArg(const ArgType &arg)
		{
			GetValue().GetArgRef<ArgType>(mNextArg) = arg;
			++mNextArg;
		}

		template <typename ReturnType>
		const ReturnType &GetReturnValue() const
		{
			return GetValue().GetReturnRef<ReturnType>();
		}

		void Call();

	private:
		bu32_t mNextArg;
	};

	VM(Allocator &allocator, const CodeSegment &codeSegment, size_t stackSize);
	~VM();

	const CodeSegment &GetCodeSegment() const { return mCodeSegment; }

	CalleeStackFrame &GetTopStackFrame() { return mStackFrames.GetTop(); }
	const CalleeStackFrame &GetTopStackFrame() const { return mStackFrames.GetTop(); }

	void DumpCallStack(TextWriter &writer) const;
	void DumpStackFrame(TextWriter &writer, const CalleeStackFrame &frame) const;

	void RaiseError(const char *message) const;

private:
	friend class CallerStackFrame;

	void ExecuteScriptFunction();
	bu8_t *InvokeFunction(const Function *function, bu8_t *stackTop);
	void ValidateStackPointer(bu8_t *stackPointer) const;

	StackFrames mStackFrames;
	StackFrames::Element mDummyFrame;
	Allocator &mAllocator;
	const CodeSegment &mCodeSegment;
	bu8_t *mStack;
	size_t mStackSize;
};

}

#endif
