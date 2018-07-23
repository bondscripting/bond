#include "bond/io/memoryoutputstream.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/math.h"
#include "bond/types/opcodes.h"
#include "bond/vm/vm.h"
#include <cstring>

namespace Bond
{

struct MatchOffsetPair
{
	MatchOffsetPair(): match(0), offset(0) {}
	explicit MatchOffsetPair(int32_t m): match(m), offset(0) {}
	int32_t match;
	int32_t offset;

	bool operator<(const MatchOffsetPair &other) const
	{
		return match < other.match;
	}
};


template <typename SourceType, typename DestType>
inline void CopyValue(const void *source, void *dest)
{
	// Expects the source and dest pointers to be appropriately aligned.
	*reinterpret_cast<DestType *>(dest) = static_cast<DestType>(*reinterpret_cast<const SourceType *>(source));
}


inline void CopyValue32(const void *source, void *dest)
{
	// Expects the source and dest pointers to be 32 bit aligned.
	*reinterpret_cast<uint32_t *>(dest) = *reinterpret_cast<const uint32_t *>(source);
}


inline void CopyValue64(const void *source, void *dest)
{
	// Expects the source and dest pointers to be 64 bit aligned.
	*reinterpret_cast<uint64_t *>(dest) = *reinterpret_cast<const uint64_t *>(source);
}


inline void SwapValue64(void *a, void *b)
{
	// Expects the pointers to be 64 bit aligned.
	const uint64_t temp = *reinterpret_cast<uint64_t *>(a);
	*reinterpret_cast<uint64_t *>(a) = *reinterpret_cast<const uint64_t *>(b);
	*reinterpret_cast<uint64_t *>(b) = temp;
}


void InvocationStackFrame::Initialize(VM &vm, const QualifiedName &functionName, void *returnPointer)
{
	const CodeSegment &codeSegment = vm.GetCodeSegment();
	const Function *function = codeSegment.GetFunction(functionName);

	if (function == nullptr)
	{
		char buffer[Exception::MESSAGE_BUFFER_LENGTH];
		MemoryOutputStream stream(buffer, Stream::pos_t(Exception::MESSAGE_BUFFER_LENGTH));
		functionName.PrintTo(stream);
		BOND_FAIL_FORMAT(("Failed to look up function '%s'.", buffer));
	}

	Initialize(vm, *function, returnPointer);
}


void InvocationStackFrame::Initialize(VM &vm, const char *functionName, void *returnPointer)
{
	const CodeSegment &codeSegment = vm.GetCodeSegment();
	const Function *function = codeSegment.GetFunction(functionName);
	BOND_ASSERT_FORMAT(function != nullptr, ("Failed to look up function '%s'.", functionName));
	Initialize(vm, *function, returnPointer);
}


void InvocationStackFrame::Initialize(VM &vm, const Function &function, void *returnPointer)
{
	uint8_t *prevStackPointer = GetNext()->GetValue().mStackPointer;
	uint8_t *framePointer = AlignPointerUp(prevStackPointer + function.mArgSize, function.mFramePointerAlignment);
	uint8_t *stackPointer = AlignPointerUp(framePointer + function.mLocalSize, BOND_SLOT_SIZE);
	mValue.mFunction = &function;
	mValue.mFramePointer = framePointer;
	mValue.mStackPointer = stackPointer;
	mValue.mReturnPointer = reinterpret_cast<uint8_t *>(returnPointer);
	vm.ValidateStackPointer(stackPointer);
}


void InvocationStackFrame::Call()
{
	StackFrame &frame = GetValue();
	const Function *function = frame.mFunction;

#if BOND_RUNTIME_CHECKS_ENABLED
	if (mNextArg < function->mParamListSignature.mParamCount)
	{
		frame.mVm.RaiseError("Attempt to call function with too few arguments.");
	}
#endif

	if (function->IsNative())
	{
		function->mNativeFunction(frame);
	}
	else
	{
		frame.mVm.ExecuteScriptFunction(frame);
	}
}


VM::VM(
		Allocator &allocator,
		const CodeSegment &codeSegment,
		size_t stackSize,
		InputStream *stdIn,
		OutputStream *stdOut,
		OutputStream *stdErr,
		StreamFactory *streamFactory):
	mStackFrames(),
	mDummyFrame(mStackFrames, *this),
	mAllocator(allocator),
	mCodeSegment(codeSegment),
	mStdIn(stdIn),
	mStdOut(stdOut),
	mStdErr(stdErr),
	mStreamFactory(streamFactory),
	mStack(allocator.AllocOwned<uint8_t>(stackSize)),
	mStackSize(stackSize)
{
	StackFrame &top = mDummyFrame.GetValue();
	top.mFunction = nullptr;
	top.mStackPointer = mStack.get();
	top.mFramePointer = mStack.get();
	top.mReturnPointer = nullptr;
}


VM::~VM()
{
}


void VM::ExecuteScriptFunction(StackFrame &frame)
{
	const ConstantTable *constantTable = frame.mFunction->mConstantTable;
	const Value32 *value32Table = constantTable->mValue32Table;
	const uint8_t *code = frame.mFunction->mCode;
	uint8_t *const fp = frame.mFramePointer;
	uint8_t *sp = frame.mStackPointer;
	size_t pc = 0;

	for (;;)
	{
		const OpCode opCode = static_cast<OpCode>(code[pc++]);
		switch (opCode)
		{
			case OPCODE_CONSTC:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(static_cast<int8_t>(code[pc++]));
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTUC:
			{
				*reinterpret_cast<uint32_t *>(sp) = static_cast<uint32_t>(code[pc++]);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTS:
			{
				*reinterpret_cast<int32_t *>(sp) = Value16(code + pc).mShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTUS:
			{
				*reinterpret_cast<uint32_t *>(sp) = Value16(code + pc).mUShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONST32:
			{
				const Value16 valueIndex(code + pc);
				CopyValue32(value32Table + valueIndex.mUShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONST64:
			{
				const Value16 valueIndex(code + pc);
				CopyValue64(constantTable->mValue64Table + valueIndex.mUShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_N2:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(-2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_N1:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(-1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_0:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_1:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_2:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_3:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(3);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_4:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(4);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_8:
			{
				*reinterpret_cast<int32_t *>(sp) = static_cast<int32_t>(8);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_N2:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(-2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_N1:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(-1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_0:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_1:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_2:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_3:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(3);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_4:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(4);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_8:
			{
				*reinterpret_cast<int64_t *>(sp) = static_cast<int64_t>(8);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_N2:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(-2.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_N1:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(-1.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_NH:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(-0.5f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_0:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(0.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_H:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(0.5f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_1:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(1.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_2:
			{
				*reinterpret_cast<float *>(sp) = static_cast<float>(2.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_N2:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(-2.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_N1:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(-1.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_NH:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(-0.5);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_0:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(0.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_H:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(0.5);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_1:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(1.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_2:
			{
				*reinterpret_cast<double *>(sp) = static_cast<double>(2.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADFP:
			{
				const Value16 offset(code + pc);
				*reinterpret_cast<uint8_t **>(sp) = fp + offset.mShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADFPW:
			{
				const Value16 offsetIndex(code + pc);
				*reinterpret_cast<uint8_t **>(sp) = fp + value32Table[offsetIndex.mUShort].mInt;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADEA:
			{
				const Value16 dataIndex(code + pc);
				const DataEntry &dataEntry = mCodeSegment.GetDataEntryAtIndex(dataIndex.mUShort);
				*reinterpret_cast<void **>(sp) = dataEntry.mData;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADSTR:
			{
				const Value16 stringIndex(code + pc);
				*reinterpret_cast<const char **>(sp) = constantTable->mStringTable[stringIndex.mUShort].GetString();
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADC:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<int8_t, int32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADUC:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<uint8_t, uint32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADS:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<int16_t, int32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADUS:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<uint16_t, uint32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOAD32:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue32(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOAD64:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue64(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADMEMW:
			{
				const Value16 memSizeIndex(code + pc);
				const size_t memSize = size_t(value32Table[memSizeIndex.mUShort].mInt);
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				memcpy(sp - BOND_SLOT_SIZE, address, memSize);
				pc += sizeof(Value16);
				sp = AlignPointerUp(sp - BOND_SLOT_SIZE + memSize, BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_STOREC:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue<int32_t, int8_t>(sp - BOND_SLOT_SIZE, address);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_STORES:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue<int32_t, int16_t>(sp - BOND_SLOT_SIZE, address);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_STORE32:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue32(sp - BOND_SLOT_SIZE, address);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_STORE64:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue64(sp - BOND_SLOT_SIZE, address);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHC:
			{
				const Value16 offset(code + pc);
				CopyValue<int8_t, int32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<int8_t, int32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUC:
			{
				const Value16 offset(code + pc);
				CopyValue<uint8_t, uint32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<uint8_t, uint32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHS:
			{
				const Value16 offset(code + pc);
				CopyValue<int16_t, int32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<int16_t, int32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUS:
			{
				const Value16 offset(code + pc);
				CopyValue<uint16_t, uint32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<uint16_t, uint32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32:
			{
				const Value16 offset(code + pc);
				CopyValue32(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32W:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue32(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_P0:
			{
				CopyValue32(fp - (1 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_P1:
			{
				CopyValue32(fp - (2 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_P2:
			{
				CopyValue32(fp - (3 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_P3:
			{
				CopyValue32(fp - (4 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_L0:
			{
				CopyValue32(fp + (0 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_L1:
			{
				CopyValue32(fp + (1 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_L2:
			{
				CopyValue32(fp + (2 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH32_L3:
			{
				CopyValue32(fp + (3 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64:
			{
				const Value16 offset(code + pc);
				CopyValue64(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64W:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue64(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_P0:
			{
				CopyValue64(fp - (1 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_P1:
			{
				CopyValue64(fp - (2 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_P2:
			{
				CopyValue64(fp - (3 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_P3:
			{
				CopyValue64(fp - (4 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_L0:
			{
				CopyValue64(fp + (0 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_L1:
			{
				CopyValue64(fp + (1 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_L2:
			{
				CopyValue64(fp + (2 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSH64_L3:
			{
				CopyValue64(fp + (3 * BOND_SLOT_SIZE), sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP:
			{
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPC:
			{
				const Value16 offset(code + pc);
				CopyValue<int32_t, int8_t>(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<int32_t, int8_t>(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPS:
			{
				const Value16 offset(code + pc);
				CopyValue<int32_t, int16_t>(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<int32_t, int16_t>(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32:
			{
				const Value16 offset(code + pc);
				CopyValue32(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32W:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue32(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_P0:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp - (1 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_P1:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp - (2 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_P2:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp - (3 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_P3:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp - (4 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_L0:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp + (0 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_L1:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp + (1 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_L2:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp + (2 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP32_L3:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, fp + (3 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64:
			{
				const Value16 offset(code + pc);
				CopyValue64(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64W:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue64(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_P0:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp - (1 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_P1:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp - (2 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_P2:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp - (3 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_P3:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp - (4 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_L0:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp + (0 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_L1:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp + (1 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_L2:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp + (2 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POP64_L3:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, fp + (3 * BOND_SLOT_SIZE));
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MEMCOPYW:
			{
				const Value16 memSizeIndex(code + pc);
				const size_t memSize = size_t(value32Table[memSizeIndex.mUShort].mInt);
				const void *sourceAddress = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				void *destAddress = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				memcpy(destAddress, sourceAddress, memSize);
				pc += sizeof(Value16);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MEMZEROW:
			{
				const Value16 memSizeIndex(code + pc);
				const size_t memSize = size_t(value32Table[memSizeIndex.mUShort].mInt);
				void *destAddress = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				memset(destAddress, 0, memSize);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DUP:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DUPINS:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, sp);
				CopyValue64(sp - (2 * BOND_SLOT_SIZE), sp - BOND_SLOT_SIZE);
				CopyValue64(sp, sp - (2 * BOND_SLOT_SIZE));
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SWAP:
			{
				SwapValue64(sp - (2 * BOND_SLOT_SIZE), sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_ITOC:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int8_t>(*a);
			}
			break;

			case OPCODE_UITOUC:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint8_t>(*a);
			}
			break;

			case OPCODE_ITOS:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int16_t>(*a);
			}
			break;

			case OPCODE_UITOUS:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint16_t>(*a);
			}
			break;

			case OPCODE_ITOL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				const int32_t *b = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*a = *b;
			}
			break;

			case OPCODE_UITOUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - BOND_SLOT_SIZE);
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				*a = *b;
			}
			break;

			case OPCODE_ITOF:
			{
				float *a = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				const int32_t *b = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<float>(*b);
			}
			break;

			case OPCODE_UITOF:
			{
				float *a = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<float>(*b);
			}
			break;

			case OPCODE_ITOD:
			{
				double *a = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				const int32_t *b = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<double>(*b);
			}
			break;

			case OPCODE_UITOD:
			{
				double *a = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<double>(*b);
			}
			break;

			case OPCODE_LTOI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const int64_t *b = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int32_t>(*b);
			}
			break;

			case OPCODE_LTOF:
			{
				float *a = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				const int64_t *b = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<float>(*b);
			}
			break;

			case OPCODE_ULTOF:
			{
				float *a = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<float>(*b);
			}
			break;

			case OPCODE_LTOD:
			{
				double *a = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				const int64_t *b = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<double>(*b);
			}
			break;

			case OPCODE_ULTOD:
			{
				double *a = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<double>(*b);
			}
			break;

			case OPCODE_FTOI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const float *b = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int32_t>(*b);
			}
			break;

			case OPCODE_FTOUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				const float *b = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint32_t>(*b);
			}
			break;

			case OPCODE_FTOL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				const float *b = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int64_t>(*b);
			}
			break;

			case OPCODE_FTOUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - BOND_SLOT_SIZE);
				const float *b = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint64_t>(*b);
			}
			break;

			case OPCODE_FTOD:
			{
				double *a = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				const float *b = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<double>(*b);
			}
			break;

			case OPCODE_DTOI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const double *b = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int32_t>(*b);
			}
			break;

			case OPCODE_DTOUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - BOND_SLOT_SIZE);
				const double *b = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint32_t>(*b);
			}
			break;

			case OPCODE_DTOL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - BOND_SLOT_SIZE);
				const double *b = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<int64_t>(*b);
			}
			break;

			case OPCODE_DTOUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - BOND_SLOT_SIZE);
				const double *b = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<uint64_t>(*b);
			}
			break;

			case OPCODE_DTOF:
			{
				float *a = reinterpret_cast<float *>(sp - BOND_SLOT_SIZE);
				const double *b = reinterpret_cast<double *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<float>(*b);
			}
			break;

			case OPCODE_PTROFF:
			{
				intptr_t *ptr = reinterpret_cast<intptr_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *offset = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				const Value16 elementSize(code + pc);
				*ptr += static_cast<intptr_t>(*offset) * static_cast<intptr_t>(elementSize.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PTRDIFF:
			{
				intptr_t *a = reinterpret_cast<intptr_t *>(sp - (2 * BOND_SLOT_SIZE));
				intptr_t *b = reinterpret_cast<intptr_t *>(sp - (1 * BOND_SLOT_SIZE));
				const Value16 elementSize(code + pc);
				*a = (*a - *b) / static_cast<intptr_t>(elementSize.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_INCI:
			{
				const uint32_t slotIndex = static_cast<uint32_t>(code[pc]);
				const int32_t value = static_cast<int32_t>(static_cast<int8_t>(code[pc + 1]));
				pc += 2;
				*reinterpret_cast<int32_t *>(fp + (slotIndex * BOND_SLOT_SIZE)) += value;
			}
			break;

			case OPCODE_INCL:
			{
				const uint32_t slotIndex = static_cast<uint32_t>(code[pc]);
				const int64_t value = static_cast<int64_t>(static_cast<int8_t>(code[pc + 1]));
				pc += 2;
				*reinterpret_cast<int64_t *>(fp + (slotIndex * BOND_SLOT_SIZE)) += value;
			}
			break;

			case OPCODE_ADDI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDF:
			{
				float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDD:
			{
				double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBF:
			{
				float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBD:
			{
				double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULF:
			{
				float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULD:
			{
				double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVF:
			{
				float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVD:
			{
				double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REML:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LSHI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a <<= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LSHL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a <<= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHUI:
			{
				uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHUL:
			{
				uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ANDI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a &= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ANDL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a &= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ORI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a |= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ORL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a |= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_XORI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a ^= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_XORL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a ^= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_NEGI:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGL:
			{
				int64_t *a = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGF:
			{
				float *a = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGD:
			{
				double *a = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NOT:
			{
				int32_t *a = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = !(*a);
			}
			break;

			case OPCODE_CMPEQI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQD:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQD:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUI:
			{
				const uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUL:
			{
				const uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTD:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUI:
			{
				const uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUL:
			{
				const uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLED:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUI:
			{
				const uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUL:
			{
				const uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTD:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEI:
			{
				const int32_t *a = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int32_t *b = reinterpret_cast<int32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUI:
			{
				const uint32_t *a = reinterpret_cast<uint32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint32_t *b = reinterpret_cast<uint32_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEL:
			{
				const int64_t *a = reinterpret_cast<int64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const int64_t *b = reinterpret_cast<int64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUL:
			{
				const uint64_t *a = reinterpret_cast<uint64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const uint64_t *b = reinterpret_cast<uint64_t *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEF:
			{
				const float *a = reinterpret_cast<float *>(sp - (2 * BOND_SLOT_SIZE));
				const float *b = reinterpret_cast<float *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGED:
			{
				const double *a = reinterpret_cast<double *>(sp - (2 * BOND_SLOT_SIZE));
				const double *b = reinterpret_cast<double *>(sp - (1 * BOND_SLOT_SIZE));
				int32_t *result = reinterpret_cast<int32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_NBRZ:
			{
				int32_t *condition = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*condition = int32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRZ:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				if (condition == 0)
				{
					pc += offset.mShort;
				}
				else
				{
					sp -= BOND_SLOT_SIZE;
				}
			}
			break;

			case OPCODE_NBRZW:
			{
				int32_t *condition = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*condition = int32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRZW:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				if (condition == 0)
				{
					pc += value32Table[offsetIndex.mUShort].mInt;
				}
				else
				{
					sp -= BOND_SLOT_SIZE;
				}
			}
			break;

			case OPCODE_NBRNZ:
			{
				int32_t *condition = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*condition = int32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRNZ:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				if (condition != 0)
				{
					pc += offset.mShort;
				}
				else
				{
					sp -= BOND_SLOT_SIZE;
				}
			}
			break;

			case OPCODE_NBRNZW:
			{
				int32_t *condition = reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				*condition = int32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRNZW:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				if (condition != 0)
				{
					pc += value32Table[offsetIndex.mUShort].mInt;
				}
				else
				{
					sp -= BOND_SLOT_SIZE;
				}
			}
			break;

			case OPCODE_IFZ:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
				if (condition == 0)
				{
					pc += offset.mShort;
				}
			}
			break;

			case OPCODE_IFZW:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
				if (condition == 0)
				{
					pc += value32Table[offsetIndex.mUShort].mInt;
				}
			}
			break;

			case OPCODE_IFNZ:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
				if (condition != 0)
				{
					pc += offset.mShort;
				}
			}
			break;

			case OPCODE_IFNZW:
			{
				const int32_t condition = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
				if (condition != 0)
				{
					pc += value32Table[offsetIndex.mUShort].mInt;
				}
			}
			break;

			case OPCODE_GOTO:
			{
				const Value16 offset(code + pc);
				pc += sizeof(Value16) + offset.mShort;
			}
			break;

			case OPCODE_GOTOW:
			{
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16) + value32Table[offsetIndex.mUShort].mInt;
			}
			break;

			case OPCODE_LOOKUPSWITCH:
			{
				pc = AlignUp(pc, sizeof(Value32));
				const int32_t defaultOffset = *reinterpret_cast<const int32_t *>(code + pc);
				const uint32_t numMatches = *reinterpret_cast<const uint32_t *>(code + pc + sizeof(Value32));
				const MatchOffsetPair *jumpTable =
					reinterpret_cast<const MatchOffsetPair *>(code + pc + (2 * sizeof(Value32)));
				const MatchOffsetPair *jumpTableEnd = jumpTable + numMatches;
				const MatchOffsetPair condition(*reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE));
				const MatchOffsetPair *pair = lower_bound(jumpTable, jumpTableEnd, condition);
				const int32_t offset =
					((pair < jumpTableEnd) && (pair->match == condition.match)) ? pair->offset : defaultOffset;
				pc += ((2 + (numMatches * 2)) * sizeof(Value32)) + offset;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_TABLESWITCH:
			{
				pc = AlignUp(pc, sizeof(Value32));
				const int32_t defaultOffset = *reinterpret_cast<const int32_t *>(code + pc);
				const int32_t minMatch = *reinterpret_cast<const uint32_t *>(code + pc + sizeof(Value32));
				const int32_t maxMatch = *reinterpret_cast<const uint32_t *>(code + pc + (2 * sizeof(Value32)));
				const int32_t *jumpTable = reinterpret_cast<const int32_t *>(code + pc + (3 * sizeof(Value32)));
				const int32_t index = *reinterpret_cast<int32_t *>(sp - BOND_SLOT_SIZE);
				const int32_t offset =
					((index >= minMatch) && (index <= maxMatch)) ? jumpTable[index - minMatch] : defaultOffset;
				pc += ((4 + maxMatch - minMatch) * sizeof(Value32)) + offset;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_INVOKE:
			{
				const Value16 functionIndex(code + pc);
				pc += sizeof(Value16);
				const Function &function = mCodeSegment.GetFunctionAtIndex(functionIndex.mUShort);
				sp = InvokeFunction(function, sp);
			}
			break;

			case OPCODE_RETURN:
				return;

			case OPCODE_RETURN32:
			{
				CopyValue32(sp - BOND_SLOT_SIZE, frame.mReturnPointer);
			}
			return;

			case OPCODE_RETURN64:
			{
				CopyValue64(sp - BOND_SLOT_SIZE, frame.mReturnPointer);
			}
			return;

			case OPCODE_RETURNMEMW:
			{
				const Value16 memSizeIndex(code + pc);
				const int32_t memSize = value32Table[memSizeIndex.mUShort].mInt;
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				memcpy(frame.mReturnPointer, address, memSize);
				//pc += sizeof(Value16);
				//sp = AlignPointerUp(sp - BOND_SLOT_SIZE + memSize, BOND_SLOT_SIZE);
			}
			return;

			case OPCODE_NOP: break;
			case OPCODE_MAX: break;
		}
	}
}


uint8_t *VM::InvokeFunction(const Function &function, uint8_t *stackTop)
{
	// Functions return values on top of the stack with the exception of functions that return
	// structs. Those functions expect an additional argument on the operand stack to indicate
	// where the value should be returned. So for functions that return a struct, we have to
	// pay particular attention to locating the top of the arguments, the return address and
	// where we leave the stack pointer when the function returns, since the return value is
	// not left on top of the stack.
	uint8_t *argTop;
	uint8_t *returnPointer;
	uint8_t *finalStackPointer;
	switch (function.mReturnSignature.mType)
	{
		case SIG_AGGREGATE:
			argTop = stackTop - BOND_SLOT_SIZE;
			returnPointer = *reinterpret_cast<uint8_t **>(argTop);
			finalStackPointer = argTop - function.mPackedArgSize;
			break;
		case SIG_VOID:
			argTop = stackTop;
			returnPointer = nullptr;
			finalStackPointer = argTop - function.mPackedArgSize;
			break;
		default:
			argTop = stackTop;
			returnPointer = argTop - function.mPackedArgSize;
			finalStackPointer = returnPointer + BOND_SLOT_SIZE;
			break;
	}

	uint8_t *framePointer = AlignPointerUp(argTop + function.mArgSize - function.mPackedArgSize, function.mFramePointerAlignment);

	if (function.mUnpackArguments)
	{
		const uint32_t numParams = function.mParamListSignature.mParamCount;
		const ParamSignature *signatures = function.mParamListSignature.mParamSignatures;
		uint8_t *source = argTop;
		for (uint32_t i = 0; i < numParams; ++i)
		{
			const ParamSignature &signature = signatures[i];
			const size_t size = signature.mSize;
			source -= AlignUp(size, size_t(BOND_SLOT_SIZE));

			switch (signature.mType)
			{
				case SIG_CHAR:
					CopyValue<int32_t, int8_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_BOOL:
				case SIG_UCHAR:
					CopyValue<uint32_t, uint8_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_SHORT:
					CopyValue<int32_t, int16_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_USHORT:
					CopyValue<uint32_t, uint16_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				default:
					memmove(framePointer + signature.mFramePointerOffset, source, size);
					break;
			}
		}
	}

	uint8_t *stackPointer = AlignPointerUp(framePointer + function.mLocalSize, BOND_SLOT_SIZE);

	StackFrames::Element stackFrameElement(mStackFrames, *this, function, framePointer, stackPointer, returnPointer);
	ValidateStackPointer(stackPointer);

	if (function.IsNative())
	{
		function.mNativeFunction(stackFrameElement.GetValue());
	}
	else
	{
		ExecuteScriptFunction(stackFrameElement.GetValue());
	}

	return finalStackPointer;
}


void VM::ValidateStackPointer(uint8_t *stackPointer) const
{
	if (stackPointer >= mStack.get() + mStackSize)
	{
		RaiseError("Stack overflow");
	}
}


StackFrame &VM::GetCallerStackFrame()
{
	auto it = mStackFrames.begin();
	auto end = mStackFrames.end();
	if ((it == end) || (++it == end))
	{
		RaiseError("%s", "VM::GetCallerStackFrame(): No caller stack frame exists.");
	}
	return *it;
}


const StackFrame &VM::GetCallerStackFrame() const
{
	auto it = mStackFrames.cbegin();
	auto end = mStackFrames.cend();
	if ((it == end) || (++it == end))
	{
		RaiseError("%s", "VM::GetCallerStackFrame(): No caller stack frame exists.");
	}
	return *it;
}


void VM::DumpCallStack(OutputStream &stream) const
{
	for (const StackFrame &frame: mStackFrames)
	{
		DumpStackFrame(stream, frame);
	}
}


void VM::DumpStackFrame(OutputStream &stream, const StackFrame &frame) const
{
	const Function *function = frame.mFunction;
	if (function != nullptr)
	{
		const SignatureType returnType = static_cast<SignatureType>(function->mReturnSignature.mType);
		const uint32_t returnSize = function->mReturnSignature.mSize;
		stream.Print(GetBondTypeMnemonic(returnType), returnSize);
		stream.Print(" ");

		function->mName.PrintTo(stream);

		stream.Print("(");
		const ParamSignature *paramSignatures = function->mParamListSignature.mParamSignatures;
		const uint32_t numParams = function->mParamListSignature.mParamCount;
		const uint8_t *framePointer = frame.mFramePointer;
		for (uint32_t i = 0; i < numParams; ++i)
		{
			const ParamSignature &signature = paramSignatures[i];
			const uint8_t *argPointer = framePointer + signature.mFramePointerOffset;

			if (i > 0)
			{
				stream.Print(", ");
			}

			switch (signature.mType)
			{
				case SIG_BOOL:
					stream.Print("%s", (*argPointer != 0) ? "true" : "false");
					break;
				case SIG_CHAR:
					stream.Print("%" BOND_PRId32, *reinterpret_cast<const int8_t *>(argPointer));
					break;
				case SIG_UCHAR:
					stream.Print("%" BOND_PRIu32, *reinterpret_cast<const uint8_t *>(argPointer));
					break;
				case SIG_SHORT:
					stream.Print("%" BOND_PRId32, *reinterpret_cast<const int16_t *>(argPointer));
					break;
				case SIG_USHORT:
					stream.Print("%" BOND_PRIu32, *reinterpret_cast<const uint16_t *>(argPointer));
					break;
				case SIG_INT:
					stream.Print("%" BOND_PRId32, *reinterpret_cast<const int32_t *>(argPointer));
					break;
				case SIG_UINT:
					stream.Print("%" BOND_PRIu32, *reinterpret_cast<const uint32_t *>(argPointer));
					break;
				case SIG_LONG:
					stream.Print("%" BOND_PRId64, *reinterpret_cast<const int64_t *>(argPointer));
					break;
				case SIG_ULONG:
					stream.Print("%" BOND_PRIu64, *reinterpret_cast<const uint64_t *>(argPointer));
					break;
				case SIG_FLOAT:
					stream.Print("%f", *reinterpret_cast<const float *>(argPointer));
					break;
				case SIG_DOUBLE:
					stream.Print("%f", *reinterpret_cast<const double *>(argPointer));
					break;
				case SIG_POINTER:
					stream.Print("%p", reinterpret_cast<const void *>(argPointer));
					break;
				case SIG_AGGREGATE:
				case SIG_VOID:
					stream.Print(GetBondTypeMnemonic(SignatureType(signature.mType)), signature.mSize);
					break;
			}
		}
		stream.Print(")\n");
	}
}


void VM::RaiseError(const char *format, ...) const
{
	char buffer[Exception::MESSAGE_BUFFER_LENGTH];
	MemoryOutputStream stream(buffer, Stream::pos_t(Exception::MESSAGE_BUFFER_LENGTH));

	va_list argList;
	va_start(argList, format);
	stream.VPrint(format, argList);
	va_end(argList);

	stream.Print("\n");
	DumpCallStack(stream);

	BOND_FAIL_MESSAGE(buffer);
}

}
