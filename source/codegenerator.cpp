#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/parsenodetraverser.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/io/binarywriter.h"
#include "bond/stl/algorithm.h"
#include "bond/stl/autostack.h"
#include "bond/stl/list.h"
#include "bond/stl/map.h"
#include "bond/stl/vector.h"
#include "bond/systems/endian.h"
#include "bond/systems/math.h"
#include "bond/tools/cboutil.h"
#include "bond/types/opcodes.h"
#include "bond/version.h"

namespace Bond
{

class OpCodeSet
{
public:
	OpCodeSet(OpCode opCodeI, OpCode opCodeL):
		mOpCodeInt(opCodeI),
		mOpCodeUInt(opCodeI),
		mOpCodeLong(opCodeL),
		mOpCodeULong(opCodeL),
		mOpCodeFloat(OPCODE_NOP),
		mOpCodeDouble(OPCODE_NOP)
	{}

	OpCodeSet(OpCode opCodeI, OpCode opCodeL, OpCode opCodeF, OpCode opCodeD):
		mOpCodeInt(opCodeI),
		mOpCodeUInt(opCodeI),
		mOpCodeLong(opCodeL),
		mOpCodeULong(opCodeL),
		mOpCodeFloat(opCodeF),
		mOpCodeDouble(opCodeD)
	{}

	OpCodeSet(OpCode opCodeI, OpCode opCodeUI, OpCode opCodeL, OpCode opCodeUL, OpCode opCodeF, OpCode opCodeD):
		mOpCodeInt(opCodeI),
		mOpCodeUInt(opCodeUI),
		mOpCodeLong(opCodeL),
		mOpCodeULong(opCodeUL),
		mOpCodeFloat(opCodeF),
		mOpCodeDouble(opCodeD)
	{}

	OpCode GetPointerOpCode(PointerSize size) const
	{
		if (size == POINTER_64BIT)
		{
			return mOpCodeULong;
		}
		return mOpCodeUInt;
	}

	OpCode GetOpCode(const TypeDescriptor &typeDescriptor) const
	{
		return GetOpCode(typeDescriptor.GetPrimitiveType());
	}

	OpCode GetOpCode(Token::TokenType type) const
	{
		switch (type)
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
			case Token::KEY_SHORT:
			case Token::KEY_INT:
				return mOpCodeInt;
			case Token::KEY_UCHAR:
			case Token::KEY_USHORT:
			case Token::KEY_UINT:
				return mOpCodeUInt;
			case Token::KEY_LONG:
				return mOpCodeLong;
			case Token::KEY_ULONG:
				return mOpCodeULong;
			case Token::KEY_FLOAT:
				return mOpCodeFloat;
			case Token::KEY_DOUBLE:
				return mOpCodeDouble;
			default:
				return OPCODE_NOP;
		}
	}

private:
	OpCode mOpCodeInt;
	OpCode mOpCodeUInt;
	OpCode mOpCodeLong;
	OpCode mOpCodeULong;
	OpCode mOpCodeFloat;
	OpCode mOpCodeDouble;
};


static const OpCodeSet CONST1_OPCODES(OPCODE_CONSTI_1, OPCODE_CONSTL_1, OPCODE_CONSTF_1, OPCODE_CONSTD_1);
static const OpCodeSet CONSTN1_OPCODES(OPCODE_CONSTI_N1, OPCODE_CONSTL_N1, OPCODE_CONSTF_N1, OPCODE_CONSTD_N1);
static const OpCodeSet INC_OPCODES(OPCODE_INCI, OPCODE_INCL);
static const OpCodeSet LOAD_OPCODES(OPCODE_LOAD32, OPCODE_LOAD64);
static const OpCodeSet STORE_OPCODES(OPCODE_STORE32, OPCODE_STORE64);
static const OpCodeSet ADD_OPCODES(OPCODE_ADDI, OPCODE_ADDL, OPCODE_ADDF, OPCODE_ADDD);
static const OpCodeSet SUB_OPCODES(OPCODE_SUBI, OPCODE_SUBL, OPCODE_SUBF, OPCODE_SUBD);
static const OpCodeSet MUL_OPCODES(OPCODE_MULI, OPCODE_MULUI, OPCODE_MULL, OPCODE_MULUL, OPCODE_MULF, OPCODE_MULD);
static const OpCodeSet DIV_OPCODES(OPCODE_DIVI, OPCODE_DIVUI, OPCODE_DIVL, OPCODE_DIVUL, OPCODE_DIVF, OPCODE_DIVD);
static const OpCodeSet REM_OPCODES(OPCODE_REMI, OPCODE_REMUI, OPCODE_REML, OPCODE_REMUL, OPCODE_NOP, OPCODE_NOP);
static const OpCodeSet LSH_OPCODES(OPCODE_LSHI, OPCODE_LSHL);
static const OpCodeSet RSH_OPCODES(OPCODE_RSHI, OPCODE_RSHUI, OPCODE_RSHL, OPCODE_RSHUL, OPCODE_NOP, OPCODE_NOP);
static const OpCodeSet AND_OPCODES(OPCODE_ANDI, OPCODE_ANDL);
static const OpCodeSet OR_OPCODES(OPCODE_ORI, OPCODE_ORL);
static const OpCodeSet XOR_OPCODES(OPCODE_XORI, OPCODE_XORL);
static const OpCodeSet NEG_OPCODES(OPCODE_NEGI, OPCODE_NEGL, OPCODE_NEGF, OPCODE_NEGD);
static const OpCodeSet CMPEQ_OPCODES(OPCODE_CMPEQI, OPCODE_CMPEQL, OPCODE_CMPEQF, OPCODE_CMPEQD);
static const OpCodeSet CMPNEQ_OPCODES(OPCODE_CMPNEQI, OPCODE_CMPNEQL, OPCODE_CMPNEQF, OPCODE_CMPNEQD);
static const OpCodeSet CMPLT_OPCODES(OPCODE_CMPLTI, OPCODE_CMPLTUI, OPCODE_CMPLTL, OPCODE_CMPLTUL, OPCODE_CMPLTF, OPCODE_CMPLTD);
static const OpCodeSet CMPLE_OPCODES(OPCODE_CMPLEI, OPCODE_CMPLEUI, OPCODE_CMPLEL, OPCODE_CMPLEUL, OPCODE_CMPLEF, OPCODE_CMPLED);
static const OpCodeSet CMPGT_OPCODES(OPCODE_CMPGTI, OPCODE_CMPGTUI, OPCODE_CMPGTL, OPCODE_CMPGTUL, OPCODE_CMPGTF, OPCODE_CMPGTD);
static const OpCodeSet CMPGE_OPCODES(OPCODE_CMPGEI, OPCODE_CMPGEUI, OPCODE_CMPGEL, OPCODE_CMPGEUL, OPCODE_CMPGEF, OPCODE_CMPGED);
static const OpCodeSet RETURN_OPCODES(OPCODE_RETURN32, OPCODE_RETURN64, OPCODE_RETURN32, OPCODE_RETURN64);


class GeneratorCore: private ParseNodeTraverser
{
public:
	GeneratorCore(
			Allocator &allocator,
			BinaryWriter &writer,
			CompilerErrorBuffer &errorBuffer,
			const TranslationUnit *translationUnitList,
			PointerSize pointerSize):
		mStringIndexMap(StringIndexMap::Compare(), StringIndexMap::Allocator(&allocator)),
		mStringList(StringList::Allocator(&allocator)),
		mValue32IndexMap(Value32IndexMap::Compare(), Value32IndexMap::Allocator(&allocator)),
		mValue32List(Value32List::Allocator(&allocator)),
		mValue64IndexMap(Value64IndexMap::Compare(), Value64IndexMap::Allocator(&allocator)),
		mValue64List(Value64List::Allocator(&allocator)),
		mFunctionList(FunctionList::Allocator(&allocator)),
		mNativeMemberList(NamedInitializerList::Allocator(&allocator)),
		mAllocator(allocator),
		mWriter(writer),
		mErrorBuffer(errorBuffer),
		mTranslationUnitList(translationUnitList),
		mPointerSize(pointerSize)
	{}

	void Generate();

private:
	enum Fixedness
	{
		PREFIX,
		POSTFIX
	};

	struct Result
	{
		enum Context
		{
			CONTEXT_NONE,
			CONTEXT_FP_INDIRECT,
			CONTEXT_FP_OFFSET,
			CONTEXT_STACK_VALUE,
			CONTEXT_ADDRESS_INDIRECT,
			CONTEXT_CONSTANT_VALUE,
			CONTEXT_MEMBER_FUNCTION,
			CONTEXT_NATIVE_MEMBER,
		};

		Result():
			mContext(CONTEXT_NONE),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(NULL),
			mNativeMember(NULL),
			mOffset(0)
		{}

		Result(Context context):
			mContext(context),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(NULL),
			mNativeMember(NULL),
			mOffset(0)
		{}

		Result(Context context, bi32_t offset):
			mContext(context),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(NULL),
			mNativeMember(NULL),
			mOffset(offset)
		{}

		Result(Context context, const Result &thisPointerResult, const NamedInitializer *nativeMember = NULL):
			mContext(context),
			mThisPointerContext(thisPointerResult.mContext),
			mConstantValue(NULL),
			mNativeMember(nativeMember),
			mOffset(thisPointerResult.mOffset)
		{}

		Result(const TypeAndValue *constantValue):
			mContext(CONTEXT_CONSTANT_VALUE),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(constantValue),
			mNativeMember(NULL),
			mOffset(0)
		{}

		Result GetThisPointerResult() const { return Result(mThisPointerContext, mOffset); }

		Context mContext;
		Context mThisPointerContext;
		const TypeAndValue *mConstantValue;
		const NamedInitializer *mNativeMember;
		bi32_t mOffset;
	};

	struct JumpEntry
	{
		JumpEntry(size_t opCodePos, size_t toLabel):
			mOpCodePos(opCodePos),
			mToLabel(toLabel),
			mToPos(0)
		{}
		size_t mOpCodePos;
		size_t mToLabel;
		size_t mToPos;
		size_t GetFromPos() const { return mOpCodePos + 3; }
	};

	struct JumpEntryOpCodePosComparator
	{
		bool operator()(const JumpEntry &entry, size_t opCodePos) const;
	};

	typedef Vector<bu8_t> ByteCode;
	typedef Vector<size_t> LabelList;
	typedef Vector<JumpEntry> JumpList;

	struct CompiledFunction
	{
		CompiledFunction(
				const FunctionDefinition *definition,
				Allocator &allocator,
				bu32_t argSize,
				bu32_t packedArgSize,
				bu32_t framePointerAlignment):
			mDefinition(definition),
			mByteCode(ByteCode::Allocator(&allocator)),
			mLabelList(LabelList::Allocator(&allocator)),
			mJumpList(JumpList::Allocator(&allocator)),
			mArgSize(argSize),
			mPackedArgSize(packedArgSize),
			mLocalSize(0),
			mStackSize(0),
			mFramePointerAlignment(framePointerAlignment)
		{}
		const FunctionDefinition *mDefinition;
		ByteCode::Type mByteCode;
		LabelList::Type mLabelList;
		JumpList::Type mJumpList;
		bu32_t mArgSize;
		bu32_t mPackedArgSize;
		bu32_t mLocalSize;
		bu32_t mStackSize;
		bu32_t mFramePointerAlignment;
	};

	typedef Map<HashedString, bu16_t> StringIndexMap;
	typedef Map<Value32, bu16_t> Value32IndexMap;
	typedef Map<Value64, bu16_t> Value64IndexMap;
	typedef List<HashedString> StringList;
	typedef Vector<Value32> Value32List;
	typedef Vector<Value64> Value64List;
	typedef List<CompiledFunction> FunctionList;
	typedef Vector<const NamedInitializer *> NamedInitializerList;
	typedef AutoStack<CompiledFunction *> FunctionStack;
	typedef AutoStack<Result> ResultStack;
	typedef SizeStack LabelStack;

	virtual void Traverse(const ParseNode *parseNode);
	void TraverseOmitOptionalTemporaries(const Expression *expression);
	bool TraverseCollapseNotOperators(const Expression *expression);

