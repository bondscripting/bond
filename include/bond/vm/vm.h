#ifndef BOND_VM_VM_H
#define BOND_VM_VM_H

#include "bond/conf.h"
#include "bond/io/inputstreamadaptor.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/stl/autostack.h"
#include "bond/systems/allocator.h"
#include "bond/systems/error.h"
#include "bond/vm/codesegment.h"
#include "bond/vm/collector.h"

namespace Bond
{

class StackFrame;
class StreamFactory;
class InvocationStackFrame;
class VM;

/// \addtogroup vm
/// @{

/// \brief The activation record of a function invoked by a VM.
///
/// A StackFrame gives access to the arguments and return value of a function call. Native
/// functions receive the StackFrame of their own invocation, through which they read their
/// arguments with GetArg() and set their return value with SetReturnValue(). StackFrames are
/// created and destroyed by the VM and by InvocationStackFrame; they are not constructed directly
/// by applications.
///
/// \sa InvocationStackFrame, NativeFunction, VM
class BOND_API StackFrame
{
public:
	/// \brief Constructs a StackFrame object that is not associated with any function.
	/// \param vm The VM that owns the stack frame.
	explicit StackFrame(VM &vm):
		mVm(vm),
		mFunction(nullptr),
		mFramePointer(nullptr),
		mStackPointer(nullptr),
		mReturnPointer(nullptr)
	{}

	/// \brief Constructs a StackFrame object for an invocation of the given function.
	/// \param vm The VM that owns the stack frame.
	/// \param function The function being invoked.
	/// \param framePointer Pointer to the base of the function's frame on the VM stack, relative to
	///        which its arguments and local variables are addressed.
	/// \param stackPointer Pointer to the top of the function's frame on the VM stack.
	/// \param returnPointer Pointer to where the function's return value is written, or null if the
	///        function returns void.
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

	/// \brief Returns a reference to an argument of the invoked function.
	///
	/// When BOND_RUNTIME_CHECKS_ENABLED is non-zero, a Bond::Exception is thrown if the index is
	/// out of range or if ArgType does not match the type of the parameter. Integer types match any
	/// Bond integer type of the same size, regardless of signedness.
	/// \tparam ArgType The native type of the argument.
	/// \param index The zero-based index of the argument.
	template <typename ArgType>
	const ArgType &GetArg(size_t index) const;

	/// \brief Sets the return value of the invoked function.
	///
	/// Integers smaller than 32 bits are widened to 32 bits according to the Bond return type, and
	/// enums are stored as their underlying type. When BOND_RUNTIME_CHECKS_ENABLED is non-zero, a
	/// Bond::Exception is thrown if ReturnType does not match the return type of the function.
	/// \tparam ReturnType The native type of the return value.
	/// \param returnValue The value to be returned.
	template <typename ReturnType>
	void SetReturnValue(const ReturnType &returnValue);

	/// \brief Returns the VM that owns the stack frame.
	VM &GetVM() { return mVm; }

	/// \brief Returns the VM that owns the stack frame.
	const VM &GetVM() const { return mVm; }

	/// \brief Returns the Collector whose Collectable objects are destroyed when the stack frame
	/// is popped off of the VM's call stack.
	///
	/// A native function that allocates memory or objects on behalf of the calling Bond function
	/// typically registers them with the Collector of the caller's stack frame, obtained with
	/// VM::GetCallerStackFrame(), so that they live until the caller returns.
	Collector &GetCollector() { return mCollector; }

	/// \brief Returns the Collector whose Collectable objects are destroyed when the stack frame
	/// is popped off of the VM's call stack.
	const Collector &GetCollector() const { return mCollector; }

private:
	friend class VM;
	friend class InvocationStackFrame;

	template <typename ArgType>
	ArgType &GetArgRef(size_t index) const;

	template <typename ReturnType>
	void ValidateReturnAssignmentType() const;

