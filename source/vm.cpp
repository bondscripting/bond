#include "bond/io/bufferedtextwriter.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/allocator.h"
#include "bond/systems/math.h"
#include "bond/types/opcodes.h"
#include "bond/vm/vm.h"
#include <string.h>

namespace Bond
{

struct MatchOffsetPair
{
	MatchOffsetPair(): match(0), offset(0) {}
	MatchOffsetPair(bi32_t m): match(m), offset(0) {}
	bi32_t match;
	bi32_t offset;

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
	*reinterpret_cast<bu32_t *>(dest) = *reinterpret_cast<const bu32_t *>(source);
}


inline void CopyValue64(const void *source, void *dest)
{
	// Expects the source and dest pointers to be 64 bit aligned.
	*reinterpret_cast<bu64_t *>(dest) = *reinterpret_cast<const bu64_t *>(source);
}


inline void SwapValue64(void *a, void *b)
{
	// Expects the pointers to be 64 bit aligned.
	const bu64_t temp = *reinterpret_cast<bu64_t *>(a);
	*reinterpret_cast<bu64_t *>(a) = *reinterpret_cast<const bu64_t *>(b);
	*reinterpret_cast<bu64_t *>(b) = temp;
}


void CallerStackFrame::Initialize(VM &vm, const HashedString &functionName, void *returnPointer)
{
	bu8_t *prevStackPointer = GetNext()->GetValue().mStackPointer;
	const CodeSegment &codeSegment = vm.GetCodeSegment();
	const Function *function = codeSegment.GetFunction(functionName);
	BOND_ASSERT_FORMAT(function != NULL, ("Failed to look up function '%s'.", functionName.GetString()));
	bu8_t *framePointer = static_cast<bu8_t *>(AlignPointerUp(prevStackPointer + function->mArgSize, function->mFramePointerAlignment));
	bu8_t *stackPointer = static_cast<bu8_t *>(AlignPointerUp(framePointer + function->mLocalSize, BOND_SLOT_SIZE));
	mValue.mFunction = function;
	mValue.mFramePointer = framePointer;
	mValue.mStackPointer = stackPointer;
	mValue.mReturnPointer = reinterpret_cast<bu8_t *>(returnPointer);
	vm.ValidateStackPointer(stackPointer);
}


void CallerStackFrame::Call()
{
	CalleeStackFrame &frame = GetValue();
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
		frame.mVm.ExecuteScriptFunction();
	}
}


VM::VM(Allocator &allocator, const CodeSegment &codeSegment, size_t stackSize):
	mStackFrames(),
	mDummyFrame(mStackFrames, CalleeStackFrame(*this)),
	mAllocator(allocator),
	mCodeSegment(codeSegment),
	mStack(NULL),
	mStackSize(stackSize)
{
	mStack = mAllocator.Alloc<bu8_t>(stackSize);
	CalleeStackFrame &top = mDummyFrame.GetValue();
	top.mFunction = NULL;
	top.mStackPointer = mStack;
	top.mFramePointer = mStack;
	top.mReturnPointer = NULL;
}


VM::~VM()
{
	mAllocator.Free(mStack);
	mStack = NULL;
}


