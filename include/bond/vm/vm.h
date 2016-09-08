#ifndef BOND_VM_VM_H
#define BOND_VM_VM_H

#include "bond/io/inputstreamadaptor.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/stl/autostack.h"
#include "bond/systems/allocator.h"
#include "bond/systems/assert.h"
#include "bond/vm/codesegment.h"
#include "bond/vm/collector.h"

namespace Bond
{

class StackFrame;
class InvocationStackFrame;
class VM;

/// \addtogroup vm
/// @{

class StackFrame
{
public:
	StackFrame(VM &vm):
		mVm(vm),
		mFunction(nullptr),
		mFramePointer(nullptr),
		mStackPointer(nullptr),
		mReturnPointer(nullptr)
	{}

	StackFrame(
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

	StackFrame(const StackFrame &other) = delete;
	StackFrame &operator=(const StackFrame &other) = delete;

	template <typename ArgType>
	const ArgType &GetArg(size_t index) const;

	template <typename ReturnType>
	void SetReturnValue(const ReturnType &returnValue);

	VM &GetVM() { return mVm; }
	const VM &GetVM() const { return mVm; }

	Collector &GetCollector() { return mCollector; }
	const Collector &GetCollector() const { return mCollector; }

private:
	friend class VM;
	friend class InvocationStackFrame;

	template <typename ArgType>
	ArgType &GetArgRef(size_t index) const;

	template <typename ReturnType>
	void AssertValidReturnAssignmentType() const;

	Collector mCollector;
	VM &mVm;
	const Function *mFunction;
	uint8_t *mFramePointer;
	uint8_t *mStackPointer;
	uint8_t *mReturnPointer;
};

typedef AutoStack<StackFrame> StackFrames;


class InvocationStackFrame: private StackFrames::Element
{
public:
	InvocationStackFrame(VM &vm, const QualifiedName &functionName);
	InvocationStackFrame(VM &vm, const char *functionName);
	InvocationStackFrame(VM &vm, const Function &function);

	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const QualifiedName &functionName, ReturnType *returnPointer);

	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const char *functionName, ReturnType *returnPointer);

	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const Function &function, ReturnType *returnPointer);

	template <typename ArgType>
	void PushArg(const ArgType &arg);

	void PushArgs() {}

	template <typename Arg1, typename... Args>
	void PushArgs(Arg1 arg1, Args... args);

	void Call();

	InvocationStackFrame &operator=(const InvocationStackFrame &other) = delete;

private:
	void Initialize(VM &vm, const QualifiedName &functionName, void *returnPointer);
	void Initialize(VM &vm, const char *functionName, void *returnPointer);
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

	StackFrame *GetStackFrame() { return &mStackFrames.GetTop(); }
	const StackFrame &GetStackFrame() const { return mStackFrames.GetTop(); }

	StackFrame &GetCallerStackFrame();
	const StackFrame &GetCallerStackFrame() const;

	InputStreamAdaptor &GetStdIn() { return mStdIn; }
	OutputStreamAdaptor &GetStdOut() { return mStdOut; }
	OutputStreamAdaptor &GetStdErr() { return mStdErr; }

	void DumpCallStack(OutputStream &stream) const;
	void DumpStackFrame(OutputStream &stream, const StackFrame &frame) const;

	void RaiseError(const char *format, ...) const;

	template <typename ReturnType, typename... Args>
	void CallFunction(const QualifiedName &functionName, ReturnType *returnAddress, Args... args);

	template <typename ReturnType, typename... Args>
	void CallFunction(const char *functionName, ReturnType *returnAddress, Args... args);

	template <typename ReturnType, typename... Args>
	void CallFunction(const Function &function, ReturnType *returnAddress, Args... args);

	template <typename... Args>
	void CallVoidFunction(const QualifiedName &functionName, Args... args);

	template <typename... Args>
	void CallVoidFunction(const char *functionName, Args... args);

	template <typename... Args>
	void CallVoidFunction(const Function &function, Args... args);

	VM &operator=(const VM &other) = delete;

private:
	friend class InvocationStackFrame;

	void ExecuteScriptFunction(StackFrame &frame);
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

/// @}

}

#include "bond/private/vm.h"

#endif