	virtual void Visit(const TranslationUnit *translationUnit);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const TypeDescriptor *typeDescriptor) {}
	virtual void Visit(const NamedInitializer *namedInitializer);
	virtual void Visit(const CompoundStatement *compoundStatement);
	virtual void Visit(const IfStatement *ifStatement);
	virtual void Visit(const SwitchStatement *switchStatement);
	virtual void Visit(const SwitchSection *switchSection);
	virtual void Visit(const WhileStatement *whileStatement);
	virtual void Visit(const ForStatement *forStatement);
	virtual void Visit(const JumpStatement *jumpStatement);
	virtual void Visit(const ExpressionStatement *expressionStatement);
	virtual void Visit(const ConditionalExpression *conditionalExpression);
	virtual void Visit(const BinaryExpression *binaryExpression);
	virtual void Visit(const UnaryExpression *unaryExpression);
	virtual void Visit(const PostfixExpression *postfixExpression);
	virtual void Visit(const MemberExpression *memberExpression);
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(const FunctionCallExpression *functionCallExpression);
	virtual void Visit(const CastExpression *castExpression);
	virtual void Visit(const SizeofExpression *sizeofExpression);
	virtual void Visit(const ConstantExpression *constantExpression);
	virtual void Visit(const IdentifierExpression *identifierExpression);
	virtual void Visit(const ThisExpression *thisExpression);

	bool ProcessConstantExpression(const Expression *expression);

	Result EmitCallNativeGetter(const Result &thisPointerResult, const NamedInitializer *member);

	void EmitPushResultAs(const Result &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType);
	void EmitPushResult(const Result &result, const TypeDescriptor *typeDescriptor);
	void EmitPushFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPushFramePointerIndirectValue32(bi32_t offset);
	void EmitPushFramePointerIndirectValue64(bi32_t offset);
	void EmitPushAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPushStackValue(const TypeDescriptor *typeDescriptor, bi32_t offset);

	void EmitPushConstantAs(const TypeAndValue &typeAndValue, const TypeDescriptor *resultType);
	void EmitPushConstant(const TypeAndValue &typeAndValue);
	void EmitPushConstantInt(bi32_t value);
	void EmitPushConstantUInt(bu32_t value);
	void EmitPushConstantLong(bi64_t value);
	void EmitPushConstantULong(bu64_t value);
	void EmitPushConstantFloat(bf32_t value);
	void EmitPushConstantDouble(bf64_t value);

	void EmitPopResult(const Result &result, const TypeDescriptor *typeDescriptor);
	void EmitPopFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPopFramePointerIndirectValue32(bi32_t offset);
	void EmitPopFramePointerIndirectValue64(bi32_t offset);
	void EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	Result EmitAccumulateAddressOffset(const Result &result);
	void EmitAccumulateAddressOffset(bi32_t offset);

	void EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType);

	Result EmitCommaOperator(const BinaryExpression *binaryExpression);
	Result EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);

	void EmitLogicalOperator(
		const BinaryExpression *binaryExpression,
		OpCode branchOpCode,
		OpCode negatedOpCode,
		OpCode altBranchOpCode,
		OpCode negatedAltOpCode);

	Result EmitAssignmentOperator(const BinaryExpression *binaryExpression);
	Result EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);
	Result EmitPointerCompoundAssignmentOperator(const Expression *pointerExpression, const Expression *offsetExpression, int sign);
	Result EmitPointerArithmetic(const Expression *pointerExpression, const Expression *offsetExpression, int sign);
	Result EmitPointerComparison(const Expression *lhs, const Expression *rhs, const OpCodeSet &opCodeSet);
	Result EmitPointerDifference(const Expression *lhs, const Expression *rhs);
	void EmitPointerOffset(const Expression *offsetExpression, bi32_t elementSize);

	Result EmitPointerIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign);
	Result EmitIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign);
	Result EmitSignOperator(const UnaryExpression *unaryExpression);
	Result EmitNotOperator(const UnaryExpression *unaryExpression);
	Result EmitBitwiseNotOperator(const UnaryExpression *unaryExpression);
	Result EmitAddressOfOperator(const UnaryExpression *unaryExpression);
	void EmitPushAddressOfResult(const Result &result);
	Result EmitAddressOfResult(const Result &result);
	Result EmitDereferenceOperator(const UnaryExpression *unaryExpression);
	Result EmitDereferenceResult(const Result &result, const TypeDescriptor *typeDescriptor);

	void EmitArgumentList(const Expression *argList, const Parameter *paramList);

	void EmitJump(OpCode opCode, size_t toLabel);

	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitOpCode(OpCode opCode);
	void EmitValue16(Value16 value);
	void EmitValue32At(Value32 value, size_t pos);
	void EmitIndexedValue32(Value32 value);
	void EmitIndexedValue64(Value64 value);
	void EmitHashCode(bu32_t hash);

	Result::Context TransformContext(Result::Context targetContext, const TypeDescriptor *typeDescriptor) const;

	void ResolveJumps();
	const JumpEntry *FindJumpEntry(const JumpList::Type &jumpList, size_t opCodePos) const;

	void WriteConstantTable();
	void WriteFunctionList(bu16_t functionIndex);
	void WriteNativeMemberList(bu16_t functionIndex);
	void WriteQualifiedSymbolName(const Symbol *symbol);
	void WriteReturnSignature(const TypeDescriptor *type);
	void WriteParamListSignature(const Parameter *parameterList, bool includeThis);
	void WriteSymbolNameIndices(const Symbol *symbol);
	void WriteValue16(Value16 value);
	void WriteValue32(Value32 value);
	void WriteValue64(Value64 value);

	bool Is64BitPointer() const { return mPointerSize == POINTER_64BIT; }
	CompiledFunction &GetFunction() { return *mFunction.GetTop(); }
	ByteCode::Type &GetByteCode() { return GetFunction().mByteCode; }
	void ApplyStackDelta(bi32_t delta);
	bi32_t AllocateLocal(const TypeDescriptor* typeDescriptor);
	size_t CreateLabel();
	void SetLabelValue(size_t label, size_t value);
	void MapQualifiedSymbolName(const Symbol *symbol);
	bu16_t MapString(const HashedString &str);
	bu16_t MapValue32(const Value32 &value32);
	bu16_t MapValue64(const Value64 &value32);

	void AssertStackEmpty();
	void PushError(CompilerError::Type type);

	StringIndexMap::Type mStringIndexMap;
	StringList::Type mStringList;
	Value32IndexMap::Type mValue32IndexMap;
	Value32List::Type mValue32List;
	Value64IndexMap::Type mValue64IndexMap;
	Value64List::Type mValue64List;
	FunctionList::Type mFunctionList;
	NamedInitializerList::Type mNativeMemberList;
	ResultStack mResult;
	FunctionStack mFunction;
	LabelStack mContinueLabel;
	LabelStack mBreakLabel;
	UIntStack mLocalOffset;
	UIntStack mStackTop;
	BoolStack mEmitOptionalTemporaries;
	BoolStack mCollapseNotOperators;
	BoolStack mExpressionIsNegated;
	Allocator &mAllocator;
	BinaryWriter &mWriter;
	CompilerErrorBuffer &mErrorBuffer;
	const TranslationUnit *mTranslationUnitList;
	PointerSize mPointerSize;
};


void CodeGenerator::Generate(const TranslationUnit *translationUnitList, BinaryWriter &writer)
{
	GeneratorCore generator(mAllocator, writer, mErrorBuffer, translationUnitList, mPointerSize);
	generator.Generate();
}


void GeneratorCore::Generate()
{
	TraverseList(mTranslationUnitList);

	WriteValue32(Value32(MAGIC_NUMBER));
	WriteValue16(Value16(MAJOR_VERSION));
	WriteValue16(Value16(MINOR_VERSION));
	WriteValue16(Value16(EncodePointerSize(0, mPointerSize)));

	const bu16_t listIndex = MapString("List");
	const bu16_t functionIndex = mFunctionList.empty() ? 0 : MapString("Func");

	ResolveJumps();
	WriteConstantTable();

	// Cache the start position and skip 4 bytes for the blob size.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value32));

	WriteValue16(Value16(listIndex));
	WriteValue32(Value32(/* mDefinitionList.size() + */ bu32_t(mFunctionList.size()) + bu32_t(2 * mNativeMemberList.size())));

	WriteFunctionList(functionIndex);
	WriteNativeMemberList(functionIndex);

	// Patch up the blob size.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue32(Value32(endPos - startPos));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::Traverse(const ParseNode *parseNode)
{
	BoolStack::Element emitOptionalTemporariesElement(mEmitOptionalTemporaries, true);
	BoolStack::Element collapseNotOperatorsElement(mCollapseNotOperators, false);
	ParseNodeTraverser::Traverse(parseNode);
}


void GeneratorCore::TraverseOmitOptionalTemporaries(const Expression *expression)
{
	BoolStack::Element emitOptionalTemporariesElement(mEmitOptionalTemporaries, false);
	BoolStack::Element collapseNotOperatorsElement(mCollapseNotOperators, false);
	ParseNodeTraverser::Traverse(expression);

	// Remove any temporaries that may have been left on the stack.
	const Result::Context context = mResult.GetTop().mContext;
	if ((context == Result::CONTEXT_ADDRESS_INDIRECT) ||
	    (context == Result::CONTEXT_STACK_VALUE))
	{
		EmitOpCode(OPCODE_POP);
	}
}


bool GeneratorCore::TraverseCollapseNotOperators(const Expression *expression)
{
	BoolStack::Element emitOptionalTemporariesElement(mEmitOptionalTemporaries, true);
	BoolStack::Element collapseNotOperatorsElement(mCollapseNotOperators, true);
	BoolStack::Element expressionIsNegatedElement(mExpressionIsNegated, false);
	ParseNodeTraverser::Traverse(expression);
	return expressionIsNegatedElement.GetValue();
}


void GeneratorCore::Visit(const TranslationUnit *translationUnit)
{
	if (translationUnit->RequiresCodeGeneration())
	{
		ParseNodeTraverser::Visit(translationUnit);
	}
}


void GeneratorCore::Visit(const FunctionDefinition *functionDefinition)
{
	const FunctionPrototype *prototype = functionDefinition->GetPrototype();
	bu32_t offset = (functionDefinition->GetScope() == SCOPE_STRUCT_MEMBER) ? bu32_t(BOND_SLOT_SIZE) : 0;
	bu32_t packedOffset = offset;
	bu32_t framePointerAlignment = bu32_t(BOND_SLOT_SIZE);
	const Parameter *parameterList = prototype->GetParameterList();
	while (parameterList != NULL)
	{
		const TypeDescriptor *typeDescriptor = parameterList->GetTypeDescriptor();
		const bu32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), bu32_t(BOND_SLOT_SIZE));
		offset += typeDescriptor->GetSize(mPointerSize);
		offset = AlignUp(offset, alignment);
		packedOffset += typeDescriptor->GetStackSize(mPointerSize);
		framePointerAlignment = Max(framePointerAlignment, alignment);
		parameterList->SetOffset(-bi32_t(offset));
		parameterList = NextNode(parameterList);
	}

	CompiledFunction &function = *mFunctionList.insert(
		mFunctionList.end(),
		CompiledFunction(
			functionDefinition,
			mAllocator,
			offset,
			packedOffset,
			framePointerAlignment));
	function.mLabelList.resize(functionDefinition->GetNumReservedJumpTargetIds());
	MapQualifiedSymbolName(functionDefinition);

	if (!functionDefinition->IsNative())
	{
		FunctionStack::Element functionElement(mFunction, &function);
		UIntStack::Element localOffsetElement(mLocalOffset, 0);
		UIntStack::Element stackTopElement(mStackTop, 0);
		Traverse(functionDefinition->GetBody());

		if (functionDefinition->GetPrototype()->GetReturnType()->IsVoidType())
		{
			EmitOpCode(OPCODE_RETURN);
		}
	}
}


void GeneratorCore::Visit(const NamedInitializer *namedInitializer)
{
	switch (namedInitializer->GetScope())
	{
		case SCOPE_GLOBAL:
			//MapString("Decl");
			//MapString(namedInitializer->GetName()->GetHashedText());
			// TODO: output initializer data.
			break;

		case SCOPE_LOCAL:
		{
			// TODO: Omit code generation and stack allocation for non lvalue foldable constants.
			// Allocate stack space for the local variable.
			const TypeDescriptor *lhDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
			namedInitializer->SetOffset(AllocateLocal(lhDescriptor));

			const Initializer *initializer = namedInitializer->GetInitializer();
			if ((initializer != NULL) && (initializer->GetExpression() != NULL))
			{
				UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());

				if (lhDescriptor->IsStructType())
				{
					EmitOpCodeWithOffset(OPCODE_LOADFP, namedInitializer->GetOffset());
					ResultStack::Element rhResult(mResult);
					Traverse(initializer);
					EmitPushAddressOfResult(rhResult);
					EmitOpCode(OPCODE_MEMCOPYW);
					EmitIndexedValue32(Value32(lhDescriptor->GetSize(mPointerSize)));
				}
				else
				{
					const TypeDescriptor *rhDescriptor = initializer->GetExpression()->GetTypeDescriptor();
					Result lhResult(Result::CONTEXT_FP_INDIRECT, namedInitializer->GetOffset());
					ResultStack::Element rhResult(mResult);
					Traverse(initializer);
					EmitPushResultAs(rhResult, rhDescriptor, lhDescriptor);
					EmitPopResult(lhResult, lhDescriptor);
				}
			}
			else
			{
				// TODO: Handle struct and array initializers.
			}
		}
		break;

		case SCOPE_STRUCT_MEMBER:
			if (namedInitializer->IsNativeStructMember())
			{
				mNativeMemberList.push_back(namedInitializer);
				MapQualifiedSymbolName(namedInitializer);
			}
			break;
	}
}


void GeneratorCore::Visit(const CompoundStatement *compoundStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	ParseNodeTraverser::Visit(compoundStatement);
	AssertStackEmpty();
}


void GeneratorCore::Visit(const IfStatement *ifStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	const Expression *condition = ifStatement->GetCondition();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();

	const TypeAndValue &conditionTav = ifStatement->GetCondition()->GetTypeAndValue();
	if (conditionTav.IsValueDefined())
	{
		if (conditionTav.GetBoolValue())
		{
			Traverse(ifStatement->GetThenStatement());
		}
		else if (ifStatement->GetElseStatement() != NULL)
		{
			Traverse(ifStatement->GetElseStatement());
		}
	}
	else
	{
		ResultStack::Element conditionResult(mResult);
		const bool negated = TraverseCollapseNotOperators(condition);
		EmitPushResult(conditionResult, conditionDescriptor);

		const size_t thenEndLabel = CreateLabel();
		EmitJump(negated ? OPCODE_IFNZ : OPCODE_IFZ, thenEndLabel);

		Traverse(ifStatement->GetThenStatement());
		size_t thenEndPos = GetByteCode().size();

		if (ifStatement->GetElseStatement() != NULL)
		{
			size_t elseEndLabel = CreateLabel();
			EmitJump(OPCODE_GOTO, elseEndLabel);
			thenEndPos = GetByteCode().size();

			Traverse(ifStatement->GetElseStatement());
			SetLabelValue(elseEndLabel, GetByteCode().size());
		}

		SetLabelValue(thenEndLabel, thenEndPos);
	}
	AssertStackEmpty();
}


