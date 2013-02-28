#ifndef BOND_VM_H
#define BOND_VM_H

#include "bond/assert.h"
#include "bond/autostack.h"
#include "bond/codesegment.h"
#include "bond/typeutil.h"

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
		template <typename T>
		const T &GetArg(size_t index) const
		{
			return *reinterpret_cast<const T*>(mFramePointer + mFunction->mParamListSignature.mParamSignatures[index].mFramePointerOffset);
		}

		template <typename T>
		void SetReturnValue(const T &returnValue) const
		{
			*reinterpret_cast<T*>(mReturnPointer) = returnValue;
		}

	private:
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
			CalleeStackFrame &frame = GetValue();
			const ParamListSignature &paramListSignature = frame.mFunction->mParamListSignature;
			const ParamSignature &param = paramListSignature.mParamSignatures[mNextArg];

#if BOND_RUNTIME_CHECKS_ENABLED
			if (mNextArg >= paramListSignature.mParamCount)
			{
				mVm.RaiseError("Attempt to push too many arguments.");
			}

			if (!ValidateArgType<ArgType>(size_t(param.mSize), SignatureType(param.mType)))
			{
				mVm.RaiseError("Argument type or size mismatch.");
			}
#endif

			++mNextArg;
			*reinterpret_cast<ArgType *>(frame.mFramePointer + param.mFramePointerOffset) = arg;
		}

		void Call();

	private:
		VM &mVm;
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
