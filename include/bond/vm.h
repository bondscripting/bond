#ifndef BOND_VM_H
#define BOND_VM_H

#include "bond/autostack.h"
#include "bond/codesegment.h"

namespace Bond
{

class Allocator;

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
			const ParamSignature &param = frame.mFunction->mParamListSignature.mParamSignatures[mNextArg++];
			// TODO: Assert that mNextArg < frame.mFunction->mParamListSignature.mParamCount.
			// TODO: Assert that Min(sizeof(ArgType), MIN_STACK_FRAME_ALIGN) == param.mSize.
			*reinterpret_cast<ArgType *>(frame.mFramePointer + param.mFramePointerOffset) = arg;
		}

		void Call()
		{
			mVm.ExecuteScriptFunction();
		}

	private:
		VM &mVm;
		int mNextArg;
	};

	VM(Allocator &allocator, const CodeSegment &codeSegment, size_t stackSize);
	~VM();

	const CodeSegment &GetCodeSegment() const { return mCodeSegment; }

	CalleeStackFrame &GetTopStackFrame() { return mStackFrames.GetTop(); }
	const CalleeStackFrame &GetTopStackFrame() const { return mStackFrames.GetTop(); }

private:
	friend class CallerStackFrame;

	void ExecuteScriptFunction();
	bu8_t *InvokeFunction(const Function *function, bu8_t *stackPointer);

	StackFrames mStackFrames;
	StackFrames::Element mDummyFrame;
	Allocator &mAllocator;
	const CodeSegment &mCodeSegment;
	bu8_t *mStack;
	size_t mStackSize;
};

}

#endif
