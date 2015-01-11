#ifndef BOND_VM_VM_H
#define BOND_VM_VM_H

#include "bond/io/inputstreamadaptor.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/stl/autostack.h"
#include "bond/systems/allocator.h"
#include "bond/systems/assert.h"
#include "bond/vm/codesegment.h"

namespace Bond
{

class CalleeStackFrame;
class CallerStackFrame;
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
			const Function &function,
			uint8_t *framePointer,
			uint8_t *stackPointer,
			uint8_t *returnPointer):
		mVm(vm),
		mFunction(&function),
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
	CallerStackFrame(VM &vm, const Function &function);

	template <typename ReturnType>
	CallerStackFrame(VM &vm, const HashedString &functionName, ReturnType *returnPointer);

	template <typename ReturnType>
	CallerStackFrame(VM &vm, const Function &function, ReturnType *returnPointer);

	template <typename ArgType>
	void PushArg(const ArgType &arg);

	void PushArgs() {}

	template <typename Arg1, typename... Args>
	void PushArgs(Arg1 arg1, Args... args);

	void Call();

private:
	void Initialize(VM &vm, const HashedString &functionName, void *returnPointer);
	void Initialize(VM &vm, const Function &function, void *returnPointer);

	uint32_t mNextArg;
};


class VM
{
public:
	VM(
			Allocator &allocator,
			const CodeSegment &codeSegment,
			size_t stackSize,
			InputStream *stdIn = nullptr,
			OutputStream *stdOut = nullptr,
			OutputStream *stdErr = nullptr);
	~VM();

	Allocator &GetAllocator() const { return mAllocator; }

	const CodeSegment &GetCodeSegment() const { return mCodeSegment; }

	CalleeStackFrame &GetTopStackFrame() { return mStackFrames.GetTop(); }
	const CalleeStackFrame &GetTopStackFrame() const { return mStackFrames.GetTop(); }

	InputStreamAdaptor &GetStdIn() { return mStdIn; }
	OutputStreamAdaptor &GetStdOut() { return mStdOut; }
	OutputStreamAdaptor &GetStdErr() { return mStdErr; }

	void DumpCallStack(OutputStream &stream) const;
	void DumpStackFrame(OutputStream &stream, const CalleeStackFrame &frame) const;

	void RaiseError(const char *format, ...) const;

	template <typename ReturnType, typename... Args>
	void CallFunction(const HashedString &functionName, ReturnType *returnAddress, Args... args);

	template <typename ReturnType, typename... Args>
	void CallFunction(const Function &function, ReturnType *returnAddress, Args... args);

	template <typename... Args>
	void CallVoidFunction(const HashedString &functionName, Args... args);

	template <typename... Args>
	void CallVoidFunction(const Function &function, Args... args);

private:
	friend class CallerStackFrame;

	void ExecuteScriptFunction();
	uint8_t *InvokeFunction(const Function &function, uint8_t *stackTop);
	void ValidateStackPointer(uint8_t *stackPointer) const;

	StackFrames mStackFrames;
	StackFrames::Element mDummyFrame;
	Allocator &mAllocator;
	const CodeSegment &mCodeSegment;
	InputStreamAdaptor mStdIn;
	OutputStreamAdaptor mStdOut;
	OutputStreamAdaptor mStdErr;
	Allocator::Handle<uint8_t> mStack;
	size_t mStackSize;
};

}

#include "bond/private/vm.h"

#endif
