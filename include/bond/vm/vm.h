#ifndef BOND_VM_VM_H
#define BOND_VM_VM_H

#include "bond/stl/autostack.h"
#include "bond/systems/assert.h"
#include "bond/vm/codesegment.h"

namespace Bond
{

class Allocator;
class CalleeStackFrame;
class CallerStackFrame;
class TextWriter;
class VM;

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
	const ArgType &GetArg(size_t index) const;

	template <typename ReturnType>
	void SetReturnValue(const ReturnType &returnValue);

private:
	template <typename ArgType>
	ArgType &GetArgRef(size_t index) const;

	template <typename ReturnType>
	void AssertValidReturnAssignmentType() const;

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
	void PushArg(const ArgType &arg);

	void Call();

private:
	bu32_t mNextArg;
};


class VM
{
public:
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

#include "bond/private/vm.h"

#endif