void VM::ExecuteScriptFunction()
{
	CalleeStackFrame &frame = GetTopStackFrame();
	const Value32 *value32Table = frame.mFunction->mConstantTable->mValue32Table;
	const Value64 *value64Table = frame.mFunction->mConstantTable->mValue64Table;
	const Function *functionTable = mCodeSegment.GetFunctionAtIndex(0);
	const bu8_t *code = frame.mFunction->mCode;
	bu8_t *const fp = frame.mFramePointer;
	bu8_t *sp = frame.mStackPointer;
	size_t pc = 0;

	for (;;)
	{
		const OpCode opCode = static_cast<OpCode>(code[pc++]);
		switch (opCode)
		{
			case OPCODE_CONSTC:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(static_cast<char>(code[pc++]));
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTUC:
			{
				*reinterpret_cast<bu32_t *>(sp) = static_cast<bu32_t>(code[pc++]);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTS:
			{
				*reinterpret_cast<bi32_t *>(sp) = Value16(code + pc).mShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTUS:
			{
				*reinterpret_cast<bu32_t *>(sp) = Value16(code + pc).mUShort;
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
				CopyValue64(value64Table + valueIndex.mUShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_N2:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(-2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_N1:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(-1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_0:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_1:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_2:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_3:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(3);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_4:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(4);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_8:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(8);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_N2:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(-2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_N1:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(-1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_0:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_1:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(1);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_2:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(2);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_3:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(3);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_4:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(4);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTL_8:
			{
				*reinterpret_cast<bi64_t *>(sp) = static_cast<bi64_t>(8);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_N2:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(-2.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_N1:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(-1.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_NH:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(-0.5f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_0:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(0.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_H:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(0.5f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_1:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(1.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_2:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(2.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_N2:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(-2.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_N1:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(-1.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_NH:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(-0.5);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_0:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(0.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_H:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(0.5);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_1:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(1.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTD_2:
			{
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(2.0);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADFP:
			{
				const Value16 offset(code + pc);
				*reinterpret_cast<bu8_t **>(sp) = fp + offset.mShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADFPW:
			{
				const Value16 offsetIndex(code + pc);
				*reinterpret_cast<bu8_t **>(sp) = fp + value32Table[offsetIndex.mUShort].mInt;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADEA:
				// TODO
				break;

			case OPCODE_LOADC:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<char, bi32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADUC:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<bu8_t, bu32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADS:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<bi16_t, bi32_t>(address, sp - BOND_SLOT_SIZE);
			}
			break;

			case OPCODE_LOADUS:
			{
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				CopyValue<bu16_t, bu32_t>(address, sp - BOND_SLOT_SIZE);
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
				sp = static_cast<bu8_t *>(AlignPointerUp(sp - BOND_SLOT_SIZE + memSize, BOND_SLOT_SIZE));
			}
			break;

			case OPCODE_STOREC:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue<bi32_t, char>(sp - BOND_SLOT_SIZE, address);
				sp -= 2 * BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_STORES:
			{
				void *address = *reinterpret_cast<void **>(sp - (2 * BOND_SLOT_SIZE));
				CopyValue<bi32_t, bi16_t>(sp - BOND_SLOT_SIZE, address);
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
				CopyValue<char, bi32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<char, bi32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUC:
			{
				const Value16 offset(code + pc);
				CopyValue<bu8_t, bu32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<bu8_t, bu32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHS:
			{
				const Value16 offset(code + pc);
				CopyValue<bi16_t, bi32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<bi16_t, bi32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUS:
			{
				const Value16 offset(code + pc);
				CopyValue<bu16_t, bu32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<bu16_t, bu32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
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
				CopyValue<bi32_t, char>(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<bi32_t, char>(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPS:
			{
				const Value16 offset(code + pc);
				CopyValue<bi32_t, bi16_t>(sp - BOND_SLOT_SIZE, fp + offset.mShort);
				pc += sizeof(Value16);
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_POPSW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<bi32_t, bi16_t>(sp - BOND_SLOT_SIZE, fp + value32Table[offsetIndex.mUShort].mInt);
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
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<char>(*a);
			}
			break;

			case OPCODE_UITOUC:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu8_t>(*a);
			}
			break;

			case OPCODE_ITOS:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi16_t>(*a);
			}
			break;

			case OPCODE_UITOUS:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu16_t>(*a);
			}
			break;

			case OPCODE_ITOL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = *b;
			}
			break;

			case OPCODE_UITOUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - BOND_SLOT_SIZE);
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = *b;
			}
			break;

			case OPCODE_ITOF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf32_t>(*b);
			}
			break;

			case OPCODE_UITOF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf32_t>(*b);
			}
			break;

			case OPCODE_ITOD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf64_t>(*b);
			}
			break;

			case OPCODE_UITOD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf64_t>(*b);
			}
			break;

			case OPCODE_LTOI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi32_t>(*b);
			}
			break;

			case OPCODE_LTOF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf32_t>(*b);
			}
			break;

			case OPCODE_ULTOF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf32_t>(*b);
			}
			break;

			case OPCODE_LTOD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf64_t>(*b);
			}
			break;

			case OPCODE_ULTOD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf64_t>(*b);
			}
			break;

			case OPCODE_FTOI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi32_t>(*b);
			}
			break;

			case OPCODE_FTOUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu32_t>(*b);
			}
			break;

			case OPCODE_FTOL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi64_t>(*b);
			}
			break;

			case OPCODE_FTOUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - BOND_SLOT_SIZE);
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu64_t>(*b);
			}
			break;

			case OPCODE_FTOD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf64_t>(*b);
			}
			break;

			case OPCODE_DTOI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi32_t>(*b);
			}
			break;

			case OPCODE_DTOUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu32_t>(*b);
			}
			break;

			case OPCODE_DTOL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - BOND_SLOT_SIZE);
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bi64_t>(*b);
			}
			break;

			case OPCODE_DTOUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - BOND_SLOT_SIZE);
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bu64_t>(*b);
			}
			break;

			case OPCODE_DTOF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - BOND_SLOT_SIZE);
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<bf32_t>(*b);
			}
			break;

			case OPCODE_PTROFF:
			{
				intptr_t *ptr = reinterpret_cast<intptr_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *offset = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
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
				const bu32_t slotIndex = static_cast<bu32_t>(code[pc]);
				const bi32_t value = static_cast<bi32_t>(static_cast<char>(code[pc + 1]));
				pc += 2;
				*reinterpret_cast<bi32_t *>(fp + (slotIndex * BOND_SLOT_SIZE)) += value;
			}
			break;

			case OPCODE_INCL:
			{
				const bu32_t slotIndex = static_cast<bu32_t>(code[pc]);
				const bi64_t value = static_cast<bi64_t>(static_cast<char>(code[pc + 1]));
				pc += 2;
				*reinterpret_cast<bi64_t *>(fp + (slotIndex * BOND_SLOT_SIZE)) += value;
			}
			break;

			case OPCODE_ADDI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ADDD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a += *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_SUBD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a -= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_MULD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a *= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_DIVD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a /= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REML:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_REMUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a %= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LSHI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a <<= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LSHL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a <<= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_RSHUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a >>= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ANDI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a &= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ANDL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a &= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ORI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a |= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_ORL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a |= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_XORI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a ^= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_XORL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a ^= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_NEGI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NEGD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = -(*a);
			}
			break;

			case OPCODE_NOT:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = !(*a);
			}
			break;

			case OPCODE_CMPEQI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQD:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a == *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQD:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a != *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUI:
			{
				const bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUL:
			{
				const bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTD:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a < *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUI:
			{
				const bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUL:
			{
				const bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLED:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a <= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUI:
			{
				const bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUL:
			{
				const bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTD:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a > *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEI:
			{
				const bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUI:
			{
				const bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEL:
			{
				const bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUL:
			{
				const bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEF:
			{
				const bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGED:
			{
				const bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				const bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				bi32_t *result = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				*result = (*a >= *b) ? 1 : 0;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_NBRZ:
			{
				bi32_t *condition = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*condition = bi32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRZ:
			{
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				bi32_t *condition = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*condition = bi32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRZW:
			{
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				bi32_t *condition = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*condition = bi32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRNZ:
			{
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				bi32_t *condition = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*condition = bi32_t(1) - *condition;
			}
			// Fall through.

			case OPCODE_BRNZW:
			{
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				const bi32_t condition = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
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
				const bi32_t defaultOffset = *reinterpret_cast<const bi32_t *>(code + pc);
				const bu32_t numMatches = *reinterpret_cast<const bu32_t *>(code + pc + sizeof(Value32));
				const MatchOffsetPair *jumpTable =
					reinterpret_cast<const MatchOffsetPair *>(code + pc + (2 * sizeof(Value32)));
				const MatchOffsetPair *jumpTableEnd = jumpTable + numMatches;
				const MatchOffsetPair condition(*reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE));
				const MatchOffsetPair *pair = lower_bound(jumpTable, jumpTableEnd, condition);
				const bi32_t offset =
					((pair < jumpTableEnd) && (pair->match == condition.match)) ? pair->offset : defaultOffset;
				pc += ((2 + (numMatches * 2)) * sizeof(Value32)) + offset;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_TABLESWITCH:
			{
				pc = AlignUp(pc, sizeof(Value32));
				const bi32_t defaultOffset = *reinterpret_cast<const bi32_t *>(code + pc);
				const bi32_t minMatch = *reinterpret_cast<const bu32_t *>(code + pc + sizeof(Value32));
				const bi32_t maxMatch = *reinterpret_cast<const bu32_t *>(code + pc + (2 * sizeof(Value32)));
				const bi32_t *jumpTable = reinterpret_cast<const bi32_t *>(code + pc + (3 * sizeof(Value32)));
				const bi32_t index = *reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				const bi32_t offset =
					((index >= minMatch) && (index <= maxMatch)) ? jumpTable[index - minMatch] : defaultOffset;
				pc += ((4 + maxMatch - minMatch) * sizeof(Value32)) + offset;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_INVOKE:
			{
				const Value16 functionIndex(code + pc);
				pc += sizeof(Value16);
				sp = InvokeFunction(functionTable + functionIndex.mUShort, sp);
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
				const bi32_t memSize = value32Table[memSizeIndex.mUShort].mInt;
				const void *address = *reinterpret_cast<void **>(sp - BOND_SLOT_SIZE);
				memcpy(frame.mReturnPointer, address, memSize);
				//pc += sizeof(Value16);
				//sp = static_cast<bu8_t *>(AlignPointerUp(sp - BOND_SLOT_SIZE + memSize, BOND_SLOT_SIZE));
			}
			return;

			case OPCODE_NOP: break;
			case OPCODE_MAX: break;
		}
	}
}


bu8_t *VM::InvokeFunction(const Function *function, bu8_t *stackTop)
{
	// Functions return values on top of the stack with the exception of functions that return
	// structs. Those functions expect an additional argument on the operand stack to indicate
	// where the value should be returned. So for functions that return a struct, we have to
	// pay particular attention to locating the top of the arguments, the return address and
	// where we leave the stack pointer when the function returns, since the return value is
	// not left on top of the stack.
	bu8_t *argTop;
	bu8_t *returnPointer;
	bu8_t *finalStackPointer;
	switch (function->mReturnSignature.mType)
	{
		case SIG_STRUCT:
			argTop = stackTop - BOND_SLOT_SIZE;
			returnPointer = *reinterpret_cast<bu8_t **>(argTop);
			finalStackPointer = argTop - function->mPackedArgSize;
			break;
		case SIG_VOID:
			argTop = stackTop;
			returnPointer = NULL;
			finalStackPointer = argTop - function->mPackedArgSize;
			break;
		default:
			argTop = stackTop;
			returnPointer = argTop - function->mPackedArgSize;
			finalStackPointer = returnPointer + BOND_SLOT_SIZE;
			break;
	}

	bu8_t *framePointer = static_cast<bu8_t *>(AlignPointerUp(argTop + function->mArgSize - function->mPackedArgSize, function->mFramePointerAlignment));

	if (function->mUnpackArguments)
	{
		const bu32_t numParams = function->mParamListSignature.mParamCount;
		const ParamSignature *signatures = function->mParamListSignature.mParamSignatures;
		bu8_t *source = argTop;
		for (bu32_t i = 0; i < numParams; ++i)
		{
			const ParamSignature &signature = signatures[i];
			const size_t size = signature.mSize;
			source -= AlignUp(size, size_t(BOND_SLOT_SIZE));

			switch (signature.mType)
			{
				case SIG_CHAR:
					CopyValue<bi32_t, bi8_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_BOOL:
				case SIG_UCHAR:
					CopyValue<bu32_t, bu8_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_SHORT:
					CopyValue<bi32_t, bi16_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				case SIG_USHORT:
					CopyValue<bu32_t, bu16_t>(source, framePointer + signature.mFramePointerOffset);
					break;
				default:
					memmove(framePointer + signature.mFramePointerOffset, source, size);
					break;
			}
		}
	}

	bu8_t *stackPointer = static_cast<bu8_t *>(AlignPointerUp(framePointer + function->mLocalSize, BOND_SLOT_SIZE));

	StackFrames::Element stackFrameElement(mStackFrames, CalleeStackFrame(*this, function, framePointer, stackPointer, returnPointer));
	ValidateStackPointer(stackPointer);

	if (function->IsNative())
	{
		function->mNativeFunction(stackFrameElement.GetValue());
	}
	else
	{
		ExecuteScriptFunction();
	}

	return finalStackPointer;
}


void VM::ValidateStackPointer(bu8_t *stackPointer) const
{
	if (stackPointer >= mStack + mStackSize)
	{
		RaiseError("Stack overflow");
	}
}


void VM::DumpCallStack(TextWriter &writer) const
{
	StackFrames::ConstIterator it = mStackFrames.Begin();
	while (it != mStackFrames.End())
	{
		DumpStackFrame(writer, *it);
		++it;
	}
}


void VM::DumpStackFrame(TextWriter &writer, const CalleeStackFrame &frame) const
{
	const Function *function = frame.mFunction;
	if (function != NULL)
	{
		const SignatureType returnType = static_cast<SignatureType>(function->mReturnSignature.mType);
		const bu32_t returnSize = function->mReturnSignature.mSize;
		writer.Write(GetBondTypeMnemonic(returnType), returnSize);
		writer.Write(" ");

		const char *const *elements = function->mName;
		bool isFirstElement = true;
		while (*elements != NULL)
		{
			if (!isFirstElement)
			{
				writer.Write("::");
			}
			writer.Write(*elements++);
			isFirstElement = false;
		}

		writer.Write("(");
		const ParamSignature *paramSignatures = function->mParamListSignature.mParamSignatures;
		const bu32_t numParams = function->mParamListSignature.mParamCount;
		const bu8_t *framePointer = frame.mFramePointer;
		for (bu32_t i = 0; i < numParams; ++i)
		{
			const ParamSignature &signature = paramSignatures[i];
			const bu8_t *argPointer = framePointer + signature.mFramePointerOffset;

			if (i > 0)
			{
				writer.Write(", ");
			}

			switch (signature.mType)
			{
				case SIG_BOOL:
					writer.Write("%s", (*argPointer != 0) ? "true" : "false");
					break;
				case SIG_CHAR:
					writer.Write("%" BOND_PRId32, *reinterpret_cast<const bi8_t *>(argPointer));
					break;
				case SIG_UCHAR:
					writer.Write("%" BOND_PRIu32, *reinterpret_cast<const bu8_t *>(argPointer));
					break;
				case SIG_SHORT:
					writer.Write("%" BOND_PRId32, *reinterpret_cast<const bi16_t *>(argPointer));
					break;
				case SIG_USHORT:
					writer.Write("%" BOND_PRIu32, *reinterpret_cast<const bu16_t *>(argPointer));
					break;
				case SIG_INT:
					writer.Write("%" BOND_PRId32, *reinterpret_cast<const bi32_t *>(argPointer));
					break;
				case SIG_UINT:
					writer.Write("%" BOND_PRIu32, *reinterpret_cast<const bu32_t *>(argPointer));
					break;
				case SIG_LONG:
					writer.Write("%" BOND_PRId64, *reinterpret_cast<const bi64_t *>(argPointer));
					break;
				case SIG_ULONG:
					writer.Write("%" BOND_PRIu64, *reinterpret_cast<const bu64_t *>(argPointer));
					break;
				case SIG_FLOAT:
					writer.Write("%" BOND_PRIf32, *reinterpret_cast<const bf32_t *>(argPointer));
					break;
				case SIG_DOUBLE:
					writer.Write("%" BOND_PRIf64, *reinterpret_cast<const bf64_t *>(argPointer));
					break;
				case SIG_POINTER:
					writer.Write("%p", reinterpret_cast<const void *>(argPointer));
					break;
				case SIG_STRUCT:
				case SIG_VOID:
					writer.Write(GetBondTypeMnemonic(SignatureType(signature.mType)), signature.mSize);
					break;
			}
		}
		writer.Write(")\n");
	}
}


void VM::RaiseError(const char *format, ...) const
{
	char buffer[Exception::MESSAGE_BUFFER_LENGTH];
	BufferedTextWriter writer(buffer, Exception::MESSAGE_BUFFER_LENGTH);

	va_list argList;
	va_start(argList, format);
	writer.VWrite(format, argList);
	va_end(argList);

	writer.Write("\n");
	DumpCallStack(writer);

	BOND_FAIL_MESSAGE(buffer);
}

}