void GeneratorCore::Visit(const SwitchStatement *switchStatement)
{
	const size_t endLabel = CreateLabel();
	LabelStack::Element breakElement(mBreakLabel, endLabel);
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());

	const Expression *control = switchStatement->GetControl();
	const TypeDescriptor *controlDescriptor = control->GetTypeDescriptor();
	ResultStack::Element controlResult(mResult);
	Traverse(control);
	EmitPushResult(controlResult, controlDescriptor);

	const bu32_t numMatches = switchStatement->GetNumMatches();
	const bi32_t minMatch = switchStatement->GetMinMatch();
	const bi32_t maxMatch = switchStatement->GetMaxMatch();
	const bu32_t range = maxMatch - minMatch;
	const bool doLookupSwitch = (numMatches <= 0) || (((numMatches * 2) - 2) < range);
	size_t jumpTableSize = 0;

	if (doLookupSwitch)
	{
		EmitOpCode(OPCODE_LOOKUPSWITCH);
		jumpTableSize = (2 + (numMatches * 2)) * sizeof(Value32);
	}
	else
	{
		EmitOpCode(OPCODE_TABLESWITCH);
		jumpTableSize = (4 + range) * sizeof(Value32);
	}

	ByteCode::Type &byteCode = GetByteCode();
	const size_t jumpTableStart = AlignUp(byteCode.size(), sizeof(Value32));
	const size_t jumpTableEnd = jumpTableStart + jumpTableSize;

	// Add space for the jump table and emit the switch sections.
	byteCode.resize(jumpTableEnd, 0);
	TraverseList(switchStatement->GetSectionList());
	const size_t endPos = byteCode.size();
	SetLabelValue(endLabel, endPos);

	// Now that the switch sections have been generated, the jump table can be resolved.
	const LabelList::Type &labelList = GetFunction().mLabelList;
	const ResolvedSwitchLabel *resolvedLabels = switchStatement->GetResolvedLabelList();

	bi32_t defaultOffset = 0;
	if ((resolvedLabels != NULL) && resolvedLabels->IsDefault())
	{
		defaultOffset = bu32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
		resolvedLabels = NextNode(resolvedLabels);
	}
	else
	{
		defaultOffset = bu32_t(endPos - jumpTableEnd);
	}

	EmitValue32At(Value32(defaultOffset), jumpTableStart);

	if (doLookupSwitch)
	{
		EmitValue32At(Value32(numMatches), jumpTableStart + sizeof(Value32));
		size_t pos = jumpTableStart + (2 * sizeof(Value32));
		while (resolvedLabels != NULL)
		{
			const bi32_t offset = bi32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
			EmitValue32At(Value32(resolvedLabels->GetMatch()), pos);
			EmitValue32At(Value32(offset), pos + sizeof(Value32));
			pos += 2 * sizeof(Value32);
			resolvedLabels = NextNode(resolvedLabels);
		}
	}
	else
	{
		EmitValue32At(Value32(minMatch), jumpTableStart + sizeof(Value32));
		EmitValue32At(Value32(maxMatch), jumpTableStart + (2 * sizeof(Value32)));
		size_t pos = jumpTableStart + (3 * sizeof(Value32));
		for (bu32_t i = 0; i <= range; ++i)
		{
			const bi32_t match = minMatch + i;
			bi32_t offset = 0;
			if (match >= resolvedLabels->GetMatch())
			{
				offset = bi32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
				EmitValue32At(Value32(offset), pos);
				resolvedLabels = NextNode(resolvedLabels);
			}
			else
			{
				offset = defaultOffset;
			}
			EmitValue32At(Value32(offset), pos);
			pos += sizeof(Value32);
		}
	}
	AssertStackEmpty();
}


void GeneratorCore::Visit(const SwitchSection *switchSection)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	const size_t sectionStartPos = GetByteCode().size();
	SetLabelValue(switchSection->GetJumpTargetId(), sectionStartPos);
	TraverseList(switchSection->GetStatementList());
	AssertStackEmpty();
}


void GeneratorCore::Visit(const WhileStatement *whileStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	const size_t loopStartLabel = CreateLabel();
	const size_t loopEndLabel = CreateLabel();
	SetLabelValue(loopStartLabel, GetByteCode().size());

	LabelStack::Element continueElement(mContinueLabel, loopStartLabel);
	LabelStack::Element breakElement(mBreakLabel, loopEndLabel);

	const Expression *condition = whileStatement->GetCondition();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();

	if (whileStatement->IsDoLoop())
	{
		Traverse(whileStatement->GetBody());

		ResultStack::Element conditionResult(mResult);
		const bool negated = TraverseCollapseNotOperators(condition);
		EmitPushResult(conditionResult, conditionDescriptor);
		EmitJump(negated ? OPCODE_IFZ : OPCODE_IFNZ, loopStartLabel);
	}
	else
	{
		ResultStack::Element conditionResult(mResult);
		const bool negated = TraverseCollapseNotOperators(condition);
		EmitPushResult(conditionResult, conditionDescriptor);
		EmitJump(negated ? OPCODE_IFNZ : OPCODE_IFZ, loopEndLabel);

		Traverse(whileStatement->GetBody());

		EmitJump(OPCODE_GOTO, loopStartLabel);
	}

	SetLabelValue(loopEndLabel, GetByteCode().size());
	AssertStackEmpty();
}


void GeneratorCore::Visit(const ForStatement *forStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	Traverse(forStatement->GetInitializer());

	const size_t loopStartLabel = CreateLabel();
	const size_t countingLabel = CreateLabel();
	const size_t loopEndLabel = CreateLabel();
	SetLabelValue(loopStartLabel, GetByteCode().size());

	LabelStack::Element continueElement(mContinueLabel, countingLabel);
	LabelStack::Element breakElement(mBreakLabel, loopEndLabel);

	const Expression *condition = forStatement->GetCondition();
	if (condition != NULL)
	{
		const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();
		ResultStack::Element conditionResult(mResult);
		const bool negated = TraverseCollapseNotOperators(condition);
		EmitPushResult(conditionResult, conditionDescriptor);
		EmitJump(negated ? OPCODE_IFNZ : OPCODE_IFZ, loopEndLabel);
	}

	Traverse(forStatement->GetBody());
	SetLabelValue(countingLabel, GetByteCode().size());

	ResultStack::Element countingResult(mResult);
	TraverseOmitOptionalTemporaries(forStatement->GetCountingExpression());

	EmitJump(OPCODE_GOTO, loopStartLabel);
	SetLabelValue(loopEndLabel, GetByteCode().size());
	AssertStackEmpty();
}


void GeneratorCore::Visit(const JumpStatement *jumpStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());

	if (jumpStatement->IsBreak())
	{
		EmitJump(OPCODE_GOTO, mBreakLabel.GetTop());
	}
	else if (jumpStatement->IsContinue())
	{
		EmitJump(OPCODE_GOTO, mContinueLabel.GetTop());
	}
	else if (jumpStatement->IsReturn())
	{
		const TypeDescriptor *returnDescriptor = GetFunction().mDefinition->GetPrototype()->GetReturnType();
		if (!returnDescriptor->IsVoidType())
		{
			const Expression *rhs = jumpStatement->GetRhs();
			const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
			ResultStack::Element rhResult(mResult);
			Traverse(rhs);

			if (returnDescriptor->IsPointerType())
			{
				EmitPushResultAs(rhResult, rhDescriptor, returnDescriptor);
				EmitOpCode(RETURN_OPCODES.GetPointerOpCode(mPointerSize));
			}
			else
			{
				switch (returnDescriptor->GetPrimitiveType())
				{
					case Token::KEY_BOOL:
					case Token::KEY_CHAR:
					case Token::KEY_UCHAR:
					case Token::KEY_SHORT:
					case Token::KEY_USHORT:
					case Token::KEY_INT:
					case Token::KEY_UINT:
					case Token::KEY_FLOAT:
						EmitPushResultAs(rhResult, rhDescriptor, returnDescriptor);
						EmitOpCode(OPCODE_RETURN32);
						break;

					case Token::KEY_LONG:
					case Token::KEY_ULONG:
					case Token::KEY_DOUBLE:
						EmitPushResultAs(rhResult, rhDescriptor, returnDescriptor);
						EmitOpCode(OPCODE_RETURN64);
						break;

					default:
						EmitPushAddressOfResult(rhResult);
						EmitOpCode(OPCODE_RETURNMEMW);
						EmitIndexedValue32(Value32(returnDescriptor->GetSize(mPointerSize)));
						break;
				}
			}
		}
		else
		{
			EmitOpCode(OPCODE_RETURN);
		}
	}
	AssertStackEmpty();
}


void GeneratorCore::Visit(const ExpressionStatement *expressionStatement)
{
	UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());
	ResultStack::Element expressionResult(mResult);
	TraverseOmitOptionalTemporaries(expressionStatement->GetExpression());
	AssertStackEmpty();
}


void GeneratorCore::Visit(const ConditionalExpression *conditionalExpression)
{
	const Expression *condition = conditionalExpression->GetCondition();
	const Expression *trueExpression = conditionalExpression->GetTrueExpression();
	const Expression *falseExpression = conditionalExpression->GetFalseExpression();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();
	const TypeDescriptor *trueDescriptor = trueExpression->GetTypeDescriptor();
	const TypeDescriptor *falseDescriptor = falseExpression->GetTypeDescriptor();
	const TypeDescriptor *resultDescriptor = conditionalExpression->GetTypeDescriptor();
	const bool isStruct = resultDescriptor->IsStructType();

	{
		ResultStack::Element conditionResult(mResult);
		const bool negated = TraverseCollapseNotOperators(condition);
		EmitPushResult(conditionResult, conditionDescriptor);

		const size_t trueEndLabel = CreateLabel();
		EmitJump(negated ? OPCODE_IFNZ : OPCODE_IFZ, trueEndLabel);

		{
			UIntStack::Element stackTopElement(mStackTop, mStackTop.GetTop());
			ResultStack::Element trueResult(mResult);
			Traverse(trueExpression);
			if (isStruct)
			{
				EmitPushAddressOfResult(trueResult);
			}
			else
			{
				EmitPushResultAs(trueResult, trueDescriptor, resultDescriptor);
			}
		}

		const size_t falseEndLabel = CreateLabel();
		EmitJump(OPCODE_GOTO, falseEndLabel);
		SetLabelValue(trueEndLabel, GetByteCode().size());

		ResultStack::Element falseResult(mResult);
		Traverse(falseExpression);
		if (isStruct)
		{
			EmitPushAddressOfResult(falseResult);
		}
		else
		{
			EmitPushResultAs(falseResult, falseDescriptor, resultDescriptor);
		}

		SetLabelValue(falseEndLabel, GetByteCode().size());
	}

	mResult.SetTop(Result(isStruct ?
		Result::CONTEXT_ADDRESS_INDIRECT :
		Result::CONTEXT_STACK_VALUE));
}


void GeneratorCore::Visit(const BinaryExpression *binaryExpression)
{
	if (!ProcessConstantExpression(binaryExpression))
	{
		const Expression *lhs = binaryExpression->GetLhs();
		const Expression *rhs = binaryExpression->GetRhs();
		const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
		const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
		const Token *op = binaryExpression->GetOperator();

		Result result = Result(Result::CONTEXT_STACK_VALUE);

		switch (op->GetTokenType())
		{
			case Token::COMMA:
				result = EmitCommaOperator(binaryExpression);
				break;
			case Token::ASSIGN:
				result = EmitAssignmentOperator(binaryExpression);
				break;
			case Token::ASSIGN_LEFT:
				result = EmitCompoundAssignmentOperator(binaryExpression, LSH_OPCODES);
				break;
			case Token::ASSIGN_RIGHT:
				result = EmitCompoundAssignmentOperator(binaryExpression, RSH_OPCODES);
				break;
			case Token::ASSIGN_MOD:
				result = EmitCompoundAssignmentOperator(binaryExpression, REM_OPCODES);
				break;
			case Token::ASSIGN_AND:
				result = EmitCompoundAssignmentOperator(binaryExpression, AND_OPCODES);
				break;
			case Token::ASSIGN_OR:
				result = EmitCompoundAssignmentOperator(binaryExpression, OR_OPCODES);
				break;
			case Token::ASSIGN_XOR:
				result = EmitCompoundAssignmentOperator(binaryExpression, XOR_OPCODES);
				break;

			case Token::ASSIGN_PLUS:
				if (lhDescriptor->IsPointerType())
				{
					result = EmitPointerCompoundAssignmentOperator(lhs, rhs, 1);
				}
				else
				{
					result = EmitCompoundAssignmentOperator(binaryExpression, ADD_OPCODES);
				}
				break;

			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					result = EmitPointerCompoundAssignmentOperator(lhs, rhs, -1);
				}
				else
				{
					result = EmitCompoundAssignmentOperator(binaryExpression, SUB_OPCODES);
				}
				break;

			case Token::ASSIGN_MULT:
				result = EmitCompoundAssignmentOperator(binaryExpression, MUL_OPCODES);
				break;
			case Token::ASSIGN_DIV:
				result = EmitCompoundAssignmentOperator(binaryExpression, DIV_OPCODES);
				break;
			case Token::OP_AND:
				EmitLogicalOperator(binaryExpression, OPCODE_BRZ, OPCODE_NBRZ, OPCODE_BRNZ, OPCODE_NBRNZ);
				break;
			case Token::OP_OR:
				EmitLogicalOperator(binaryExpression, OPCODE_BRNZ, OPCODE_NBRNZ, OPCODE_BRZ, OPCODE_NBRZ);
				break;
			case Token::OP_AMP:
				result = EmitSimpleBinaryOperator(binaryExpression, AND_OPCODES);
				break;
			case Token::OP_BIT_OR:
				result = EmitSimpleBinaryOperator(binaryExpression, OR_OPCODES);
				break;
			case Token::OP_BIT_XOR:
				result = EmitSimpleBinaryOperator(binaryExpression, XOR_OPCODES);
				break;
			case Token::OP_MOD:
				result = EmitSimpleBinaryOperator(binaryExpression, REM_OPCODES);
				break;
			case Token::OP_LEFT:
				result = EmitSimpleBinaryOperator(binaryExpression, LSH_OPCODES);
				break;
			case Token::OP_RIGHT:
				result = EmitSimpleBinaryOperator(binaryExpression, RSH_OPCODES);
				break;
			case Token::OP_LT:
				result = EmitSimpleBinaryOperator(binaryExpression, CMPLT_OPCODES);
				break;
			case Token::OP_LTE:
				result = EmitSimpleBinaryOperator(binaryExpression, CMPLE_OPCODES);
				break;
			case Token::OP_GT:
				result = EmitSimpleBinaryOperator(binaryExpression, CMPGT_OPCODES);
				break;
			case Token::OP_GTE:
				result = EmitSimpleBinaryOperator(binaryExpression, CMPGE_OPCODES);
				break;
			case Token::OP_EQUAL:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPEQ_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPEQ_OPCODES);
				}
				break;
			case Token::OP_NOT_EQUAL:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPNEQ_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPNEQ_OPCODES);
				}
				break;

			case Token::OP_PLUS:
			{
				if (lhDescriptor->IsPointerType())
				{
					result = EmitPointerArithmetic(lhs, rhs, 1);
				}
				else if (rhDescriptor->IsPointerType())
				{
					result = EmitPointerArithmetic(rhs, lhs, 1);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, ADD_OPCODES);
				}
			}
			break;

			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerDifference(lhs, rhs);
				}
				else if (lhDescriptor->IsPointerType())
				{
					result = EmitPointerArithmetic(lhs, rhs, -1);
				}
				else if (rhDescriptor->IsPointerType())
				{
					result = EmitPointerArithmetic(rhs, lhs, -1);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, SUB_OPCODES);
				}
				break;

			case Token::OP_STAR:
				result = EmitSimpleBinaryOperator(binaryExpression, MUL_OPCODES);
				break;
			case Token::OP_DIV:
				result = EmitSimpleBinaryOperator(binaryExpression, DIV_OPCODES);
				break;

			default:
				break;
		}

		mResult.SetTop(result);
	}
}