	Collector mCollector;
	VM &mVm;
	const Function *mFunction;
	uint8_t *mFramePointer;
	uint8_t *mStackPointer;
	uint8_t *mReturnPointer;
};

/// \brief The call stack of a VM.
typedef AutoStack<StackFrame> StackFrames;


/// \brief Sets up and invokes a call to a Bond function from native code.
///
/// Constructing an InvocationStackFrame pushes a new StackFrame onto the VM's call stack, and
/// destroying it pops the frame off. Arguments are pushed in order with PushArg() or PushArgs(),
/// after which the function is invoked with Call(). Since the frame is popped by the destructor,
/// InvocationStackFrames must be destroyed in the reverse order of their construction, which is
/// naturally the case when they are local variables.
///
/// The constructors that look up a function by name throw a Bond::Exception if no such function
/// exists in the VM's CodeSegment, and all constructors throw a Bond::Exception if the frame does
/// not fit on the VM stack. When BOND_RUNTIME_CHECKS_ENABLED is non-zero, the constructors also
/// validate the return type, PushArg() validates the argument types and count, and Call()
/// validates that all arguments have been pushed. Violations throw a Bond::Exception.
///
/// VM::CallFunction and VM::CallVoidFunction are convenient alternatives for calls whose
/// arguments are all known up front.
///
/// \sa StackFrame, VM
class BOND_API InvocationStackFrame: private StackFrames::Element
{
public:
	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns void.
	/// \param vm The VM on which the function is invoked.
	/// \param functionName The fully qualified name of the function.
	InvocationStackFrame(VM &vm, const QualifiedName &functionName);

	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns void.
	/// \param vm The VM on which the function is invoked.
	/// \param functionName The fully qualified name of the function with its elements separated by
	///        the '.' character, e.g. "Namespace.Function".
	InvocationStackFrame(VM &vm, const char *functionName);

	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns void.
	/// \param vm The VM on which the function is invoked.
	/// \param function The function to be invoked. It must belong to the VM's CodeSegment.
	InvocationStackFrame(VM &vm, const Function &function);

	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns a
	/// value.
	/// \tparam ReturnType The native type of the return value. Integers smaller than 32 bits are
	///         returned widened to 32 bits, so they must be received in an int32_t or uint32_t.
	/// \param vm The VM on which the function is invoked.
	/// \param functionName The fully qualified name of the function.
	/// \param returnPointer Pointer to where the return value is written. It must remain valid
	///        until Call() returns.
	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const QualifiedName &functionName, ReturnType *returnPointer);

	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns a
	/// value.
	/// \tparam ReturnType The native type of the return value. Integers smaller than 32 bits are
	///         returned widened to 32 bits, so they must be received in an int32_t or uint32_t.
	/// \param vm The VM on which the function is invoked.
	/// \param functionName The fully qualified name of the function with its elements separated by
	///        the '.' character, e.g. "Namespace.Function".
	/// \param returnPointer Pointer to where the return value is written. It must remain valid
	///        until Call() returns.
	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const char *functionName, ReturnType *returnPointer);

	/// \brief Constructs an InvocationStackFrame object for a call to a function that returns a
	/// value.
	/// \tparam ReturnType The native type of the return value. Integers smaller than 32 bits are
	///         returned widened to 32 bits, so they must be received in an int32_t or uint32_t.
	/// \param vm The VM on which the function is invoked.
	/// \param function The function to be invoked. It must belong to the VM's CodeSegment.
	/// \param returnPointer Pointer to where the return value is written. It must remain valid
	///        until Call() returns.
	template <typename ReturnType>
	InvocationStackFrame(VM &vm, const Function &function, ReturnType *returnPointer);

	/// \brief Pushes the next argument of the function call.
	///
	/// Enums are passed as their underlying type and nullptr is passed as a null void pointer.
	/// \tparam ArgType The native type of the argument.
	/// \param arg The value of the argument.
	template <typename ArgType>
	void PushArg(const ArgType &arg);

	/// \brief Pushes the next arguments of the function call, in order, as if by calling PushArg()
	/// for each of them.
	/// \param args The values of the arguments.
	template <typename... Args>
	void PushArgs(Args... args);

	/// \brief Invokes the function. Must be called after all of its arguments have been pushed.
	void Call();

	InvocationStackFrame &operator=(const InvocationStackFrame &other) = delete;

