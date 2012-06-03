#include "bond/allocator.h"
#include "bond/codesegment.h"
#include "bond/math.h"
#include "bond/opcodes.h"
#include "bond/vm.h"

#include <stdio.h>

namespace Bond
{

inline unsigned char *AddPointerOffset(unsigned char *ptr, bu32_t offset, bu32_t alignment)
{
	const intptr_t p = reinterpret_cast<intptr_t>(ptr);
	const intptr_t o = static_cast<intptr_t>(offset);
	const intptr_t a = static_cast<intptr_t>(alignment);
	const intptr_t result = AlignUp(p + o, a);
	return reinterpret_cast<unsigned char *>(result);
}


template <typename SourceType, typename DestType>
inline void CopyValue(const void *source, void *dest)
{
	// Assumes that source and dest are appropriately aligned.
	*reinterpret_cast<DestType *>(dest) = static_cast<DestType>(*reinterpret_cast<const SourceType *>(source));
}


inline void CopyValue32(const void *source, void *dest)
{
	// Assumes that source and dest are 32 bit aligned.
	*reinterpret_cast<bu32_t *>(dest) = *reinterpret_cast<const bu32_t *>(source);
}


inline void CopyValue64(const void *source, void *dest)
{
	// Assumes that source and dest are 64 bit aligned.
	*reinterpret_cast<bu64_t *>(dest) = *reinterpret_cast<const bu64_t *>(source);
}


VM::CallerStackFrame::CallerStackFrame(VM &vm, const HashedString &functionName, void *returnPointer):
	StackFrames::Element(vm.mStackFrames),
	mVm(vm),
	mNextArg(0)
{
	unsigned char *prevSp = GetNext()->GetValue().mStackPointer;
	const CodeSegment &codeSegment = vm.GetCodeSegment();
	const Function *function = codeSegment.GetFunction(functionName);
	// TODO: report error if function lookup fails.
	unsigned char *fp = AddPointerOffset(prevSp, function->mFrameSize, function->mFramePointerAlignment);
	unsigned char *sp = AddPointerOffset(fp, function->mLocalSize, BOND_SLOT_SIZE);
	// TODO: Report error if sp >= mStack + mStackSize.
	mValue.mFunction = function;
	mValue.mFramePointer = fp;
	mValue.mStackPointer = sp;
	mValue.mReturnPointer = reinterpret_cast<unsigned char *>(returnPointer);
}


VM::VM(Allocator &allocator, const CodeSegment &codeSegment, size_t stackSize):
	mStackFrames(),
	mDummyFrame(mStackFrames),
	mAllocator(allocator),
	mCodeSegment(codeSegment),
	mStack(NULL),
	mStackSize(stackSize)
{
	mStack = mAllocator.Alloc<unsigned char>(stackSize);
	CalleeStackFrame &top = mDummyFrame.GetValue();
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
	const unsigned char *code = frame.mFunction->mCode;
	unsigned char *const fp = frame.mFramePointer;
	unsigned char *sp = frame.mStackPointer;
	bu32_t pc = 0;

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
				pc += sizeof(Value16);
				CopyValue32(value32Table + valueIndex.mUShort, sp);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONST64:
			{
				const Value16 valueIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue64(value64Table + valueIndex.mUShort, sp);
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

			case OPCODE_CONSTI_5:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(5);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_6:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(6);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_7:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(7);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTI_8:
			{
				*reinterpret_cast<bi32_t *>(sp) = static_cast<bi32_t>(8);
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

			case OPCODE_CONSTF_N2:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(-2.0f);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CONSTF_N1:
			{
				*reinterpret_cast<bf32_t *>(sp) = static_cast<bf32_t>(-2.0f);
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
				*reinterpret_cast<bf64_t *>(sp) = static_cast<bf64_t>(-2.0);
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
				*reinterpret_cast<unsigned char **>(sp) = fp + offset.mShort;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_LOADFPW:
			{
				const Value16 offsetIndex(code + pc);
				*reinterpret_cast<unsigned char **>(sp) = fp + value32Table[offsetIndex.mUShort].mInt;
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
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
				CopyValue<unsigned char, bu32_t>(address, sp - BOND_SLOT_SIZE);
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
				CopyValue<unsigned char, bu32_t>(fp + offset.mShort, sp);
				pc += sizeof(Value16);
				sp += BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_PUSHUCW:
			{
				const Value16 offsetIndex(code + pc);
				CopyValue<unsigned char, bu32_t>(fp + value32Table[offsetIndex.mUShort].mInt, sp);
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

			case OPCODE_ITOC:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<char>(*a);
			}
			break;

			case OPCODE_UITOUC:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - BOND_SLOT_SIZE);
				*a = static_cast<unsigned char>(*a);
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

			case OPCODE_INCI:
			{
				const bu32_t slotIndex = static_cast<bu32_t>(code[pc]);
				const bi32_t value = static_cast<bi32_t>(static_cast<char>(code[pc + 1]));
				pc += 2;
				*reinterpret_cast<bi32_t *>(fp + (slotIndex * BOND_SLOT_SIZE)) += value;
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
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a == *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a == *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a == *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPEQD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a == *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a != *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a != *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a != *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPNEQD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a != *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLTD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a < *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLEF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPLED:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a <= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGTD:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a > *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEI:
			{
				bi32_t *a = reinterpret_cast<bi32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi32_t *b = reinterpret_cast<bi32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUI:
			{
				bu32_t *a = reinterpret_cast<bu32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu32_t *b = reinterpret_cast<bu32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEL:
			{
				bi64_t *a = reinterpret_cast<bi64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bi64_t *b = reinterpret_cast<bi64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEUL:
			{
				bu64_t *a = reinterpret_cast<bu64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bu64_t *b = reinterpret_cast<bu64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGEF:
			{
				bf32_t *a = reinterpret_cast<bf32_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf32_t *b = reinterpret_cast<bf32_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

			case OPCODE_CMPGED:
			{
				bf64_t *a = reinterpret_cast<bf64_t *>(sp - (2 * BOND_SLOT_SIZE));
				bf64_t *b = reinterpret_cast<bf64_t *>(sp - (1 * BOND_SLOT_SIZE));
				*a = *a >= *b;
				sp -= BOND_SLOT_SIZE;
			}
			break;

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

			case OPCODE_RETURN: return;

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

			case OPCODE_NOP: break;
			case OPCODE_MAX: break;
			default: break;
		}
	}
}

}