void GeneratorCore::Visit(const UnaryExpression *unaryExpression)
{
	if (!ProcessConstantExpression(unaryExpression))
	{
		const Expression *rhs = unaryExpression->GetRhs();
		const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
		const Token *op = unaryExpression->GetOperator();
		Result result;

		switch (op->GetTokenType())
		{
			case Token::OP_PLUS:
			case Token::OP_MINUS:
				result = EmitSignOperator(unaryExpression);
				break;

			case Token::OP_INC:
				if (rhDescriptor->IsPointerType())
				{
					result = EmitPointerIncrementOperator(unaryExpression, unaryExpression->GetRhs(), PREFIX, 1);
				}
				else
				{
					result = EmitIncrementOperator(unaryExpression, unaryExpression->GetRhs(), PREFIX, 1);
				}
				break;

			case Token::OP_DEC:
				if (rhDescriptor->IsPointerType())
				{
					result = EmitPointerIncrementOperator(unaryExpression, unaryExpression->GetRhs(), PREFIX, -1);
				}
				else
				{
					result = EmitIncrementOperator(unaryExpression, unaryExpression->GetRhs(), PREFIX, -1);
				}
				break;

			case Token::OP_NOT:
				result = EmitNotOperator(unaryExpression);
				break;

			case Token::OP_AMP:
				result = EmitAddressOfOperator(unaryExpression);
				break;

			case Token::OP_BIT_NOT:
				result = EmitBitwiseNotOperator(unaryExpression);
				break;

			case Token::OP_STAR:
				result = EmitDereferenceOperator(unaryExpression);
				break;

			default:
				break;
		}

		mResult.SetTop(result);
	}
}


void GeneratorCore::Visit(const PostfixExpression *postfixExpression)
{
	const Expression *lhs = postfixExpression->GetLhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const Token *op = postfixExpression->GetOperator();
	Result result;

	switch (op->GetTokenType())
	{
		case Token::OP_INC:
			if (lhDescriptor->IsPointerType())
			{
				result = EmitPointerIncrementOperator(postfixExpression, postfixExpression->GetLhs(), POSTFIX, 1);
			}
			else
			{
				result = EmitIncrementOperator(postfixExpression, postfixExpression->GetLhs(), POSTFIX, 1);
			}
			break;

		case Token::OP_DEC:
			if (lhDescriptor->IsPointerType())
			{
				result = EmitPointerIncrementOperator(postfixExpression, postfixExpression->GetLhs(), POSTFIX, -1);
			}
			else
			{
				result = EmitIncrementOperator(postfixExpression, postfixExpression->GetLhs(), POSTFIX, -1);
			}
			break;

		default:
			break;
	}
	mResult.SetTop(result);
}


void GeneratorCore::Visit(const MemberExpression *memberExpression)
{
	Result result;
	{
		const Expression *lhs = memberExpression->GetLhs();
		const Token *op = memberExpression->GetOperator();
		ResultStack::Element lhResult(mResult);
		Traverse(lhs);
		result = lhResult;

		const Symbol *member = memberExpression->GetDefinition();
		const NamedInitializer *namedInitializer = NULL;
		if ((namedInitializer = CastNode<NamedInitializer>(member)) != NULL)
		{
			if (namedInitializer->IsNativeStructMember())
			{
				if (op->GetTokenType() == Token::PERIOD)
				{
					result = EmitAddressOfResult(lhResult);
				}
				result = Result(Result::CONTEXT_NATIVE_MEMBER, result, namedInitializer);
			}
			else
			{
				const TypeDescriptor *typeDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
				if (op->GetTokenType() == Token::OP_ARROW)
				{
					result = EmitDereferenceResult(lhResult, lhs->GetTypeDescriptor());
				}

				result.mContext = TransformContext(result.mContext, typeDescriptor);
				result.mOffset += namedInitializer->GetOffset();
			}
		}
		else if (CastNode<FunctionDefinition>(member) != NULL)
		{
			if (op->GetTokenType() == Token::PERIOD)
			{
				result = EmitAddressOfResult(lhResult);
			}
			result = Result(Result::CONTEXT_MEMBER_FUNCTION, result);
		}
	}

	mResult.SetTop(result);
}


void GeneratorCore::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	if (!ProcessConstantExpression(arraySubscriptExpression))
	{
		const Expression *lhs = arraySubscriptExpression->GetLhs();
		const Expression *index = arraySubscriptExpression->GetIndex();
		Result result = EmitPointerArithmetic(lhs, index, 1);
		if (!arraySubscriptExpression->GetTypeDescriptor()->IsArrayType())
		{
			result = EmitDereferenceResult(result, lhs->GetTypeDescriptor());
		}
		mResult.SetTop(result);
	}
}


void GeneratorCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	const Expression *lhs = functionCallExpression->GetLhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeSpecifier *lhSpecifier = lhDescriptor->GetTypeSpecifier();
	const FunctionDefinition *function = CastNode<FunctionDefinition>(lhSpecifier->GetDefinition());
	const FunctionPrototype *prototype = function->GetPrototype();
	const TypeDescriptor *returnDescriptor = prototype->GetReturnType();
	const bu32_t returnType = returnDescriptor->GetSignatureType();
	const bi32_t returnOffset = (returnType == SIG_STRUCT) ? AllocateLocal(returnDescriptor) : 0;
	const Parameter *paramList = prototype->GetParameterList();
	const Expression *argList = functionCallExpression->GetArgumentList();

	{
		UIntStack::Element stackTopElement(mStackTop, mStackTop.GetTop());
		EmitArgumentList(argList, paramList);

		ResultStack::Element lhResult(mResult);
		Traverse(lhs);

		// Push the 'this' pointer, if needed.
		if (lhResult.GetValue().mContext == Result::CONTEXT_MEMBER_FUNCTION)
		{
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(lhResult.GetValue().GetThisPointerResult(), &voidStar);
		}

		if (returnType == SIG_STRUCT)
		{
			EmitOpCodeWithOffset(OPCODE_LOADFP, returnOffset);
		}

		EmitOpCode(OPCODE_INVOKE);
		EmitHashCode(function->GetGlobalHashCode());
	}

	if (returnType == SIG_STRUCT)
	{
		mResult.SetTop(Result(Result::CONTEXT_FP_INDIRECT, returnOffset));
	}
	else if (returnType == SIG_VOID)
	{
		mResult.SetTop(Result(Result::CONTEXT_NONE));
	}
	else
	{
		ApplyStackDelta(BOND_SLOT_SIZE);
		mResult.SetTop(Result(Result::CONTEXT_STACK_VALUE));
	}
}


void GeneratorCore::Visit(const CastExpression *castExpression)
{
	if (!ProcessConstantExpression(castExpression))
	{
		{
			const Expression *rhs = castExpression->GetRhs();
			const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
			const TypeDescriptor *resultDescriptor = castExpression->GetTypeDescriptor();
			ResultStack::Element rhResult(mResult);
			Traverse(rhs);
			EmitPushResultAs(rhResult, rhDescriptor, resultDescriptor);
		}
		mResult.SetTop(Result(Result::CONTEXT_STACK_VALUE));
	}
}


void GeneratorCore::Visit(const SizeofExpression *sizeofExpression)
{
	ProcessConstantExpression(sizeofExpression);
}


void GeneratorCore::Visit(const ConstantExpression *constantExpression)
{
	ProcessConstantExpression(constantExpression);
}


void GeneratorCore::Visit(const IdentifierExpression *identifierExpression)
{
	if (!ProcessConstantExpression(identifierExpression))
	{
		const Symbol *symbol = identifierExpression->GetDefinition();
		const NamedInitializer *namedInitializer = NULL;
		const Parameter *parameter = NULL;
		const FunctionDefinition *functionDefinition = NULL;
		if ((namedInitializer = CastNode<NamedInitializer>(symbol)) != NULL)
		{
			const bi32_t offset = namedInitializer->GetOffset();
			const TypeDescriptor *typeDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
			switch (namedInitializer->GetScope())
			{
				case SCOPE_GLOBAL:
				{
					// TODO
				}
				break;
				case SCOPE_LOCAL:
				{
					const Result::Context context =
						TransformContext(Result::CONTEXT_FP_INDIRECT, typeDescriptor);
					mResult.SetTop(Result(context, offset));
				}
				break;
				case SCOPE_STRUCT_MEMBER:
				{
					// Push the this pointer and add the offset.
					if (Is64BitPointer())
					{
						EmitPushFramePointerIndirectValue64(-BOND_SLOT_SIZE);
					}
					else
					{
						EmitPushFramePointerIndirectValue32(-BOND_SLOT_SIZE);
					}
					const Result::Context context = TransformContext(Result::CONTEXT_ADDRESS_INDIRECT, typeDescriptor);
					mResult.SetTop(Result(context, offset));
				}
				break;
			}
		}
		else if ((parameter = CastNode<Parameter>(symbol)) != NULL)
		{
			mResult.SetTop(Result(Result::CONTEXT_FP_INDIRECT, parameter->GetOffset()));
		}
		else if (((functionDefinition = CastNode<FunctionDefinition>(symbol)) != NULL) &&
		         (functionDefinition->GetScope() == SCOPE_STRUCT_MEMBER))
		{
			Result thisPointerResult(Result::CONTEXT_FP_INDIRECT, -BOND_SLOT_SIZE);
			mResult.SetTop(Result(Result::CONTEXT_MEMBER_FUNCTION, thisPointerResult));
		}
	}
}


void GeneratorCore::Visit(const ThisExpression *thisExpression)
{
	mResult.SetTop(Result(Result::CONTEXT_FP_INDIRECT, -BOND_SLOT_SIZE));
}


bool GeneratorCore::ProcessConstantExpression(const Expression *expression)
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsValueDefined())
	{
		mResult.SetTop(Result(&tav));
		return true;
	}
	return false;
}


GeneratorCore::Result GeneratorCore::EmitCallNativeGetter(const Result &thisPointerResult, const NamedInitializer *member)
{
	const TypeDescriptor *returnDescriptor = member->GetTypeAndValue()->GetTypeDescriptor();
	const bu32_t returnType = returnDescriptor->GetSignatureType();
	const bi32_t returnOffset = (returnType == SIG_STRUCT) ? AllocateLocal(returnDescriptor) : 0;

	{
		const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
		EmitPushResult(thisPointerResult, &voidStar);
	}

	if (returnType == SIG_STRUCT)
	{
		EmitOpCodeWithOffset(OPCODE_LOADFP, returnOffset);
	}

	EmitOpCode(OPCODE_INVOKE);
	EmitHashCode(member->GetGlobalHashCodeWithSuffix(BOND_NATIVE_GETTER_SUFFIX));

	bi32_t stackDelta = -BOND_SLOT_SIZE;
	Result result;
	if (returnType == SIG_STRUCT)
	{
		result = Result(Result::CONTEXT_FP_INDIRECT, returnOffset);
	}
	else
	{
		stackDelta += BOND_SLOT_SIZE;
		result = Result(Result::CONTEXT_STACK_VALUE);
	}
	ApplyStackDelta(stackDelta);

	return result;
}


void GeneratorCore::EmitPushResultAs(const Result &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	switch (result.mContext)
	{
		case Result::CONTEXT_FP_INDIRECT:
			EmitPushFramePointerIndirectValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case Result::CONTEXT_FP_OFFSET:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case Result::CONTEXT_ADDRESS_INDIRECT:
			EmitPushAddressIndirectValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case Result::CONTEXT_CONSTANT_VALUE:
			EmitPushConstantAs(*result.mConstantValue, destType);
			break;

		case Result::CONTEXT_STACK_VALUE:
			EmitPushStackValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case Result::CONTEXT_NATIVE_MEMBER:
		{
			const Result memberResult = EmitCallNativeGetter(result.GetThisPointerResult(), result.mNativeMember);
			EmitPushResultAs(memberResult, sourceType, destType);
		}
		break;

		case Result::CONTEXT_MEMBER_FUNCTION:
		case Result::CONTEXT_NONE:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
}


void GeneratorCore::EmitPushResult(const Result &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case Result::CONTEXT_FP_INDIRECT:
			EmitPushFramePointerIndirectValue(typeDescriptor, result.mOffset);
			break;

		case Result::CONTEXT_FP_OFFSET:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			break;

		case Result::CONTEXT_ADDRESS_INDIRECT:
			EmitPushAddressIndirectValue(typeDescriptor, result.mOffset);
			break;

		case Result::CONTEXT_CONSTANT_VALUE:
			EmitPushConstant(*result.mConstantValue);
			break;

		case Result::CONTEXT_STACK_VALUE:
			EmitPushStackValue(typeDescriptor, result.mOffset);
			break;

		case Result::CONTEXT_NATIVE_MEMBER:
		{
			const Result memberResult = EmitCallNativeGetter(result.GetThisPointerResult(), result.mNativeMember);
			EmitPushResult(memberResult, typeDescriptor);
		}
		break;

		case Result::CONTEXT_MEMBER_FUNCTION:
		case Result::CONTEXT_NONE:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
}


void GeneratorCore::EmitPushFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_UCHAR:
				EmitOpCodeWithOffset(OPCODE_PUSHUC, offset);
				break;
			case Token::KEY_CHAR:
				EmitOpCodeWithOffset(OPCODE_PUSHC, offset);
				break;
			case Token::KEY_SHORT:
				EmitOpCodeWithOffset(OPCODE_PUSHS, offset);
				break;
			case Token::KEY_USHORT:
				EmitOpCodeWithOffset(OPCODE_PUSHUS, offset);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				EmitPushFramePointerIndirectValue32(offset);
				break;
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_DOUBLE:
				EmitPushFramePointerIndirectValue64(offset);
				break;
			default:
				EmitOpCodeWithOffset(OPCODE_LOADFP, offset);
				EmitOpCode(OPCODE_LOADMEMW);
				EmitIndexedValue32(Value32(typeDescriptor->GetStackSize(mPointerSize)));
				break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		if (Is64BitPointer())
		{
			EmitPushFramePointerIndirectValue64(offset);
		}
		else
		{
			EmitPushFramePointerIndirectValue32(offset);
		}
	}
}