private:
	void Initialize(VM &vm, const QualifiedName &functionName, void *returnPointer);
	void Initialize(VM &vm, const char *functionName, void *returnPointer);
	void Initialize(VM &vm, const Function &function, void *returnPointer);

	uint32_t mNextArg;
};


/// \brief A virtual machine that executes the Bond bytecode in a CodeSegment.
///
/// A VM owns a stack of a fixed size on which the frames of invoked functions are allocated. It
/// executes Bond functions by interpreting their bytecode and invokes native functions through
/// their bound NativeFunction. Functions are invoked from native code with CallFunction(),
/// CallVoidFunction() or an InvocationStackFrame. Errors detected while executing code, such as a
/// stack overflow, are reported by throwing a Bond::Exception whose message includes a dump of the
/// call stack.
///
/// The VM does not call the static initializers of the CodeSegment; an application typically
/// calls CodeSegment::CallStaticInitializers() before invoking its entry point.
///
/// \sa CboLoader, CodeSegment, InvocationStackFrame, StackFrame
class BOND_API VM
{
public:
	/// \brief Constructs a VM object.
	/// \param allocator The memory allocator from which the VM stack is allocated. It is also
	///        available to native functions via GetAllocator().
	/// \param codeSegment The CodeSegment containing the functions and data executed by the VM. It
	///        is not copied, so it must remain valid for the lifetime of the VM.
	/// \param stackSize The size of the VM stack in bytes.
	/// \param stdIn The stream wrapped by the adaptor returned by GetStdIn(), or null.
	/// \param stdOut The stream wrapped by the adaptor returned by GetStdOut(), or null.
	/// \param stdErr The stream wrapped by the adaptor returned by GetStdErr(), or null.
	/// \param streamFactory The factory used by the Bond Standard Library to open files, or null.
	///        None of the streams nor the factory are owned by the VM, so they must remain valid
	///        for as long as the VM uses them.
	VM(
			Allocator &allocator,
			const CodeSegment &codeSegment,
			size_t stackSize,
			InputStream *stdIn = nullptr,
			OutputStream *stdOut = nullptr,
			OutputStream *stdErr = nullptr,
			StreamFactory *streamFactory = nullptr);

	/// \brief Destroys the VM and frees its stack.
	~VM();

	/// \brief Returns the memory allocator used by the VM.
	Allocator &GetAllocator() const { return mAllocator; }

	/// \brief Returns the CodeSegment executed by the VM.
	const CodeSegment &GetCodeSegment() const { return mCodeSegment; }

	/// \brief Returns a pointer to the StackFrame at the top of the call stack, which is that of
	/// the function currently being executed.
	StackFrame *GetStackFrame() { return &mStackFrames.GetTop(); }

	/// \brief Returns the StackFrame at the top of the call stack, which is that of the function
	/// currently being executed.
	const StackFrame &GetStackFrame() const { return mStackFrames.GetTop(); }

	/// \brief Returns the StackFrame immediately below the top of the call stack, which is that of
	/// the caller of the function currently being executed.
	///
	/// Throws a Bond::Exception if there is no such frame.
	StackFrame &GetCallerStackFrame();

	/// \brief Returns the StackFrame immediately below the top of the call stack, which is that of
	/// the caller of the function currently being executed.
	///
	/// Throws a Bond::Exception if there is no such frame.
	const StackFrame &GetCallerStackFrame() const;

	/// \brief Returns the adaptor wrapping the standard input stream given to the constructor.
	InputStreamAdaptor &GetStdIn() { return mStdIn; }

	/// \brief Returns the adaptor wrapping the standard output stream given to the constructor.
	OutputStreamAdaptor &GetStdOut() { return mStdOut; }

