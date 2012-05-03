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


inline void CopyValue32(const void *source, void *dest)
{
	// Assumes that source and dest are 32 bit aligned.
	*reinterpret_cast<bu32_t *>(dest) = *reinterpret_cast<const bu32_t *>(source);
}


inline void CopyValue64(const void *source, void *dest)
{
	// Assumes that source and dest are 32 bit aligned.
	const bu32_t *s = reinterpret_cast<const bu32_t *>(source);
	bu32_t *d = reinterpret_cast<bu32_t *>(dest);
	d[0] = s[0];
	d[1] = s[1];
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
	unsigned char *sp = AddPointerOffset(fp, function->mLocalSize, MIN_STACK_FRAME_ALIGN);
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
			case OPCODE_CONST32:
			{
				const Value16 valueIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue32(value32Table + valueIndex.mUShort, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONST64:
			{
				const Value16 valueIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue64(value64Table + valueIndex.mUShort, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_CONSTI_N2:
			{
				const Value32 value(static_cast<bi32_t>(-2));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_N1:
			{
				const Value32 value(static_cast<bi32_t>(-1));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_0:
			{
				const Value32 value(static_cast<bi32_t>(0));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_1:
			{
				const Value32 value(static_cast<bi32_t>(1));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_2:
			{
				const Value32 value(static_cast<bi32_t>(2));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_3:
			{
				const Value32 value(static_cast<bi32_t>(3));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_4:
			{
				const Value32 value(static_cast<bi32_t>(4));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_5:
			{
				const Value32 value(static_cast<bi32_t>(5));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_6:
			{
				const Value32 value(static_cast<bi32_t>(6));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_7:
			{
				const Value32 value(static_cast<bi32_t>(7));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTI_8:
			{
				const Value32 value(static_cast<bi32_t>(8));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTL_N1:
			{
				const Value64 value(static_cast<bi64_t>(-1));
				CopyValue64(value.mBytes, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_CONSTL_0:
			{
				const Value64 value(static_cast<bi64_t>(0));
				CopyValue64(value.mBytes, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_CONSTL_1:
			{
				const Value64 value(static_cast<bi64_t>(1));
				CopyValue64(value.mBytes, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_CONSTF_N2:
			{
				const Value32 value(static_cast<bf32_t>(-2.0f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_N1:
			{
				const Value32 value(static_cast<bf32_t>(-1.0f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_NH:
			{
				const Value32 value(static_cast<bf32_t>(-0.5f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_0:
			{
				const Value32 value(static_cast<bf32_t>(0.0f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_H:
			{
				const Value32 value(static_cast<bf32_t>(0.5f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_1:
			{
				const Value32 value(static_cast<bf32_t>(1.0f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_CONSTF_2:
			{
				const Value32 value(static_cast<bf32_t>(2.0f));
				CopyValue32(value.mBytes, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32:
			{
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				CopyValue32(fp + offset.mShort, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32W:
			{
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue32(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_P0:
			{
				CopyValue32(fp - sizeof(Value32), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_P1:
			{
				CopyValue32(fp - (2 * sizeof(Value32)), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_P2:
			{
				CopyValue32(fp - (3 * sizeof(Value32)), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_P3:
			{
				CopyValue32(fp - (4 * sizeof(Value32)), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_L0:
			{
				CopyValue32(fp, sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_L1:
			{
				CopyValue32(fp + sizeof(Value32), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_L2:
			{
				CopyValue32(fp + (2 * sizeof(Value32)), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH32_L3:
			{
				CopyValue32(fp + (3 * sizeof(Value32)), sp);
				sp += sizeof(Value32);
			}
			break;

			case OPCODE_PUSH64:
			{
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				CopyValue64(fp + offset.mShort, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64W:
			{
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue64(fp + value32Table[offsetIndex.mUShort].mInt, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_P0:
			{
				CopyValue64(fp - sizeof(Value64), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_P1:
			{
				CopyValue64(fp - (2 * sizeof(Value64)), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_P2:
			{
				CopyValue64(fp - (3 * sizeof(Value64)), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_P3:
			{
				CopyValue64(fp - (4 * sizeof(Value64)), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_L0:
			{
				CopyValue64(fp, sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_L1:
			{
				CopyValue64(fp + sizeof(Value64), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_L2:
			{
				CopyValue64(fp + (2 * sizeof(Value64)), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_PUSH64_L3:
			{
				CopyValue64(fp + (3 * sizeof(Value64)), sp);
				sp += sizeof(Value64);
			}
			break;

			case OPCODE_POP32:
			{
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				CopyValue32(sp - sizeof(Value32), fp + offset.mShort);
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32W:
			{
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue32(sp - sizeof(Value32), fp + value32Table[offsetIndex.mUShort].mInt);
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_P0:
			{
				CopyValue32(sp - sizeof(Value32), fp - sizeof(Value32));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_P1:
			{
				CopyValue32(sp - sizeof(Value32), fp - (2 * sizeof(Value32)));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_P2:
			{
				CopyValue32(sp - sizeof(Value32), fp - (3 * sizeof(Value32)));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_P3:
			{
				CopyValue32(sp - sizeof(Value32), fp - (4 * sizeof(Value32)));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_L0:
			{
				CopyValue32(sp - sizeof(Value32), fp);
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_L1:
			{
				CopyValue32(sp - sizeof(Value32), fp + sizeof(Value32));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_L2:
			{
				CopyValue32(sp - sizeof(Value32), fp + (2 * sizeof(Value32)));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP32_L3:
			{
				CopyValue32(sp - sizeof(Value32), fp + (3 * sizeof(Value32)));
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_POP64:
			{
				const Value16 offset(code + pc);
				pc += sizeof(Value16);
				CopyValue64(sp - sizeof(Value64), fp + offset.mShort);
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64W:
			{
				const Value16 offsetIndex(code + pc);
				pc += sizeof(Value16);
				CopyValue64(sp - sizeof(Value64), fp + value32Table[offsetIndex.mUShort].mInt);
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_P0:
			{
				CopyValue64(sp - sizeof(Value64), fp - sizeof(Value64));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_P1:
			{
				CopyValue64(sp - sizeof(Value64), fp - (2 * sizeof(Value64)));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_P2:
			{
				CopyValue64(sp - sizeof(Value64), fp - (3 * sizeof(Value64)));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_P3:
			{
				CopyValue64(sp - sizeof(Value64), fp - (4 * sizeof(Value64)));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_L0:
			{
				CopyValue64(sp - sizeof(Value64), fp);
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_L1:
			{
				CopyValue64(sp - sizeof(Value64), fp + sizeof(Value64));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_L2:
			{
				CopyValue64(sp - sizeof(Value64), fp + (2 * sizeof(Value64)));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_POP64_L3:
			{
				CopyValue64(sp - sizeof(Value64), fp + (3 * sizeof(Value64)));
				sp -= sizeof(Value64);
			}
			break;

			case OPCODE_ADDI:
			{
				bi32_t *ptr = reinterpret_cast<bi32_t *>(sp);
				*(ptr -2) = *(ptr - 2) + *(ptr - 1);
				sp -= sizeof(Value32);
			}
			break;

			case OPCODE_RETURN: return;

			case OPCODE_RETURN32:
			{
				CopyValue32(sp - sizeof(Value32), frame.mReturnPointer);
			}
			return;

			case OPCODE_RETURN64:
			{
				CopyValue64(sp - sizeof(Value64), frame.mReturnPointer);
			}
			return;

			default:
			break;
		}
	}

	/*
	const ParamListSignature &paramListSignature = frame.mFunction->mParamListSignature;
	const bu32_t numParams = paramListSignature.mParamCount;
	bu32_t total = 0;

	for (bu32_t i = 0; i < numParams; ++i)
	{
		const ParamSignature &param = paramListSignature.mParamSignatures[i];
		const unsigned char *ptr = frame.mFramePointer + param.mFramePointerOffset;
		switch (param.mType)
		{
			case SIG_INT:
			{
				bu32_t val = *reinterpret_cast<const int *>(ptr);
				printf("%u: %d\n", i, val);
				total += val;
			}
			break;
			case SIG_FLOAT:
			{
				printf("%u: %d\n", i, *reinterpret_cast<const int *>(ptr));
			}
			break;
		}
	}

	frame.SetReturnValue(total);
	*/
}

}