void GeneratorCore::EmitPushFramePointerIndirectValue32(bi32_t offset)
{
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH32, offset);
			break;
	}
}


void GeneratorCore::EmitPushFramePointerIndirectValue64(bi32_t offset)
{
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_PUSH64_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH64, offset);
			break;
	}
}


void GeneratorCore::EmitPushAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	// Add the accumulated offset to the address that is already on the stack. Then push the value
	// at the resulting address.
	EmitAccumulateAddressOffset(offset);

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_UCHAR:
				EmitOpCode(OPCODE_LOADUC);
				break;
			case Token::KEY_CHAR:
				EmitOpCode(OPCODE_LOADC);
				break;
			case Token::KEY_SHORT:
				EmitOpCode(OPCODE_LOADS);
				break;
			case Token::KEY_USHORT:
				EmitOpCode(OPCODE_LOADUS);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				EmitOpCode(OPCODE_LOAD32);
				break;
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_DOUBLE:
				EmitOpCode(OPCODE_LOAD64);
				break;
			default:
				EmitOpCode(OPCODE_LOADMEMW);
				EmitIndexedValue32(Value32(typeDescriptor->GetStackSize(mPointerSize)));
			break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		EmitOpCode(LOAD_OPCODES.GetPointerOpCode(mPointerSize));
	}
}


void GeneratorCore::EmitPushStackValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	if (typeDescriptor->IsPointerType())
	{
		EmitAccumulateAddressOffset(offset);
	}
}


void GeneratorCore::EmitPushConstantAs(const TypeAndValue &typeAndValue, const TypeDescriptor *destType)
{
	const TypeDescriptor *sourceType = typeAndValue.GetTypeDescriptor();
	if (sourceType->GetPrimitiveType() != Token::INVALID)
	{
		const Value resultValue = CastValue(typeAndValue, destType);
		TypeDescriptor nonConstDestType = *destType;
		const TypeAndValue resultTav(&nonConstDestType, resultValue);
		EmitPushConstant(resultTav);
	}
	else
	{
		EmitPushConstant(typeAndValue);
		EmitCast(sourceType, destType);
	}
}


void GeneratorCore::EmitPushConstant(const TypeAndValue &typeAndValue)
{
	const TypeDescriptor *typeDescriptor = typeAndValue.GetTypeDescriptor();
	switch (typeDescriptor->GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			EmitOpCode(typeAndValue.GetBoolValue() ? OPCODE_CONSTI_1 : OPCODE_CONSTI_0);
			break;
		case Token::KEY_CHAR:
		case Token::KEY_SHORT:
		case Token::KEY_INT:
			EmitPushConstantInt(typeAndValue.GetIntValue());
			break;
		case Token::KEY_UCHAR:
		case Token::KEY_USHORT:
		case Token::KEY_UINT:
			EmitPushConstantUInt(typeAndValue.GetUIntValue());
			break;
		case Token::KEY_LONG:
			EmitPushConstantLong(typeAndValue.GetLongValue());
			break;
		case Token::KEY_ULONG:
			EmitPushConstantULong(typeAndValue.GetULongValue());
			break;
		case Token::KEY_FLOAT:
			EmitPushConstantFloat(typeAndValue.GetFloatValue());
			break;
		case Token::KEY_DOUBLE:
			EmitPushConstantDouble(typeAndValue.GetDoubleValue());
			break;
		default:
			if (typeDescriptor->IsNullType())
			{
				EmitOpCode(Is64BitPointer() ? OPCODE_CONSTL_0 : OPCODE_CONSTI_0);
			}
			else if (typeDescriptor->IsStringType())
			{
				EmitOpCode(OPCODE_LOADSTR);
				const bu16_t stringIndex = MapString(typeAndValue.GetHashedStringValue());
				EmitValue16(Value16(stringIndex));
			}
			// TODO: Determine if there is anything to do for non-primitive values.
			break;
	}
}


void GeneratorCore::EmitPushConstantInt(bi32_t value)
{
	switch (value)
	{
		case -2:
			EmitOpCode(OPCODE_CONSTI_N2);
			break;
		case -1:
			EmitOpCode(OPCODE_CONSTI_N1);
			break;
		case 0:
			EmitOpCode(OPCODE_CONSTI_0);
			break;
		case 1:
			EmitOpCode(OPCODE_CONSTI_1);
			break;
		case 2:
			EmitOpCode(OPCODE_CONSTI_2);
			break;
		case 3:
			EmitOpCode(OPCODE_CONSTI_3);
			break;
		case 4:
			EmitOpCode(OPCODE_CONSTI_4);
			break;
		case 8:
			EmitOpCode(OPCODE_CONSTI_8);
			break;
		default:
			if (IsInCharRange(value))
			{
				EmitOpCode(OPCODE_CONSTC);
				GetByteCode().push_back(bu8_t(value));
			}
			else if (IsInUCharRange(value))
			{
				EmitOpCode(OPCODE_CONSTUC);
				GetByteCode().push_back(bu8_t(value));
			}
			else if (IsInShortRange(value))
			{
				EmitOpCode(OPCODE_CONSTS);
				EmitValue16(Value16(value));
			}
			else if (IsInUShortRange(value))
			{
				EmitOpCode(OPCODE_CONSTUS);
				EmitValue16(Value16(value));
			}
			else
			{
				EmitOpCode(OPCODE_CONST32);
				EmitIndexedValue32(Value32(value));
			}
			break;
	}
}


void GeneratorCore::EmitPushConstantUInt(bu32_t value)
{
	switch (value)
	{
		case 0:
			EmitOpCode(OPCODE_CONSTI_0);
			break;
		case 1:
			EmitOpCode(OPCODE_CONSTI_1);
			break;
		case 2:
			EmitOpCode(OPCODE_CONSTI_2);
			break;
		case 3:
			EmitOpCode(OPCODE_CONSTI_3);
			break;
		case 4:
			EmitOpCode(OPCODE_CONSTI_4);
			break;
		case 8:
			EmitOpCode(OPCODE_CONSTI_8);
			break;
		default:
			if (IsInUCharRange(value))
			{
				EmitOpCode(OPCODE_CONSTUC);
				GetByteCode().push_back(bu8_t(value));
			}
			else if (IsInUShortRange(value))
			{
				EmitOpCode(OPCODE_CONSTUS);
				EmitValue16(Value16(value));
			}
			else
			{
				EmitOpCode(OPCODE_CONST32);
				EmitIndexedValue32(Value32(value));
			}
			break;
	}
}


void GeneratorCore::EmitPushConstantLong(bi64_t value)
{
	switch (value)
	{
		case -2:
			EmitOpCode(OPCODE_CONSTL_N2);
			break;
		case -1:
			EmitOpCode(OPCODE_CONSTL_N1);
			break;
		case 0:
			EmitOpCode(OPCODE_CONSTL_0);
			break;
		case 1:
			EmitOpCode(OPCODE_CONSTL_1);
			break;
		case 2:
			EmitOpCode(OPCODE_CONSTL_2);
			break;
		case 3:
			EmitOpCode(OPCODE_CONSTL_3);
			break;
		case 4:
			EmitOpCode(OPCODE_CONSTL_4);
			break;
		case 8:
			EmitOpCode(OPCODE_CONSTL_8);
			break;
		default:
		{
			EmitOpCode(OPCODE_CONST64);
			EmitIndexedValue64(Value64(value));
		}
		break;
	}
}


void GeneratorCore::EmitPushConstantULong(bu64_t value)
{
	switch (value)
	{
		case 0:
			EmitOpCode(OPCODE_CONSTL_0);
			break;
		case 1:
			EmitOpCode(OPCODE_CONSTL_1);
			break;
		case 2:
			EmitOpCode(OPCODE_CONSTL_2);
			break;
		case 3:
			EmitOpCode(OPCODE_CONSTL_3);
			break;
		case 4:
			EmitOpCode(OPCODE_CONSTL_4);
			break;
		case 8:
			EmitOpCode(OPCODE_CONSTL_8);
			break;
		default:
		{
			EmitOpCode(OPCODE_CONST64);
			EmitIndexedValue64(Value64(value));
		}
		break;
	}
}


void GeneratorCore::EmitPushConstantFloat(bf32_t value)
{
	if (value == -2.0f)
	{
		EmitOpCode(OPCODE_CONSTF_N2);
	}
	else if (value == -1.0f)
	{
		EmitOpCode(OPCODE_CONSTF_N1);
	}
	else if (value == -0.5f)
	{
		EmitOpCode(OPCODE_CONSTF_NH);
	}
	else if (value == 0.0f)
	{
		EmitOpCode(OPCODE_CONSTF_0);
	}
	else if (value == 0.5f)
	{
		EmitOpCode(OPCODE_CONSTF_H);
	}
	else if (value == 1.0f)
	{
		EmitOpCode(OPCODE_CONSTF_1);
	}
	else if (value == 2.0f)
	{
		EmitOpCode(OPCODE_CONSTF_2);
	}
	else
	{
		EmitOpCode(OPCODE_CONST32);
		EmitIndexedValue32(Value32(value));
	}
}


void GeneratorCore::EmitPushConstantDouble(bf64_t value)
{
	if (value == -2.0)
	{
		EmitOpCode(OPCODE_CONSTD_N2);
	}
	else if (value == -1.0)
	{
		EmitOpCode(OPCODE_CONSTD_N1);
	}
	else if (value == -0.5)
	{
		EmitOpCode(OPCODE_CONSTD_NH);
	}
	else if (value == 0.0)
	{
		EmitOpCode(OPCODE_CONSTD_0);
	}
	else if (value == 0.5)
	{
		EmitOpCode(OPCODE_CONSTD_H);
	}
	else if (value == 1.0)
	{
		EmitOpCode(OPCODE_CONSTD_1);
	}
	else if (value == 2.0)
	{
		EmitOpCode(OPCODE_CONSTD_2);
	}
	else
	{
		EmitOpCode(OPCODE_CONST64);
		EmitIndexedValue64(Value64(value));
	}
}


void GeneratorCore::EmitPopResult(const Result &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case Result::CONTEXT_FP_INDIRECT:
			EmitPopFramePointerIndirectValue(typeDescriptor, result.mOffset);
			break;

		case Result::CONTEXT_ADDRESS_INDIRECT:
			EmitPopAddressIndirectValue(typeDescriptor, result.mOffset);
			break;

		default:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
}


void GeneratorCore::EmitPopFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
			case Token::KEY_UCHAR:
				EmitOpCodeWithOffset(OPCODE_POPC, offset);
				break;
			case Token::KEY_SHORT:
			case Token::KEY_USHORT:
				EmitOpCodeWithOffset(OPCODE_POPS, offset);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				EmitPopFramePointerIndirectValue32(offset);
				break;
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_DOUBLE:
				EmitPopFramePointerIndirectValue64(offset);
				break;
			default:
				break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		if (Is64BitPointer())
		{
			EmitPopFramePointerIndirectValue64(offset);
		}
		else
		{
			EmitPopFramePointerIndirectValue32(offset);
		}
	}
}


void GeneratorCore::EmitPopFramePointerIndirectValue32(bi32_t offset)
{
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP32, offset);
			break;
	}
}


void GeneratorCore::EmitPopFramePointerIndirectValue64(bi32_t offset)
{
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			EmitOpCode(OPCODE_POP64_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP64, offset);
			break;
	}
}


void GeneratorCore::EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	EmitAccumulateAddressOffset(offset);

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
			case Token::KEY_UCHAR:
				EmitOpCode(OPCODE_STOREC);
				break;
			case Token::KEY_SHORT:
			case Token::KEY_USHORT:
				EmitOpCode(OPCODE_STORES);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				EmitOpCode(OPCODE_STORE32);
				break;
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_DOUBLE:
				EmitOpCode(OPCODE_STORE64);
				break;
			default:
				break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		EmitOpCode(STORE_OPCODES.GetPointerOpCode(mPointerSize));
	}
}


GeneratorCore::Result GeneratorCore::EmitAccumulateAddressOffset(const Result &result)
{
	Result output = result;
	if (result.mContext == Result::CONTEXT_ADDRESS_INDIRECT)
	{
		EmitAccumulateAddressOffset(result.mOffset);
		output.mOffset = 0;
	}
	return output;
}


void GeneratorCore::EmitAccumulateAddressOffset(bi32_t offset)
{
	if (offset != 0)
	{
		if (Is64BitPointer())
		{
			EmitPushConstantLong(offset);
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(offset);
			EmitOpCode(OPCODE_ADDI);
		}
	}
}


void GeneratorCore::EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	switch (destType->GetPrimitiveType())
	{
		case Token::KEY_CHAR:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_SHORT:
				case Token::KEY_USHORT:
				case Token::KEY_INT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_ITOC);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					EmitOpCode(OPCODE_ITOC);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOI);
					EmitOpCode(OPCODE_ITOC);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOI);
					EmitOpCode(OPCODE_ITOC);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_BOOL:
		case Token::KEY_UCHAR:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_SHORT:
				case Token::KEY_USHORT:
				case Token::KEY_INT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOUC);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					EmitOpCode(OPCODE_UITOUC);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOUI);
					EmitOpCode(OPCODE_UITOUC);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOUI);
					EmitOpCode(OPCODE_UITOUC);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_SHORT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_INT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_ITOS);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					EmitOpCode(OPCODE_ITOS);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOI);
					EmitOpCode(OPCODE_ITOS);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOI);
					EmitOpCode(OPCODE_ITOS);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_USHORT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_INT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOUS);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					EmitOpCode(OPCODE_UITOUS);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOUI);
					EmitOpCode(OPCODE_UITOUS);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOUI);
					EmitOpCode(OPCODE_UITOUS);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_INT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOI);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOI);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_LTOI);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOUI);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOUI);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_LONG:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					EmitOpCode(OPCODE_ITOL);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOUL);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOL);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOL);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_ULONG:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					EmitOpCode(OPCODE_ITOL);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOUL);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOUL);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOUL);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_FLOAT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					EmitOpCode(OPCODE_ITOF);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOF);
					break;
				case Token::KEY_LONG:
					EmitOpCode(OPCODE_LTOF);
					break;
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_ULTOF);
					break;
				case Token::KEY_DOUBLE:
					EmitOpCode(OPCODE_DTOF);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_DOUBLE:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					EmitOpCode(OPCODE_ITOD);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					EmitOpCode(OPCODE_UITOD);
					break;
				case Token::KEY_LONG:
					EmitOpCode(OPCODE_LTOD);
					break;
				case Token::KEY_ULONG:
					EmitOpCode(OPCODE_ULTOD);
					break;
				case Token::KEY_FLOAT:
					EmitOpCode(OPCODE_FTOD);
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}
}