	/// \brief Returns the adaptor wrapping the standard error stream given to the constructor.
	OutputStreamAdaptor &GetStdErr() { return mStdErr; }

	/// \brief Returns the StreamFactory given to the constructor, or null if there is none.
	StreamFactory *GetStreamFactory() { return mStreamFactory; }

	/// \brief Writes a description of every StackFrame on the call stack, from the top down, to
	/// the given stream.
	/// \param stream The stream to which the call stack is written.
	void DumpCallStack(OutputStream &stream) const;

	/// \brief Writes a description of the given StackFrame, consisting of the signature of its
	/// function and the values of its arguments, to the given stream.
	/// \param stream The stream to which the stack frame is written.
	/// \param frame The StackFrame to be written. Nothing is written if it is not associated with a
	///        function.
	void DumpStackFrame(OutputStream &stream, const StackFrame &frame) const;

	/// \brief Throws a Bond::Exception whose message is the formatted error message followed by a
	/// dump of the call stack.
	/// \param format A printf-style format string for the error message.
	[[noreturn]] void RaiseError(const char *format, ...) const;

	/// \brief Invokes a function that returns a value with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param functionName The fully qualified name of the function.
	/// \param returnAddress Pointer to where the return value is written.
	/// \param args The arguments passed to the function.
	template <typename ReturnType, typename... Args>
	void CallFunction(const QualifiedName &functionName, ReturnType *returnAddress, Args... args);

	/// \brief Invokes a function that returns a value with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param functionName The fully qualified name of the function with its elements separated by
	///        the '.' character, e.g. "Namespace.Function".
	/// \param returnAddress Pointer to where the return value is written.
	/// \param args The arguments passed to the function.
	template <typename ReturnType, typename... Args>
	void CallFunction(const char *functionName, ReturnType *returnAddress, Args... args);

	/// \brief Invokes a function that returns a value with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param function The function to be invoked. It must belong to the VM's CodeSegment.
	/// \param returnAddress Pointer to where the return value is written.
	/// \param args The arguments passed to the function.
	template <typename ReturnType, typename... Args>
	void CallFunction(const Function &function, ReturnType *returnAddress, Args... args);

	/// \brief Invokes a function that returns void with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param functionName The fully qualified name of the function.
	/// \param args The arguments passed to the function.
	template <typename... Args>
	void CallVoidFunction(const QualifiedName &functionName, Args... args);

	/// \brief Invokes a function that returns void with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param functionName The fully qualified name of the function with its elements separated by
	///        the '.' character, e.g. "Namespace.Function".
	/// \param args The arguments passed to the function.
	template <typename... Args>
	void CallVoidFunction(const char *functionName, Args... args);

	/// \brief Invokes a function that returns void with the given arguments.
	///
	/// Equivalent to constructing an InvocationStackFrame, pushing the arguments with
	/// InvocationStackFrame::PushArgs() and calling InvocationStackFrame::Call().
	/// \param function The function to be invoked. It must belong to the VM's CodeSegment.
	/// \param args The arguments passed to the function.
	template <typename... Args>
	void CallVoidFunction(const Function &function, Args... args);

	VM &operator=(const VM &other) = delete;

private:
	friend class InvocationStackFrame;

	void ExecuteScriptFunction(StackFrame &frame);
	template <bool UNPACK_ARGUMENTS>
	uint8_t *InvokeFunction(const Function &function, uint8_t *stackTop);
	void ValidateStackPointer(uint8_t *stackPointer) const;

	StackFrames mStackFrames;
	StackFrames::Element mDummyFrame;
	Allocator &mAllocator;
	const CodeSegment &mCodeSegment;
	InputStreamAdaptor mStdIn;
	OutputStreamAdaptor mStdOut;
	OutputStreamAdaptor mStdErr;
	StreamFactory *mStreamFactory;
	Allocator::Handle<uint8_t> mStack;
	size_t mStackSize;
};

/// @}

}

#include "bond/private/vm.h"

#endif
