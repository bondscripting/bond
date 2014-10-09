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
class OutputStream;
class VM;

class CalleeStackFrame
{
public:
	CalleeStackFrame(VM &vm):
		mVm(vm),
		mFunction(nullptr),
		mFramePointer(nullptr),
		mStackPointer(nullptr),
		mReturnPointer(nullptr)
	{}

	CalleeStackFrame(
			VM &vm,
			const Function *function,
			uint8_t *framePointer,
			uint8_t *stackPointer,
			uint8_t *returnPointer):
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

	VM &GetVM() const { return mVm; }

private:
	friend class VM;
	friend class CallerStackFrame;

	template <typename ArgType>
	ArgType &GetArgRef(size_t index) const;

	template <typename ReturnType>
	void AssertValidReturnAssignmentType() const;

	VM &mVm;
	const Function *mFunction;
	uint8_t *mFramePointer;
	uint8_t *mStackPointer;
	uint8_t *mReturnPointer;
};

typedef AutoStack<CalleeStackFrame> StackFrames;


class CallerStackFrame: private StackFrames::Element
{
public:
	CallerStackFrame(VM &vm, const HashedString &functionName);

	template <typename ReturnType>
	CallerStackFrame(VM &vm, const HashedString &functionName, ReturnType *returnPointer);

	template <typename ArgType>
	void PushArg(const ArgType &arg);

	void Call();

private:
	void Initialize(VM &vm, const HashedString &functionName, void *returnPointer);

	uint32_t mNextArg;
};


class VM
{
public:
	VM(
			Allocator &allocator,
			const CodeSegment &codeSegment,
			size_t stackSize,
			OutputStream *stdOut = nullptr,
			OutputStream *stdErr = nullptr);
	~VM();

	Allocator &GetAllocator() const { return mAllocator; }

	const CodeSegment &GetCodeSegment() const { return mCodeSegment; }

	CalleeStackFrame &GetTopStackFrame() { return mStackFrames.GetTop(); }
	const CalleeStackFrame &GetTopStackFrame() const { return mStackFrames.GetTop(); }

	OutputStream *GetStdOut() const { return mStdOut; }
	OutputStream *GetStdErr() const { return mStdErr; }

	void DumpCallStack(OutputStream &stream) const;
	void DumpStackFrame(OutputStream &stream, const CalleeStackFrame &frame) const;

	void RaiseError(const char *format, ...) const;

	template <typename ReturnType>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress);

	void CallVoidFunction(const HashedString &functionName);

	template <typename ReturnType, typename ArgType0>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress, ArgType0 a0);

	template <typename ArgType0>
	void CallVoidFunction(const HashedString &functionName, ArgType0 a0);

	template <typename ReturnType, typename ArgType0, typename ArgType1>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress,
		ArgType0 a0, ArgType1 a1);

	template <typename ArgType0, typename ArgType1>
	void CallVoidFunction(const HashedString &functionName, ArgType0 a0, ArgType1 a1);

	template <typename ReturnType, typename ArgType0, typename ArgType1, typename ArgType2>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress,
		ArgType0 a0, ArgType1 a1, ArgType2 a2);

	template <typename ArgType0, typename ArgType1, typename ArgType2>
	void CallVoidFunction(const HashedString &functionName, ArgType0 a0, ArgType1 a1, ArgType2 a2);

	template <typename ReturnType, typename ArgType0, typename ArgType1, typename ArgType2,
		typename ArgType3>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress,
		ArgType0 a0, ArgType1 a1, ArgType2 a2, ArgType3 a3);

	template <typename ArgType0, typename ArgType1, typename ArgType2, typename ArgType3>
	void CallVoidFunction(const HashedString &functionName, ArgType0 a0, ArgType1 a1,
		ArgType2 a2, ArgType3 a3);

	template <typename ReturnType, typename ArgType0, typename ArgType1, typename ArgType2,
		typename ArgType3, typename ArgType4>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress,
		ArgType0 a0, ArgType1 a1, ArgType2 a2, ArgType3 a3, ArgType4 a4);

	template <typename ArgType0, typename ArgType1, typename ArgType2, typename ArgType3,
		typename ArgType4>
	void CallVoidFunction(const HashedString &functionName, ArgType0 a0, ArgType1 a1,
		ArgType2 a2, ArgType3 a3, ArgType4 a4);

private:
	friend class CallerStackFrame;

	void ExecuteScriptFunction();
	uint8_t *InvokeFunction(const Function *function, uint8_t *stackTop);
	void ValidateStackPointer(uint8_t *stackPointer) const;

	StackFrames mStackFrames;
	StackFrames::Element mDummyFrame;
	Allocator &mAllocator;
	const CodeSegment &mCodeSegment;
	OutputStream *mStdOut;
	OutputStream *mStdErr;
	uint8_t *mStack;
	size_t mStackSize;
};

}

#include "bond/private/vm.h"

#endif