GeneratorCore::Result GeneratorCore::EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResultAs(lhResult, lhDescriptor, &resultDescriptor);

	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResultAs(rhResult, rhDescriptor, &resultDescriptor);

	EmitOpCode(opCodeSet.GetOpCode(resultDescriptor));

	return Result(Result::CONTEXT_STACK_VALUE);
}


GeneratorCore::Result GeneratorCore::EmitAssignmentOperator(const BinaryExpression *binaryExpression)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	Result result;

	const MemberExpression *memberExpression = NULL;
	const NamedInitializer *namedInitializer = NULL;
	if (((memberExpression = CastNode<MemberExpression>(lhs)) != NULL) &&
	    ((namedInitializer = CastNode<NamedInitializer>(memberExpression->GetDefinition())) != NULL) &&
	    namedInitializer->IsNativeStructMember())
	{
		bi32_t stackDelta = 0;
		{
			UIntStack::Element stackTopElement(mStackTop, mStackTop.GetTop());
			ResultStack::Element rhResult(mResult);
			Traverse(rhs);
			EmitPushResultAs(rhResult, rhDescriptor, lhDescriptor);

			if (mEmitOptionalTemporaries.GetTop() && !lhDescriptor->IsStructType())
			{
				EmitOpCode(OPCODE_DUP);
				stackDelta += GetStackDelta(OPCODE_DUP);
				result.mContext = Result::CONTEXT_STACK_VALUE;
			}

			ResultStack::Element lhResult(mResult);
			Traverse(lhs);
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(lhResult.GetValue().GetThisPointerResult(), &voidStar);

			EmitOpCode(OPCODE_INVOKE);
			EmitHashCode(lhResult.GetValue().mNativeMember->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX));
		}
		ApplyStackDelta(stackDelta);
	}
	else if (lhDescriptor->IsStructType())
	{
		ResultStack::Element lhResult(mResult);
		Traverse(lhs);
		EmitPushAddressOfResult(lhResult);

		if (mEmitOptionalTemporaries.GetTop())
		{
			if (lhResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
			{
				EmitOpCode(OPCODE_DUP);
			}
			result = lhResult;
		}

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushAddressOfResult(rhResult);
		EmitOpCode(OPCODE_MEMCOPYW);
		EmitIndexedValue32(Value32(lhDescriptor->GetSize(mPointerSize)));
	}
	else
	{
		ResultStack::Element lhResult(mResult);
		Traverse(lhs);

		OpCode valueDupOpCode = OPCODE_DUP;
		if (lhResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			lhResult = EmitAccumulateAddressOffset(lhResult);
			valueDupOpCode = OPCODE_DUPINS;
		}

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult, rhDescriptor, lhDescriptor);

		if (mEmitOptionalTemporaries.GetTop())
		{
			EmitOpCode(valueDupOpCode);
			result.mContext = Result::CONTEXT_STACK_VALUE;
		}

		EmitPopResult(lhResult, lhDescriptor);
	}
	return result;
}


GeneratorCore::Result GeneratorCore::EmitCommaOperator(const BinaryExpression *binaryExpression)
{
	TraverseOmitOptionalTemporaries(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
	return mResult.GetTop();
}


void GeneratorCore::EmitLogicalOperator(
	const BinaryExpression *binaryExpression,
	OpCode branchOpCode,
	OpCode negatedOpCode,
	OpCode altBranchOpCode,
	OpCode negatedAltOpCode)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();

	ResultStack::Element lhResult(mResult);
	const bool lhsNegated = TraverseCollapseNotOperators(lhs);
	EmitPushResult(lhResult, lhDescriptor);
	const bu32_t stackTop = mStackTop.GetTop();

	const size_t endLabel = CreateLabel();
	const size_t opCodePos = GetByteCode().size();
	EmitJump(lhsNegated ? negatedOpCode : branchOpCode, endLabel);

	if (mCollapseNotOperators.GetTop())
	{
		ResultStack::Element rhResult(mResult);
		const bool rhsNegated = TraverseCollapseNotOperators(rhs);
		EmitPushResult(rhResult, rhDescriptor);

		// Apply De Morgan's laws if we can reduce the number of ! operators.
		if (rhsNegated)
		{
			// Patch up the opcode.
			GetByteCode()[opCodePos] = lhsNegated ? altBranchOpCode : negatedAltOpCode;
			mExpressionIsNegated.SetTop(true);
		}
	}
	else
	{
		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResult(rhResult, rhDescriptor);
	}

	SetLabelValue(endLabel, GetByteCode().size());

	if (stackTop != mStackTop.GetTop())
	{
		PushError(CompilerError::INTERNAL_ERROR);
	}
}


GeneratorCore::Result GeneratorCore::EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);
	const TypeAndValue &rhTav = rhs->GetTypeAndValue();
	const bi64_t rhValue = rhTav.AsLongValue() * ((&opCodeSet == &SUB_OPCODES) ? -1 : 1);
	bu32_t stackTop = mStackTop.GetTop();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	const bi32_t frameOffset = lhResult.GetValue().mOffset;
	const bi32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if (((&opCodeSet == &ADD_OPCODES) || (&opCodeSet == &SUB_OPCODES)) &&
	    (lhResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    lhDescriptor->IsLeast32IntegerType() &&
	    rhTav.IsValueDefined() &&
	    IsInCharRange(rhValue))
	{
		EmitOpCode(INC_OPCODES.GetOpCode(*lhDescriptor));
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(bu8_t(slotIndex));
		byteCode.push_back(bu8_t(rhValue));

		if (mEmitOptionalTemporaries.GetTop())
		{
			EmitPushResult(lhResult, lhDescriptor);
		}
	}
	else if (lhResult.GetValue().mContext == Result::CONTEXT_NATIVE_MEMBER)
	{
		{
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(lhResult.GetValue().GetThisPointerResult(), &voidStar);
			EmitOpCode(OPCODE_DUP);
		}

		EmitCallNativeGetter(Result(Result::CONTEXT_STACK_VALUE), lhResult.GetValue().mNativeMember);
		EmitCast(lhDescriptor, &resultDescriptor);

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult, rhDescriptor, &resultDescriptor);

		EmitOpCode(opCodeSet.GetOpCode(resultDescriptor));
		EmitCast(&resultDescriptor, lhDescriptor);

		if (mEmitOptionalTemporaries.GetTop())
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitHashCode(lhResult.GetValue().mNativeMember->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX));
		mStackTop.SetTop(stackTop);
	}
	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (lhResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			lhResult = EmitAccumulateAddressOffset(lhResult);
			EmitOpCode(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResultAs(lhResult, lhDescriptor, &resultDescriptor);

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult, rhDescriptor, &resultDescriptor);

		EmitOpCode(opCodeSet.GetOpCode(resultDescriptor));
		EmitCast(&resultDescriptor, lhDescriptor);

		if (mEmitOptionalTemporaries.GetTop())
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(lhResult, lhDescriptor);
	}

	return Result(mEmitOptionalTemporaries.GetTop() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitPointerCompoundAssignmentOperator(const Expression *pointerExpression, const Expression *offsetExpression, int sign)
{
	const TypeDescriptor *pointerDescriptor = pointerExpression->GetTypeDescriptor();
	const bi32_t elementSize = bi32_t(sign * pointerDescriptor->GetDereferencedType().GetSize(mPointerSize));
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	const bi64_t offset = offsetTav.AsLongValue() * elementSize;
	bu32_t stackTop = mStackTop.GetTop();

	ResultStack::Element pointerResult(mResult);
	Traverse(pointerExpression);
	const bi32_t frameOffset = pointerResult.GetValue().mOffset;
	const bi32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((pointerResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    offsetTav.IsValueDefined() &&
	    IsInCharRange(offset))
	{
		EmitOpCode(INC_OPCODES.GetPointerOpCode(mPointerSize));
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(bu8_t(slotIndex));
		byteCode.push_back(bu8_t(offset));

		if (mEmitOptionalTemporaries.GetTop())
		{
			if (Is64BitPointer())
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}
			else
			{
				EmitPushFramePointerIndirectValue32(frameOffset);
			}
		}
	}
	else if (pointerResult.GetValue().mContext == Result::CONTEXT_NATIVE_MEMBER)
	{
		{
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(pointerResult.GetValue().GetThisPointerResult(), &voidStar);
			EmitOpCode(OPCODE_DUP);
		}

		EmitCallNativeGetter(Result(Result::CONTEXT_STACK_VALUE), pointerResult.GetValue().mNativeMember);
		EmitPointerOffset(offsetExpression, elementSize);

		if (mEmitOptionalTemporaries.GetTop())
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitHashCode(pointerResult.GetValue().mNativeMember->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX));
		mStackTop.SetTop(stackTop);
	}
	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (pointerResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			pointerResult = EmitAccumulateAddressOffset(pointerResult);
			EmitOpCode(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResult(pointerResult, pointerDescriptor);
		EmitPointerOffset(offsetExpression, elementSize);

		if (mEmitOptionalTemporaries.GetTop())
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(pointerResult, pointerDescriptor);
	}

	return Result(mEmitOptionalTemporaries.GetTop() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitPointerArithmetic(const Expression *pointerExpression, const Expression *offsetExpression, int sign)
{
	const TypeDescriptor *pointerDescriptor = pointerExpression->GetTypeDescriptor();
	const bi32_t elementSize = bi32_t(sign * pointerDescriptor->GetDereferencedType().GetSize(mPointerSize));
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	const bi64_t offset = offsetTav.AsLongValue() * elementSize;

	ResultStack::Element pointerResult(mResult);
	Traverse(pointerExpression);

	Result result(Result::CONTEXT_STACK_VALUE);

	if (offsetTav.IsValueDefined() && IsInIntRange(offset))
	{
		switch (pointerResult.GetValue().mContext)
		{
			case Result::CONTEXT_ADDRESS_INDIRECT:
			case Result::CONTEXT_FP_INDIRECT:
			case Result::CONTEXT_CONSTANT_VALUE:
			case Result::CONTEXT_NATIVE_MEMBER:
				EmitPushResult(pointerResult, pointerDescriptor);
				break;
			default:
				result = pointerResult;
		}
		result.mOffset += bi32_t(offset);
	}
	else
	{
		EmitPushResult(pointerResult, pointerDescriptor);
		EmitPointerOffset(offsetExpression, elementSize);
	}

	return result;
}


GeneratorCore::Result GeneratorCore::EmitPointerComparison(const Expression *lhs, const Expression *rhs, const OpCodeSet &opCodeSet)
{
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult, lhDescriptor);

	const TypeDescriptor *rhDescriptor = lhs->GetTypeDescriptor();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResult(rhResult, rhDescriptor);

	EmitOpCode(opCodeSet.GetPointerOpCode(mPointerSize));

	return Result(Result::CONTEXT_STACK_VALUE);
}


GeneratorCore::Result GeneratorCore::EmitPointerDifference(const Expression *lhs, const Expression *rhs)
{
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult, lhDescriptor);

	const TypeDescriptor *rhDescriptor = lhs->GetTypeDescriptor();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResult(rhResult, rhDescriptor);

	const bu32_t elementSize = lhDescriptor->GetDereferencedType().GetSize(mPointerSize);
	if (IsInShortRange(elementSize))
	{
		EmitOpCode(OPCODE_PTRDIFF);
		EmitValue16(Value16(elementSize));
	}
	else if (Is64BitPointer())
	{
		EmitOpCode(OPCODE_SUBL);
		EmitPushConstantLong(elementSize);
		EmitOpCode(OPCODE_DIVL);
	}
	else
	{
		EmitOpCode(OPCODE_SUBI);
		EmitPushConstantInt(elementSize);
		EmitOpCode(OPCODE_DIVI);
	}

	return Result(Result::CONTEXT_STACK_VALUE);
}


void GeneratorCore::EmitPointerOffset(const Expression *offsetExpression, bi32_t elementSize)
{
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	if (offsetTav.IsValueDefined())
	{
		if (Is64BitPointer())
		{
			const bi64_t offset = offsetTav.AsLongValue() * elementSize;
			EmitPushConstantLong(offset);
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			const bi32_t offset = offsetTav.GetIntValue() * elementSize;
			EmitPushConstantInt(offset);
			EmitOpCode(OPCODE_ADDI);
		}
	}
	else
	{
		ResultStack::Element offsetResult(mResult);
		Traverse(offsetExpression);

		const TypeDescriptor *offsetDescriptor = offsetTav.GetTypeDescriptor();
		if (IsInShortRange(elementSize))
		{
			const TypeDescriptor intTypeDescriptor = TypeDescriptor::GetIntType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &intTypeDescriptor);
			EmitOpCode(OPCODE_PTROFF);
			EmitValue16(Value16(elementSize));
		}
		else if (Is64BitPointer())
		{
			const TypeDescriptor longTypeDescriptor = TypeDescriptor::GetLongType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &longTypeDescriptor);
			EmitPushConstantLong(bu64_t(elementSize));
			EmitOpCode(OPCODE_MULL);
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			const TypeDescriptor intTypeDescriptor = TypeDescriptor::GetIntType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &intTypeDescriptor);
			EmitPushConstantInt(elementSize);
			EmitOpCode(OPCODE_MULI);
			EmitOpCode(OPCODE_ADDI);
		}
	}
}


GeneratorCore::Result GeneratorCore::EmitPointerIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign)
{
	const TypeDescriptor *operandDescriptor = operand->GetTypeDescriptor();
	const bi32_t pointerOffset = bi32_t(sign * operandDescriptor->GetDereferencedType().GetSize(mPointerSize)) * sign;
	bu32_t stackTop = mStackTop.GetTop();

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const bi32_t frameOffset = operandResult.GetValue().mOffset;
	const bi32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    IsInCharRange(pointerOffset))
	{
		ByteCode::Type &byteCode = GetByteCode();
		if (Is64BitPointer())
		{
			if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}

			EmitOpCode(OPCODE_INCL);
			byteCode.push_back(bu8_t(slotIndex));
			byteCode.push_back(bu8_t(pointerOffset));

			if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}
		}
		else
		{
			if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
			{
				EmitPushFramePointerIndirectValue32(frameOffset);
			}

			EmitOpCode(OPCODE_INCI);
			byteCode.push_back(bu8_t(slotIndex));
			byteCode.push_back(bu8_t(pointerOffset));

			if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
			{
				EmitPushFramePointerIndirectValue32(frameOffset);
			}
		}
	}
	else if (operandResult.GetValue().mContext == Result::CONTEXT_NATIVE_MEMBER)
	{
		{
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(operandResult.GetValue().GetThisPointerResult(), &voidStar);
			EmitOpCode(OPCODE_DUP);
		}

		EmitCallNativeGetter(Result(Result::CONTEXT_STACK_VALUE), operandResult.GetValue().mNativeMember);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		if (Is64BitPointer())
		{
			EmitPushConstantLong(bi64_t(pointerOffset));
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(pointerOffset);
			EmitOpCode(OPCODE_ADDI);
		}

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitHashCode(operandResult.GetValue().mNativeMember->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX));
		mStackTop.SetTop(stackTop);
	}
	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (operandResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			operandResult = EmitAccumulateAddressOffset(operandResult);
			EmitOpCode(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResult(operandResult, operandDescriptor);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		if (Is64BitPointer())
		{
			EmitPushConstantLong(bi64_t(pointerOffset));
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(pointerOffset);
			EmitOpCode(OPCODE_ADDI);
		}

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(operandResult, operandDescriptor);
	}

	return Result(mEmitOptionalTemporaries.GetTop() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign)
{
	const TypeDescriptor *operandDescriptor = operand->GetTypeDescriptor();
	const TypeDescriptor *resultDescriptor = expression->GetTypeDescriptor();
	const Token::TokenType operandType = operandDescriptor->GetPrimitiveType();
	const Token::TokenType resultType = resultDescriptor->GetPrimitiveType();
	const OpCodeSet &constOpCodeSet = (sign > 0) ? CONST1_OPCODES : CONSTN1_OPCODES;
	bu32_t stackTop = mStackTop.GetTop();

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const bi32_t frameOffset = operandResult.GetValue().mOffset;
	const bi32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    resultDescriptor->IsLeast32IntegerType() &&
	    (operandType == resultType))
	{
		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			EmitPushResult(operandResult, operandDescriptor);
		}

		EmitOpCode(INC_OPCODES.GetOpCode(resultType));
		GetByteCode().push_back(bu8_t(slotIndex));
		GetByteCode().push_back(bu8_t(sign));

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			EmitPushResult(operandResult, operandDescriptor);
		}
	}
	else if (operandResult.GetValue().mContext == Result::CONTEXT_NATIVE_MEMBER)
	{
		{
			const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
			EmitPushResult(operandResult.GetValue().GetThisPointerResult(), &voidStar);
			EmitOpCode(OPCODE_DUP);
		}

		EmitCallNativeGetter(Result(Result::CONTEXT_STACK_VALUE), operandResult.GetValue().mNativeMember);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(constOpCodeSet.GetOpCode(resultType));
		EmitOpCode(ADD_OPCODES.GetOpCode(resultType));

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitHashCode(operandResult.GetValue().mNativeMember->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX));
		mStackTop.SetTop(stackTop);
	}
	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (operandResult.GetValue().mContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			operandResult = EmitAccumulateAddressOffset(operandResult);
			EmitOpCode(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResultAs(operandResult, operandDescriptor, resultDescriptor);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitOpCode(constOpCodeSet.GetOpCode(resultType));
		EmitOpCode(ADD_OPCODES.GetOpCode(resultType));

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(operandResult, operandDescriptor);
	}

	return Result(mEmitOptionalTemporaries.GetTop() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitSignOperator(const UnaryExpression *unaryExpression)
{
	const UnaryExpression *unary = unaryExpression;
	const Expression *rhs = unaryExpression;
	const TypeDescriptor *resultDescriptor = unaryExpression->GetTypeDescriptor();
	bool negated = false;
	while (unary != NULL)
	{
		const Token::TokenType op = unary->GetOperator()->GetTokenType();
		if (op == Token::OP_MINUS)
		{
			negated = !negated;
		}
		else if (op != Token::OP_PLUS)
		{
			break;
		}
		rhs = unary->GetRhs();
		unary = CastNode<UnaryExpression>(rhs);
	}

	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResult(rhResult, rhDescriptor);

	if (negated)
	{
		EmitOpCode(NEG_OPCODES.GetOpCode(*resultDescriptor));
	}

	return Result(Result::CONTEXT_STACK_VALUE);
}


GeneratorCore::Result GeneratorCore::EmitNotOperator(const UnaryExpression *unaryExpression)
{
	const Expression *rhs = unaryExpression->GetRhs();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	ResultStack::Element rhResult(mResult);
	const bool negated = TraverseCollapseNotOperators(rhs);
	EmitPushResult(rhResult, rhDescriptor);

	if (mCollapseNotOperators.GetTop())
	{
		mExpressionIsNegated.SetTop(!negated);
	}
	else if (!negated)
	{
		EmitOpCode(OPCODE_NOT);
	}

	return Result(Result::CONTEXT_STACK_VALUE);
}


GeneratorCore::Result GeneratorCore::EmitBitwiseNotOperator(const UnaryExpression *unaryExpression)
{
	const UnaryExpression *unary = unaryExpression;
	const Expression *rhs = unaryExpression;
	const TypeDescriptor *resultDescriptor = unaryExpression->GetTypeDescriptor();
	bool negated = false;
	while (unary != NULL)
	{
		const Token::TokenType op = unary->GetOperator()->GetTokenType();
		if (op == Token::OP_BIT_NOT)
		{
			negated = !negated;
		}
		else
		{
			break;
		}
		rhs = unary->GetRhs();
		unary = CastNode<UnaryExpression>(rhs);
	}

	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResult(rhResult, rhDescriptor);

	if (negated)
	{
		if (resultDescriptor->GetPrimitiveType() == Token::KEY_LONG)
		{
			EmitOpCode(OPCODE_CONSTL_1);
			EmitOpCode(OPCODE_XORL);
		}
		else
		{
			EmitOpCode(OPCODE_CONSTI_1);
			EmitOpCode(OPCODE_XORI);
		}
	}

	return Result(Result::CONTEXT_STACK_VALUE);
}


GeneratorCore::Result GeneratorCore::EmitAddressOfOperator(const UnaryExpression *unaryExpression)
{
	// TODO: Non-literal constants must not be resolved to constants. Must be FP indirect, for example.
	const Expression *rhs = unaryExpression->GetRhs();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	Result result = EmitAddressOfResult(rhResult);
	return result;
}


void GeneratorCore::EmitPushAddressOfResult(const Result &result)
{
	const Result destinationResult = EmitAddressOfResult(result);
	const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
	EmitPushResult(destinationResult, &voidStar);
}


GeneratorCore::Result GeneratorCore::EmitAddressOfResult(const Result &result)
{
	Result outputResult = result;
	switch (result.mContext)
	{
		case Result::CONTEXT_FP_INDIRECT:
			outputResult.mContext = Result::CONTEXT_FP_OFFSET;
			break;

		case Result::CONTEXT_ADDRESS_INDIRECT:
			outputResult.mContext = Result::CONTEXT_STACK_VALUE;
			break;

		default:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
	return outputResult;
}


GeneratorCore::Result GeneratorCore::EmitDereferenceOperator(const UnaryExpression *unaryExpression)
{
	// TODO: Non-literal constants must not be resolved to constants. Must be FP indirect, for example.
	const Expression *rhs = unaryExpression->GetRhs();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	Result result = rhResult;
	if (!unaryExpression->GetTypeDescriptor()->IsArrayType())
	{
		result = EmitDereferenceResult(rhResult, rhs->GetTypeDescriptor());
	}
	return result;
}


GeneratorCore::Result GeneratorCore::EmitDereferenceResult(const Result &result, const TypeDescriptor *typeDescriptor)
{
	Result outputResult = result;
	switch (result.mContext)
	{
		case Result::CONTEXT_FP_OFFSET:
			outputResult.mContext = Result::CONTEXT_FP_INDIRECT;
			break;

		case Result::CONTEXT_STACK_VALUE:
			outputResult.mContext = Result::CONTEXT_ADDRESS_INDIRECT;
			break;

		case Result::CONTEXT_FP_INDIRECT:
		case Result::CONTEXT_ADDRESS_INDIRECT:
			EmitPushResult(result, typeDescriptor);
			outputResult = Result(Result::CONTEXT_ADDRESS_INDIRECT);
			break;

		default:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
	return outputResult;
}


void GeneratorCore::EmitArgumentList(const Expression *argList, const Parameter *paramList)
{
	if (argList != NULL)
	{
		EmitArgumentList(NextNode(argList), NextNode(paramList));

		const TypeDescriptor *argDescriptor = argList->GetTypeDescriptor();
		const TypeDescriptor *paramDescriptor = paramList->GetTypeDescriptor();
		ResultStack::Element argResult(mResult);
		Traverse(argList);
		EmitPushResultAs(argResult, argDescriptor, paramDescriptor);
	}
}


void GeneratorCore::EmitJump(OpCode opCode, size_t toLabel)
{
	ByteCode::Type &byteCode = GetByteCode();
	const size_t opCodePos = byteCode.size();
	EmitOpCode(opCode);
	byteCode.push_back(0);
	byteCode.push_back(0);
	GetFunction().mJumpList.push_back(JumpEntry(opCodePos, toLabel));
}


void GeneratorCore::EmitOpCodeWithOffset(OpCode opCode, bi32_t offset)
{
	if (IsInShortRange(offset))
	{
		EmitOpCode(opCode);
		EmitValue16(Value16(offset));
	}
	else
	{
		EmitOpCode(static_cast<OpCode>(opCode + 1));
		EmitIndexedValue32(Value32(offset));
	}
}


void GeneratorCore::EmitOpCode(OpCode opCode)
{
	GetByteCode().push_back(opCode);
	ApplyStackDelta(GetStackDelta(opCode));
}


void GeneratorCore::EmitValue16(Value16 value)
{
	ConvertBigEndian16(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
}


void GeneratorCore::EmitValue32At(Value32 value, size_t pos)
{
	ConvertBigEndian32(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode[pos + 0] = value.mBytes[0];
	byteCode[pos + 1] = value.mBytes[1];
	byteCode[pos + 2] = value.mBytes[2];
	byteCode[pos + 3] = value.mBytes[3];
}

void GeneratorCore::EmitIndexedValue32(Value32 value)
{
	const bu16_t index = MapValue32(value);
	EmitValue16(Value16(index));
}


void GeneratorCore::EmitIndexedValue64(Value64 value)
{
	const bu16_t index = MapValue64(value);
	EmitValue16(Value16(index));
}


void GeneratorCore::EmitHashCode(bu32_t hash)
{
	EmitIndexedValue32(Value32(hash));
}


GeneratorCore::Result::Context GeneratorCore::TransformContext(Result::Context targetContext, const TypeDescriptor *typeDescriptor) const
{
	// Handles special cases for the outcome of the evaluation of an expression.
	// Normally an identifier in code refers to the value stored at the location represented
	// by the identifier, with the exception of arrays that evaluate to an address.
	Result::Context context = targetContext;
	if (typeDescriptor->IsArrayType())
	{
		if (targetContext == Result::CONTEXT_FP_INDIRECT)
		{
			context = Result::CONTEXT_FP_OFFSET;
		}
		else if (targetContext == Result::CONTEXT_ADDRESS_INDIRECT)
		{
			context = Result::CONTEXT_STACK_VALUE;
		}
	}
	return context;
}


void GeneratorCore::ResolveJumps()
{
	for (FunctionList::Type::iterator functionIt = mFunctionList.begin(); functionIt != mFunctionList.end(); ++functionIt)
	{
		ByteCode::Type &byteCode = functionIt->mByteCode;
		JumpList::Type &jumpList = functionIt->mJumpList;
		const LabelList::Type &labelList = functionIt->mLabelList;

		// Resolve jump targets.
		for (JumpList::Type::iterator jumpIt = jumpList.begin(); jumpIt != jumpList.end(); ++jumpIt)
		{
			jumpIt->mToPos = labelList[jumpIt->mToLabel];
		}

		// Optmize jumps if the target is another jump instruction.
		bool requiresAnotherPass = true;
		while (requiresAnotherPass)
		{
			requiresAnotherPass = false;
			for (JumpList::Type::iterator jumpIt = jumpList.begin(); jumpIt != jumpList.end(); ++jumpIt)
			{
				const JumpEntry *targetEntry = FindJumpEntry(jumpList, jumpIt->mToPos);
				if (targetEntry != NULL)
				{
					const bu8_t opCode = byteCode[jumpIt->mOpCodePos];
					const bu8_t targetOpCode = byteCode[targetEntry->mOpCodePos];
					if (targetOpCode == OPCODE_GOTO)
					{
						jumpIt->mToPos = targetEntry->mToPos;
						requiresAnotherPass = true;
					}
					else
					{
						switch (opCode)
						{
							case OPCODE_BRZ:
								switch (targetOpCode)
								{
									case OPCODE_BRZ:
										jumpIt->mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_NBRNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRNZ:
									case OPCODE_NBRZ:
									case OPCODE_IFNZ:
										jumpIt->mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_BRNZ:
								switch (targetOpCode)
								{
									case OPCODE_BRNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_NBRZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRZ:
									case OPCODE_NBRNZ:
									case OPCODE_IFZ:
										jumpIt->mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_NBRZ:
								switch (targetOpCode)
								{
									case OPCODE_BRZ:
										jumpIt->mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_BRNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRNZ:
									case OPCODE_NBRZ:
									case OPCODE_IFNZ:
										jumpIt->mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_NBRNZ:
								switch (targetOpCode)
								{
									case OPCODE_BRNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_BRZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFNZ:
										jumpIt->mToPos = targetEntry->mToPos;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRZ:
									case OPCODE_NBRNZ:
									case OPCODE_IFZ:
										jumpIt->mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpIt->mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
								}
								break;
						}
					}
				}
			}
		}

		// Add jump offsets to the constant table if necessary.
		for (JumpList::Type::const_iterator jumpIt = jumpList.begin(); jumpIt != jumpList.end(); ++jumpIt)
		{
			const bi32_t offset = bi32_t(jumpIt->mToPos - jumpIt->GetFromPos());
			if (!IsInShortRange(offset))
			{
				MapValue32(Value32(offset));
			}
		}
	}
}


const GeneratorCore::JumpEntry *GeneratorCore::FindJumpEntry(const JumpList::Type &jumpList, size_t opCodePos) const
{
	JumpList::Type::const_iterator it = lower_bound(jumpList.begin(), jumpList.end(), opCodePos, JumpEntryOpCodePosComparator());
	return ((it < jumpList.end()) && (it->mOpCodePos == opCodePos)) ? &(*it) : static_cast<const JumpEntry *>(NULL);
}


void GeneratorCore::WriteConstantTable()
{
	const int startPos = mWriter.GetPosition();

	// Skip the 4 bytes for the table size.
	mWriter.AddOffset(sizeof(Value32));

	WriteValue16(Value16(bu16_t(mValue32List.size())));
	WriteValue16(Value16(bu16_t(mValue64List.size())));
	WriteValue16(Value16(bu16_t(mStringList.size())));

	for (Value32List::Type::const_iterator it = mValue32List.begin(); it != mValue32List.end(); ++it)
	{
		WriteValue32(*it);
	}

	for (Value64List::Type::const_iterator it = mValue64List.begin(); it != mValue64List.end(); ++it)
	{
		WriteValue64(*it);
	}

	for (StringList::Type::const_iterator it = mStringList.begin(); it != mStringList.end(); ++it)
	{
		const size_t length = it->GetLength();
		const char *str = it->GetString();
		WriteValue16(Value16(bu16_t(length)));
		for (size_t i = 0; i < length; ++i)
		{
			mWriter.Write(str[i]);
		}
	}

	// Patch up the table size.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue32(Value32(endPos - startPos));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::WriteFunctionList(bu16_t functionIndex)
{
	for (FunctionList::Type::const_iterator functionIt = mFunctionList.begin(); functionIt != mFunctionList.end(); ++functionIt)
	{
		const FunctionDefinition *function = functionIt->mDefinition;

		// Cache the blob start position and skip 4 bytes for the blob size.
		const int blobStartPos = mWriter.GetPosition();
		mWriter.AddOffset(sizeof(Value32));

		WriteValue16(Value16(functionIndex));
		WriteReturnSignature(function->GetPrototype()->GetReturnType());
		WriteQualifiedSymbolName(function);
		WriteParamListSignature(function->GetPrototype()->GetParameterList(), function->GetScope() == SCOPE_STRUCT_MEMBER);
		WriteValue32(Value32(function->GetGlobalHashCode()));
		WriteValue32(Value32(functionIt->mArgSize));
		WriteValue32(Value32(functionIt->mPackedArgSize));
		WriteValue32(Value32(functionIt->mLocalSize));
		WriteValue32(Value32(functionIt->mStackSize));
		WriteValue32(Value32(functionIt->mFramePointerAlignment));

		// Cache the code start position and skip 4 bytes for the code size.
		const int codeSizePos = mWriter.GetPosition();
		mWriter.AddOffset(sizeof(Value32));
		const int codeStartPos = mWriter.GetPosition();

		const ByteCode::Type &byteCode = functionIt->mByteCode;
		const JumpList::Type &jumpList = functionIt->mJumpList;

		size_t byteCodeIndex = 0;
		for (JumpList::Type::const_iterator jumpIt = jumpList.begin(); jumpIt != jumpList.end(); ++jumpIt)
		{
			while (byteCodeIndex < jumpIt->mOpCodePos)
			{
				mWriter.Write(byteCode[byteCodeIndex++]);
			}

			bu8_t opCode = byteCode[byteCodeIndex++];
			const bi32_t offset = bi32_t(jumpIt->mToPos) - bi32_t(jumpIt->GetFromPos());
			Value16 arg(offset);

			if (!IsInShortRange(offset))
			{
				++opCode;
				arg.mUShort = MapValue32(Value32(offset));
			}

			mWriter.Write(opCode);
			WriteValue16(arg);
			byteCodeIndex += 2;
		}

		while (byteCodeIndex < byteCode.size())
		{
			mWriter.Write(byteCode[byteCodeIndex++]);
		}

		// Patch up the code size.
		const int endPos = mWriter.GetPosition();
		mWriter.SetPosition(codeSizePos);
		WriteValue32(Value32(endPos - codeStartPos));

		// Patch up the blob size.
		mWriter.SetPosition(blobStartPos);
		WriteValue32(Value32(endPos - blobStartPos));
		mWriter.SetPosition(endPos);
	}
}


void GeneratorCore::WriteNativeMemberList(bu16_t functionIndex)
{
	for (NamedInitializerList::Type::const_iterator memberIt = mNativeMemberList.begin(); memberIt != mNativeMemberList.end(); ++memberIt)
	{
		const NamedInitializer *member = *memberIt;
		const TypeDescriptor *typeDescriptor = member->GetTypeAndValue()->GetTypeDescriptor();

		// Write the getter function.
		{
			// Cache the blob start position and skip 4 bytes for the blob size.
			const int blobStartPos = mWriter.GetPosition();
			mWriter.AddOffset(sizeof(Value32));
			WriteValue16(Value16(functionIndex));
			WriteReturnSignature(typeDescriptor);
			WriteQualifiedSymbolName(member);
			WriteParamListSignature(NULL, true);
			WriteValue32(Value32(member->GetGlobalHashCodeWithSuffix(BOND_NATIVE_GETTER_SUFFIX)));
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Frame size
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Packed frame size
			WriteValue32(Value32(0));              // Local size
			WriteValue32(Value32(0));              // Stack size
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Frame pointer alignment
			WriteValue32(Value32(0));              // Code size

			// Patch up the blob size.
			const int endPos = mWriter.GetPosition();
			mWriter.SetPosition(blobStartPos);
			WriteValue32(Value32(endPos - blobStartPos));
			mWriter.SetPosition(endPos);
		}

		// Write the setter function.
		{
			const bu32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), bu32_t(BOND_SLOT_SIZE));
			const bu32_t offset = AlignUp(bu32_t(BOND_SLOT_SIZE) + typeDescriptor->GetSize(mPointerSize), alignment);
			const bu32_t packedOffset = bu32_t(BOND_SLOT_SIZE) + typeDescriptor->GetStackSize(mPointerSize);
			const TypeDescriptor voidDescriptor = TypeDescriptor::GetVoidType();
			const Parameter parameter(typeDescriptor, -bi32_t(offset));

			// Cache the blob start position and skip 4 bytes for the blob size.
			const int blobStartPos = mWriter.GetPosition();
			mWriter.AddOffset(sizeof(Value32));
			WriteValue16(Value16(functionIndex));
			WriteReturnSignature(&voidDescriptor);
			WriteQualifiedSymbolName(member);
			WriteParamListSignature(&parameter, true);
			WriteValue32(Value32(member->GetGlobalHashCodeWithSuffix(BOND_NATIVE_SETTER_SUFFIX)));
			WriteValue32(Value32(offset));         // Frame size
			WriteValue32(Value32(packedOffset));   // Packed frame size
			WriteValue32(Value32(0));              // Local size
			WriteValue32(Value32(0));              // Stack size
			WriteValue32(Value32(alignment));      // Frame pointer alignment
			WriteValue32(Value32(0));              // Code size

			// Patch up the blob size.
			const int endPos = mWriter.GetPosition();
			mWriter.SetPosition(blobStartPos);
			WriteValue32(Value32(endPos - blobStartPos));
			mWriter.SetPosition(endPos);
		}
	}
}


void GeneratorCore::WriteQualifiedSymbolName(const Symbol *symbol)
{
	// Cache the position for the number of elements and skip 2 bytes.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value16));

	WriteSymbolNameIndices(symbol);

	// Patch up the number of elements.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue16(Value16(bu16_t((endPos - startPos - sizeof(Value16)) / sizeof(Value16))));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::WriteReturnSignature(const TypeDescriptor *type)
{
	const bu32_t sizeAndType = EncodeSizeAndType(type->GetSize(mPointerSize), type->GetSignatureType());
	WriteValue32(Value32(sizeAndType));
}


void GeneratorCore::WriteParamListSignature(const Parameter *parameterList, bool includeThis)
{
	// Cache the position for the number of parameters and skip 2 bytes.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value16));

	if (includeThis)
	{
		const bu32_t sizeAndType = EncodeSizeAndType(GetPointerSize(mPointerSize), SIG_POINTER);
		WriteValue32(Value32(-BOND_SLOT_SIZE));
		WriteValue32(Value32(sizeAndType));
	}

	while (parameterList != NULL)
	{
		const TypeDescriptor *type = parameterList->GetTypeDescriptor();
		const bi32_t offset = parameterList->GetOffset();
		const bu32_t sizeAndType = EncodeSizeAndType(type->GetSize(mPointerSize), type->GetSignatureType());
		WriteValue32(Value32(offset));
		WriteValue32(Value32(sizeAndType));
		parameterList = NextNode(parameterList);
	}

	// Patch up the number of parameters.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue16(Value16(bu16_t((endPos - startPos - sizeof(Value16)) / (2 * sizeof(Value32)))));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::WriteSymbolNameIndices(const Symbol *symbol)
{
	if (symbol != NULL)
	{
		WriteSymbolNameIndices(symbol->GetParentSymbol());
		const Token *name = symbol->GetName();
		if (name != NULL)
		{
			const bu16_t nameIndex = MapString(name->GetHashedText());
			WriteValue16(Value16(nameIndex));
		}
	}
}


void GeneratorCore::WriteValue16(Value16 value)
{
	ConvertBigEndian16(value.mBytes);
	mWriter.Write(value.mBytes[0]);
	mWriter.Write(value.mBytes[1]);
}


void GeneratorCore::WriteValue32(Value32 value)
{
	ConvertBigEndian32(value.mBytes);
	mWriter.Write(value.mBytes[0]);
	mWriter.Write(value.mBytes[1]);
	mWriter.Write(value.mBytes[2]);
	mWriter.Write(value.mBytes[3]);
}


void GeneratorCore::WriteValue64(Value64 value)
{
	ConvertBigEndian64(value.mBytes);
	mWriter.Write(value.mBytes[0]);
	mWriter.Write(value.mBytes[1]);
	mWriter.Write(value.mBytes[2]);
	mWriter.Write(value.mBytes[3]);
	mWriter.Write(value.mBytes[4]);
	mWriter.Write(value.mBytes[5]);
	mWriter.Write(value.mBytes[6]);
	mWriter.Write(value.mBytes[7]);
}


void GeneratorCore::ApplyStackDelta(bi32_t delta)
{
	CompiledFunction &function = GetFunction();
	bu32_t top = mStackTop.GetTop();

	// Check for stack underflow.
	if ((delta < 0) && (bu32_t(-delta) > top))
	{
		PushError(CompilerError::INTERNAL_ERROR);
	}

	const bu32_t stackTop = mStackTop.GetTop() + delta;
	mStackTop.SetTop(stackTop);
	function.mStackSize = Max(function.mStackSize, stackTop);
}


bi32_t GeneratorCore::AllocateLocal(const TypeDescriptor* typeDescriptor)
{
	CompiledFunction &function = GetFunction();
	const bu32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), bu32_t(BOND_SLOT_SIZE));
	const bu32_t size = typeDescriptor->GetSize(mPointerSize);
	const bu32_t offset = AlignUp(mLocalOffset.GetTop(), alignment);
	const bu32_t nextOffset = AlignUp(offset + size, bu32_t(BOND_SLOT_SIZE));
	mLocalOffset.SetTop(nextOffset);
	function.mLocalSize = Max(function.mLocalSize, nextOffset);
	function.mFramePointerAlignment = Max(function.mFramePointerAlignment, alignment);
	return bi32_t(offset);
}


size_t GeneratorCore::CreateLabel()
{
	LabelList::Type &labelList = GetFunction().mLabelList;
	size_t label = labelList.size();
	labelList.push_back(0);
	return label;
}


void GeneratorCore::SetLabelValue(size_t label, size_t value)
{
	GetFunction().mLabelList[label] = value;
}


void GeneratorCore::MapQualifiedSymbolName(const Symbol *symbol)
{
	const Symbol *sym = symbol;
	while (sym != NULL)
	{
		const Token *name = sym->GetName();
		if (name != NULL)
		{
			MapString(name->GetHashedText());
		}
		sym = sym->GetParentSymbol();
	}
}


bu16_t GeneratorCore::MapString(const HashedString &str)
{
	if (!IsInUShortRange(mStringList.size()))
	{
		PushError(CompilerError::STRING_TABLE_OVERFLOW);
	}
	if (!IsInUShortRange(str.GetLength()))
	{
		PushError(CompilerError::STRING_OVERFLOW);
	}
	const bu16_t index = bu16_t(mStringList.size());
	StringIndexMap::InsertResult insertResult = mStringIndexMap.insert(StringIndexMap::KeyValue(str, index));
	if (insertResult.second)
	{
		mStringList.push_back(str);
	}
	return insertResult.first->second;
}


bu16_t GeneratorCore::MapValue32(const Value32 &value)
{
	if (!IsInUShortRange(mValue32List.size()))
	{
		PushError(CompilerError::VALUE32_TABLE_OVERFLOW);
	}
	const bu16_t index = bu16_t(mValue32List.size());
	Value32IndexMap::InsertResult insertResult = mValue32IndexMap.insert(Value32IndexMap::KeyValue(value, index));
	if (insertResult.second)
	{
		mValue32List.push_back(value);
	}
	return insertResult.first->second;
}


bu16_t GeneratorCore::MapValue64(const Value64 &value)
{
	if (!IsInUShortRange(mValue64List.size()))
	{
		PushError(CompilerError::VALUE64_TABLE_OVERFLOW);
	}
	const bu16_t index = bu16_t(mValue64List.size());
	Value64IndexMap::InsertResult insertResult = mValue64IndexMap.insert(Value64IndexMap::KeyValue(value, index));
	if (insertResult.second)
	{
		mValue64List.push_back(value);
	}
	return insertResult.first->second;
}


void GeneratorCore::AssertStackEmpty()
{
	if (mStackTop.GetTop() != 0)
	{
		PushError(CompilerError::INTERNAL_ERROR);
	}
}


void GeneratorCore::PushError(CompilerError::Type type)
{
	if (!mErrorBuffer.HasErrors())
	{
		mErrorBuffer.PushError(type);
	}
}


bool GeneratorCore::JumpEntryOpCodePosComparator::operator()(const GeneratorCore::JumpEntry &entry, size_t opCodePos) const
{
	return entry.mOpCodePos < opCodePos;
}

}
