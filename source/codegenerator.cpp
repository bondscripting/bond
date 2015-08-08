#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/parsenodetraverser.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/io/outputstream.h"
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
			OutputStream &stream,
			CompilerErrorBuffer &errorBuffer,
			const TranslationUnit *translationUnitList,
			PointerSize pointerSize):
		mQualifiedNameIndexMap(QualifiedNameIndexMap::Compare(), QualifiedNameIndexMap::Allocator(&allocator)),
		mQualifiedNameList(QualifiedNameList::Allocator(&allocator)),
		mStringIndexMap(StringIndexMap::Compare(), StringIndexMap::Allocator(&allocator)),
		mStringList(StringList::Allocator(&allocator)),
		mValue32IndexMap(Value32IndexMap::Compare(), Value32IndexMap::Allocator(&allocator)),
		mValue32List(Value32List::Allocator(&allocator)),
		mValue64IndexMap(Value64IndexMap::Compare(), Value64IndexMap::Allocator(&allocator)),
		mValue64List(Value64List::Allocator(&allocator)),
		mFunctionList(FunctionList::Allocator(&allocator)),
		mNativeMemberList(NativeMemberList::Allocator(&allocator)),
		mDataList(DataList::Allocator(&allocator)),
		mAllocator(allocator),
		mStream(stream),
		mErrorBuffer(errorBuffer),
		mTranslationUnitList(translationUnitList),
		mPointerSize(pointerSize)
	{}

	void Generate();

	GeneratorCore &operator=(const GeneratorCore &other) = delete;

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
			mConstantValue(nullptr),
			mNativeMember(nullptr),
			mOffset(0)
		{}

		Result(Context context):
			mContext(context),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(nullptr),
			mNativeMember(nullptr),
			mOffset(0)
		{}

		Result(Context context, int32_t offset):
			mContext(context),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(nullptr),
			mNativeMember(nullptr),
			mOffset(offset)
		{}

		Result(Context context, const Result &thisPointerResult, const NamedInitializer *nativeMember = nullptr):
			mContext(context),
			mThisPointerContext(thisPointerResult.mContext),
			mConstantValue(nullptr),
			mNativeMember(nativeMember),
			mOffset(thisPointerResult.mOffset)
		{}

		Result(const TypeAndValue *constantValue):
			mContext(CONTEXT_CONSTANT_VALUE),
			mThisPointerContext(CONTEXT_NONE),
			mConstantValue(constantValue),
			mNativeMember(nullptr),
			mOffset(0)
		{}

		Result GetThisPointerResult() const { return Result(mThisPointerContext, mOffset); }

		Context mContext;
		Context mThisPointerContext;
		const TypeAndValue *mConstantValue;
		const NamedInitializer *mNativeMember;
		int32_t mOffset;
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

	typedef Vector<uint8_t> ByteCode;
	typedef Vector<size_t> LabelList;
	typedef Vector<JumpEntry> JumpList;

	struct CompiledFunction
	{
		CompiledFunction(
				const FunctionDefinition *definition,
				Allocator &allocator,
				uint32_t argSize,
				uint32_t packedArgSize,
				uint32_t framePointerAlignment,
				uint16_t nameIndex):
			mDefinition(definition),
			mByteCode(ByteCode::Allocator(&allocator)),
			mLabelList(LabelList::Allocator(&allocator)),
			mJumpList(JumpList::Allocator(&allocator)),
			mArgSize(argSize),
			mPackedArgSize(packedArgSize),
			mLocalSize(0),
			mStackSize(0),
			mFramePointerAlignment(framePointerAlignment),
			mNameIndex(nameIndex)
		{}
		const FunctionDefinition *mDefinition;
		ByteCode::Type mByteCode;
		LabelList::Type mLabelList;
		JumpList::Type mJumpList;

		uint32_t mArgSize;
		uint32_t mPackedArgSize;
		uint32_t mLocalSize;
		uint32_t mStackSize;
		uint32_t mFramePointerAlignment;
		uint16_t mNameIndex;
	};

	struct InitializerIndex
	{
		InitializerIndex(bool isLocal): mCurrentOffset(0), mZeroSize(0), mIsLocal(isLocal) {}
		int32_t mCurrentOffset;
		int32_t mZeroSize;
		bool mIsLocal;
	};

	struct QualifiedNameEntry
	{
		QualifiedNameEntry(const Symbol *symbol, const SimpleString &suffix): mSymbol(symbol), mSuffix(suffix) {}

		bool operator<(const QualifiedNameEntry &other) const
		{
			return (mSymbol == other.mSymbol) ? (mSuffix < other.mSuffix) : (mSymbol < other.mSymbol);
		}

		const Symbol *mSymbol;
		SimpleString mSuffix;
	};

	struct NativeMemberEntry
	{
		NativeMemberEntry(const NamedInitializer *nativeMember, uint16_t getterIndex, uint16_t setterIndex):
			mNativeMember(nativeMember),
			mGetterNameIndex(getterIndex),
			mSetterNameIndex(setterIndex)
		{}

		NativeMemberEntry &operator=(const NativeMemberEntry &other) = delete;

		const NamedInitializer *mNativeMember;
		const uint16_t mGetterNameIndex;
		const uint16_t mSetterNameIndex;
	};

	struct DataEntry
	{
		DataEntry(const NamedInitializer *data, uint16_t nameIndex):
			mData(data),
			mNameIndex(nameIndex)
		{}

		const NamedInitializer *mData;
		uint16_t mNameIndex;
	};

	typedef Map<QualifiedNameEntry, uint16_t> QualifiedNameIndexMap;
	typedef Map<SimpleString, uint16_t> StringIndexMap;
	typedef Map<Value32, uint16_t> Value32IndexMap;
	typedef Map<Value64, uint16_t> Value64IndexMap;
	typedef Vector<QualifiedNameEntry> QualifiedNameList;
	typedef Vector<SimpleString> StringList;
	typedef Vector<Value32> Value32List;
	typedef Vector<Value64> Value64List;
	typedef List<CompiledFunction> FunctionList;
	typedef Vector<NativeMemberEntry> NativeMemberList;
	typedef Vector<DataEntry> DataList;
	typedef AutoStack<CompiledFunction *> FunctionStack;
	typedef AutoStack<Result> ResultStack;
	typedef SizeStack LabelStack;

	virtual void Traverse(const ParseNode *parseNode);
	void TraverseOmitOptionalTemporaries(const Expression *expression);
	bool TraverseCollapseNotOperators(const Expression *expression);
	void TraverseOmitConstantFolding(const Expression *expression);

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
	virtual void Visit(const ConstantLiteralExpression *constantExpression);
	virtual void Visit(const IdentifierExpression *identifierExpression);
	virtual void Visit(const ThisExpression *thisExpression);

	bool ProcessConstantExpression(const Expression *expression);

	void EmitInitializer(const Initializer *initializer, InitializerIndex &index, int32_t offset, bool isLast);
	void EmitZero(InitializerIndex &index, int32_t offset, uint32_t size);
	void FlushZero(InitializerIndex &index, bool isLast);
	void AdvancePointer(InitializerIndex &index, int32_t offset);

	Result EmitCallNativeGetter(const Result &thisPointerResult, const NamedInitializer *member);

	void EmitPushResultAs(const Result &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType);
	void EmitPushResult(const Result &result, const TypeDescriptor *typeDescriptor);
	void EmitPushFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset);
	void EmitPushFramePointerIndirectValue32(int32_t offset);
	void EmitPushFramePointerIndirectValue64(int32_t offset);
	void EmitPushAddressIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset);
	void EmitPushStackValue(const TypeDescriptor *typeDescriptor, int32_t offset);

	void EmitPushConstantAs(const TypeAndValue &typeAndValue, const TypeDescriptor *resultType);
	void EmitPushConstant(const TypeAndValue &typeAndValue);
	void EmitPushConstantInt(int32_t value);
	void EmitPushConstantUInt(uint32_t value);
	void EmitPushConstantLong(int64_t value);
	void EmitPushConstantULong(uint64_t value);
	void EmitPushConstantFloat(float value);
	void EmitPushConstantDouble(double value);

	void EmitPopResultAs(const Result &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType);
	void EmitPopResult(const Result &result, const TypeDescriptor *typeDescriptor);
	void EmitPopFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset);
	void EmitPopFramePointerIndirectValue32(int32_t offset);
	void EmitPopFramePointerIndirectValue64(int32_t offset);
	void EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset);
	Result EmitAccumulateAddressOffset(const Result &result);
	void EmitAccumulateAddressOffset(int32_t offset);

	void EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType);
	void EmitCastWithoutNarrowing32(const TypeDescriptor *sourceType, const TypeDescriptor *destType);

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
	void EmitPointerOffset(const Expression *offsetExpression, int32_t elementSize);

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

	void EmitOpCodeWithOffset(OpCode opCode, int32_t offset);
	void EmitOpCode(OpCode opCode);
	void EmitQualifiedName(const Symbol *symbol, const char *suffix = nullptr);
	void EmitValue16(Value16 value);
	void EmitValue32At(Value32 value, size_t pos);
	void EmitIndexedValue32(Value32 value);
	void EmitIndexedValue64(Value64 value);

	Result::Context TransformContext(Result::Context targetContext, const TypeDescriptor *typeDescriptor) const;

	void ResolveJumps();
	const JumpEntry *FindJumpEntry(const JumpList::Type &jumpList, size_t opCodePos) const;

	void WriteCbo();
	void WriteConstantTable();
	uint32_t WriteFunctionList(uint16_t functionIndex);
	uint32_t WriteNativeMemberList(uint16_t functionIndex);
	uint32_t WriteDataList(uint16_t dataIndex);
	void WriteQualifiedName(const QualifiedNameEntry &entry);
	void WriteQualifiedNameIndices(const Symbol *symbol);
	void WriteString(const SimpleString &simpleString);
	void WriteSizeAndType(const TypeDescriptor *type);
	void WriteParamListSignature(const Parameter *parameterList, bool includeThis);
	void WriteValue16(Value16 value);
	void WriteValue32(Value32 value);
	void WriteValue64(Value64 value);

	bool Is64BitPointer() const { return mPointerSize == POINTER_64BIT; }
	CompiledFunction &GetFunction() { return *mFunction.GetTop(); }
	ByteCode::Type &GetByteCode() { return GetFunction().mByteCode; }
	bool GetEmitOptionalTemporaries() const { return (mFlags.GetTop() & FLAG_OMIT_OPTIONAL_TEMPORARIES) == 0; }
	bool GetCollapseNotOperators() const { return (mFlags.GetTop() & FLAG_COLLAPSE_NOT_OPERATORS) != 0; }
	bool GetOmitConstantFolding() const { return (mFlags.GetTop() & FLAG_OMIT_CONSTANT_FOLDING) != 0; }
	void ApplyStackDelta(int32_t delta);
	int32_t AllocateLocal(const TypeDescriptor* typeDescriptor);
	size_t CreateLabel();
	void SetLabelValue(size_t label, size_t value);
	uint16_t MapQualifiedName(const Symbol *symbol, const char *suffix = nullptr);
	uint16_t MapString(const SimpleString &str);
	uint16_t MapValue32(const Value32 &value32);
	uint16_t MapValue64(const Value64 &value32);

	void AssertStackEmpty();
	void PushError(CompilerError::Type type);

	static const uint32_t FLAG_OMIT_OPTIONAL_TEMPORARIES = 1 << 0;
	static const uint32_t FLAG_COLLAPSE_NOT_OPERATORS = 1 << 1;
	static const uint32_t FLAG_OMIT_CONSTANT_FOLDING = 1 << 2;

	QualifiedNameIndexMap::Type mQualifiedNameIndexMap;
	QualifiedNameList::Type mQualifiedNameList;
	StringIndexMap::Type mStringIndexMap;
	StringList::Type mStringList;
	Value32IndexMap::Type mValue32IndexMap;
	Value32List::Type mValue32List;
	Value64IndexMap::Type mValue64IndexMap;
	Value64List::Type mValue64List;
	FunctionList::Type mFunctionList;
	NativeMemberList::Type mNativeMemberList;
	DataList::Type mDataList;
	ResultStack mResult;
	FunctionStack mFunction;
	LabelStack mContinueLabel;
	LabelStack mBreakLabel;
	UIntStack mLocalOffset;
	UIntStack mStackTop;
	UIntStack mFlags;
	BoolStack mExpressionIsNegated;
	Allocator &mAllocator;
	OutputStream &mStream;
	CompilerErrorBuffer &mErrorBuffer;
	const TranslationUnit *mTranslationUnitList;
	PointerSize mPointerSize;
};


void CodeGenerator::Generate(const TranslationUnit *translationUnitList, OutputStream &stream)
{
	GeneratorCore generator(mAllocator, stream, mErrorBuffer, translationUnitList, mPointerSize);
	generator.Generate();
}


void GeneratorCore::Generate()
{
	const uint16_t nameIndex = MapQualifiedName(nullptr);
	mFunctionList.emplace_back(nullptr, mAllocator, 0, 0, uint32_t(BOND_SLOT_SIZE), nameIndex);
	CompiledFunction &function = mFunctionList.back();
	FunctionStack::Element functionElement(mFunction, &function);
	UIntStack::Element localOffsetElement(mLocalOffset, 0);
	UIntStack::Element stackTopElement(mStackTop, 0);
	UIntStack::Element flagsElement(mFlags, 0);
	TraverseList(mTranslationUnitList);

	if (!function.mByteCode.empty())
	{
		EmitOpCode(OPCODE_RETURN);
	}

	ResolveJumps();
	WriteCbo();
}


void GeneratorCore::Traverse(const ParseNode *parseNode)
{
	UIntStack::Element flagsElement(mFlags, 0);
	ParseNodeTraverser::Traverse(parseNode);
}


void GeneratorCore::TraverseOmitOptionalTemporaries(const Expression *expression)
{
	UIntStack::Element flagsElement(mFlags, FLAG_OMIT_OPTIONAL_TEMPORARIES);
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
	BoolStack::Element expressionIsNegatedElement(mExpressionIsNegated, false);
	UIntStack::Element flagsElement(mFlags, FLAG_COLLAPSE_NOT_OPERATORS);
	ParseNodeTraverser::Traverse(expression);
	return expressionIsNegatedElement.GetValue();
}


void GeneratorCore::TraverseOmitConstantFolding(const Expression *expression)
{
	UIntStack::Element flagsElement(mFlags, FLAG_OMIT_CONSTANT_FOLDING);
	ParseNodeTraverser::Traverse(expression);
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
	uint32_t offset = (functionDefinition->GetScope() == SCOPE_STRUCT_MEMBER) ? uint32_t(BOND_SLOT_SIZE) : 0;
	uint32_t packedOffset = offset;
	uint32_t framePointerAlignment = uint32_t(BOND_SLOT_SIZE);
	const Parameter *parameterList = prototype->GetParameterList();
	while (parameterList != nullptr)
	{
		const TypeDescriptor *typeDescriptor = parameterList->GetTypeDescriptor();
		const uint32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), uint32_t(BOND_SLOT_SIZE));
		offset += typeDescriptor->GetSize(mPointerSize);
		offset = AlignUp(offset, alignment);
		packedOffset += typeDescriptor->GetStackSize(mPointerSize);
		framePointerAlignment = Max(framePointerAlignment, alignment);
		parameterList->SetOffset(-int32_t(offset));
		parameterList = NextNode(parameterList);
	}

	const uint16_t nameIndex = MapQualifiedName(functionDefinition);
	mFunctionList.emplace_back(
		functionDefinition,
		mAllocator,
		offset,
		packedOffset,
		framePointerAlignment,
		nameIndex);
	CompiledFunction &function = mFunctionList.back();
	function.mLabelList.resize(functionDefinition->GetNumReservedJumpTargetIds());

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
		{
			const TypeAndValue *typeAndValue = namedInitializer->GetTypeAndValue();
			const uint16_t nameIndex = MapQualifiedName(namedInitializer);
			if (typeAndValue->IsValueDefined())
			{
				switch (typeAndValue->GetTypeDescriptor()->GetSignatureType())
				{
					case SIG_LONG:
					case SIG_ULONG:
					case SIG_DOUBLE:
						MapValue64(Value64(typeAndValue->GetULongValue()));
						break;
					default:
						break;
				}
			}
			else
			{
				const Initializer *initializer = namedInitializer->GetInitializer();
				if (initializer != nullptr)
				{
					InitializerIndex index(false);
					EmitOpCode(OPCODE_LOADEA);
					EmitValue16(Value16(nameIndex));
					EmitInitializer(initializer, index, 0, true);
					FlushZero(index, true);
				}
			}
			mDataList.emplace_back(namedInitializer, nameIndex);
		}
		break;

		case SCOPE_LOCAL:
		{
			if (!namedInitializer->IsElidable())
			{
				const TypeDescriptor *lhDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
				const int32_t offset = AllocateLocal(lhDescriptor);
				namedInitializer->SetOffset(offset);
				const Initializer *initializer = namedInitializer->GetInitializer();
				if (initializer != nullptr)
				{
					InitializerIndex index(true);
					EmitInitializer(initializer, index, offset, true);
					FlushZero(index, true);
				}
			}
		}
		break;

		case SCOPE_STRUCT_MEMBER:
			if (namedInitializer->IsNativeStructMember())
			{
				const uint16_t getterIndex = MapQualifiedName(namedInitializer, BOND_NATIVE_GETTER_SUFFIX);
				const uint16_t setterIndex = MapQualifiedName(namedInitializer, BOND_NATIVE_SETTER_SUFFIX);
				mNativeMemberList.emplace_back(namedInitializer, getterIndex, setterIndex);
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
		else if (ifStatement->GetElseStatement() != nullptr)
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

		if (ifStatement->GetElseStatement() != nullptr)
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

	const uint32_t numMatches = switchStatement->GetNumMatches();
	const int32_t minMatch = switchStatement->GetMinMatch();
	const int32_t maxMatch = switchStatement->GetMaxMatch();
	const uint32_t range = maxMatch - minMatch;
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

	int32_t defaultOffset = 0;
	if ((resolvedLabels != nullptr) && resolvedLabels->IsDefault())
	{
		defaultOffset = uint32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
		resolvedLabels = NextNode(resolvedLabels);
	}
	else
	{
		defaultOffset = uint32_t(endPos - jumpTableEnd);
	}

	EmitValue32At(Value32(defaultOffset), jumpTableStart);

	if (doLookupSwitch)
	{
		EmitValue32At(Value32(numMatches), jumpTableStart + sizeof(Value32));
		size_t pos = jumpTableStart + (2 * sizeof(Value32));
		while (resolvedLabels != nullptr)
		{
			const int32_t offset = int32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
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
		for (uint32_t i = 0; i <= range; ++i)
		{
			const int32_t match = minMatch + i;
			int32_t offset = 0;
			if ((resolvedLabels != nullptr) && (match >= resolvedLabels->GetMatch()))
			{
				offset = int32_t(labelList[resolvedLabels->GetJumpTargetId()] - jumpTableEnd);
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
	if (condition != nullptr)
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
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPLT_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPLT_OPCODES);
				}
				break;
			case Token::OP_LTE:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPLE_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPLE_OPCODES);
				}
				break;
			case Token::OP_GT:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPGT_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPGT_OPCODES);
				}
				break;
			case Token::OP_GTE:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					result = EmitPointerComparison(lhs, rhs, CMPGE_OPCODES);
				}
				else
				{
					result = EmitSimpleBinaryOperator(binaryExpression, CMPGE_OPCODES);
				}
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
		const NamedInitializer *namedInitializer = nullptr;
		if ((namedInitializer = CastNode<NamedInitializer>(member)) != nullptr)
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
		else if (CastNode<FunctionDefinition>(member) != nullptr)
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
	const SignatureType returnType = returnDescriptor->GetSignatureType();
	const int32_t returnOffset = (returnType == SIG_AGGREGATE) ? AllocateLocal(returnDescriptor) : 0;
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

		if (returnType == SIG_AGGREGATE)
		{
			EmitOpCodeWithOffset(OPCODE_LOADFP, returnOffset);
		}

		EmitOpCode(OPCODE_INVOKE);
		EmitQualifiedName(function);
	}

	if (returnType == SIG_AGGREGATE)
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


void GeneratorCore::Visit(const ConstantLiteralExpression *constantExpression)
{
	ProcessConstantExpression(constantExpression);
}


void GeneratorCore::Visit(const IdentifierExpression *identifierExpression)
{
	if (!ProcessConstantExpression(identifierExpression))
	{
		const Symbol *symbol = identifierExpression->GetDefinition();
		const NamedInitializer *namedInitializer = nullptr;
		const Parameter *parameter = nullptr;
		const FunctionDefinition *functionDefinition = nullptr;
		if ((namedInitializer = CastNode<NamedInitializer>(symbol)) != nullptr)
		{
			const TypeDescriptor *typeDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
			Result::Context context = Result::CONTEXT_NONE;
			int32_t offset = namedInitializer->GetOffset();

			switch (namedInitializer->GetScope())
			{
				case SCOPE_GLOBAL:
				{
					EmitOpCode(OPCODE_LOADEA);
					EmitQualifiedName(namedInitializer);
					context = Result::CONTEXT_ADDRESS_INDIRECT;
					offset = 0;
				}
				break;
				case SCOPE_LOCAL:
				{
					context = Result::CONTEXT_FP_INDIRECT;
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
					context = Result::CONTEXT_ADDRESS_INDIRECT;
				}
				break;
			}

			context = TransformContext(context, typeDescriptor);
			mResult.SetTop(Result(context, offset));
		}
		else if ((parameter = CastNode<Parameter>(symbol)) != nullptr)
		{
			mResult.SetTop(Result(Result::CONTEXT_FP_INDIRECT, parameter->GetOffset()));
		}
		else if (((functionDefinition = CastNode<FunctionDefinition>(symbol)) != nullptr) &&
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
	if (tav.IsValueDefined() && !GetOmitConstantFolding())
	{
		mResult.SetTop(Result(&tav));
		return true;
	}
	return false;
}


void GeneratorCore::EmitInitializer(const Initializer *initializer, InitializerIndex &index, int32_t offset, bool isLast)
{
	const TypeDescriptor *typeDescriptor = initializer->GetTypeDescriptor();
	const Expression *expression = initializer->GetExpression();
	const Initializer *initializerList = initializer->GetInitializerList();

	if (typeDescriptor->IsArrayType())
	{
		uint32_t numElements = typeDescriptor->GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
		const TypeDescriptor elementDescriptor = typeDescriptor->GetDereferencedType();
		const uint32_t elementSize = elementDescriptor.GetSize(mPointerSize);

		if (initializerList != nullptr)
		{
			while ((numElements > 0) && (initializerList != nullptr))
			{
				--numElements;
				EmitInitializer(initializerList, index, offset, isLast && (numElements == 0));
				offset += elementSize;
				initializerList = NextNode(initializerList);
			}
		}
		else if (expression != nullptr)
		{
			const ConstantLiteralExpression *constantExpression = CastNode<ConstantLiteralExpression>(expression);
			const bool isStringInitializer =
				elementDescriptor.IsCharType() &&
				(constantExpression != nullptr) &&
				constantExpression->GetTypeDescriptor()->IsStringType();

			if (isStringInitializer)
			{
				numElements = 0;
				if (index.mIsLocal)
				{
					EmitOpCodeWithOffset(OPCODE_LOADFP, offset);
				}
				else if (!isLast)
				{
					EmitOpCode(OPCODE_DUP);
				}
				EmitPushConstant(constantExpression->GetTypeAndValue());
				EmitOpCode(OPCODE_MEMCOPYW);
				const uint32_t stringLength = uint32_t(constantExpression->GetValueToken()->GetStringLength() + 1);
				EmitIndexedValue32(Value32(stringLength));
			}
		}

		if (numElements > 0)
		{
			EmitZero(index, offset, numElements * elementSize);
		}
	}
	else if (typeDescriptor->IsStructType() && (initializerList != nullptr))
	{
		// TODO: Can't initialize native structs via initializer list.
		const TypeSpecifier *structSpecifier = typeDescriptor->GetTypeSpecifier();
		const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
		const DeclarativeStatement *memberDeclarationList = structDeclaration->GetMemberVariableList();
		while (memberDeclarationList != nullptr)
		{
			const DeclarativeStatement *nextMemberDeclaration = NextNode(memberDeclarationList);
			const TypeDescriptor *memberDescriptor = memberDeclarationList->GetTypeDescriptor();
			const NamedInitializer *nameList = memberDeclarationList->GetNamedInitializerList();
			const uint32_t memberSize = memberDescriptor->GetSize(mPointerSize);
			while (nameList != nullptr)
			{
				const NamedInitializer *nextName = NextNode(nameList);
				const uint32_t memberOffset = offset + uint32_t(nameList->GetOffset());

				if (initializerList != nullptr)
				{
					EmitInitializer(initializerList, index, memberOffset, isLast && (nextMemberDeclaration == nullptr) && (nextName == nullptr));
					initializerList = NextNode(initializerList);
				}
				else
				{
					EmitZero(index, memberOffset, memberSize);
				}
				nameList = nextName;
			}
			memberDeclarationList = nextMemberDeclaration;
		}
	}
	else
	{
		FlushZero(index, false);
		AdvancePointer(index, offset);
		UIntStack::Element localOffsetElement(mLocalOffset, mLocalOffset.GetTop());

		if (!index.mIsLocal && !isLast)
		{
			EmitOpCode(OPCODE_DUP);
		}

		if (typeDescriptor->IsStructType())
		{
			// TODO: Initialize the struct in-place without copying it.
			if (index.mIsLocal)
			{
				EmitOpCodeWithOffset(OPCODE_LOADFP, offset);
			}
			ResultStack::Element rhResult(mResult);
			Traverse(initializer);
			EmitPushAddressOfResult(rhResult);
			EmitOpCode(OPCODE_MEMCOPYW);
			EmitIndexedValue32(Value32(typeDescriptor->GetSize(mPointerSize)));
		}
		else
		{
			const TypeDescriptor *rhDescriptor = initializer->GetExpression()->GetTypeDescriptor();
			ResultStack::Element rhResult(mResult);
			Traverse(initializer);
			EmitPushResult(rhResult, rhDescriptor);
			const Result lhResult = index.mIsLocal ?
				Result(Result::CONTEXT_FP_INDIRECT, offset) :
				Result(Result::CONTEXT_ADDRESS_INDIRECT, 0);
			EmitPopResultAs(lhResult, rhDescriptor, typeDescriptor);
		}
	}
}


void GeneratorCore::EmitZero(InitializerIndex &index, int32_t offset, uint32_t size)
{
	// Merge the requested run of zeros with an existing one.
	if (index.mZeroSize > 0)
	{
		index.mZeroSize = (offset + size) - index.mCurrentOffset;
	}
	// Otherwise start a new run.
	else
	{
		AdvancePointer(index, offset);
		index.mZeroSize = size;
	}
}


void GeneratorCore::FlushZero(InitializerIndex &index, bool isLast)
{
	if (index.mZeroSize > 0)
	{
		if (index.mIsLocal)
		{
			EmitOpCodeWithOffset(OPCODE_LOADFP, index.mCurrentOffset);
		}
		else if (!isLast)
		{
			EmitOpCode(OPCODE_DUP);
		}
		EmitOpCode(OPCODE_MEMZEROW);
		EmitIndexedValue32(Value32(index.mZeroSize));
		index.mZeroSize = 0;
	}
}


void GeneratorCore::AdvancePointer(InitializerIndex &index, int32_t offset)
{
	if (index.mCurrentOffset < offset)
	{
		if (!index.mIsLocal)
		{
			EmitAccumulateAddressOffset(offset - index.mCurrentOffset);
		}
		index.mCurrentOffset = offset;
	}
}


GeneratorCore::Result GeneratorCore::EmitCallNativeGetter(const Result &thisPointerResult, const NamedInitializer *member)
{
	const TypeDescriptor *returnDescriptor = member->GetTypeAndValue()->GetTypeDescriptor();
	const SignatureType returnType = returnDescriptor->GetSignatureType();
	const int32_t returnOffset = (returnType == SIG_AGGREGATE) ? AllocateLocal(returnDescriptor) : 0;

	{
		const TypeDescriptor voidStar = TypeDescriptor::GetVoidPointerType();
		EmitPushResult(thisPointerResult, &voidStar);
	}

	if (returnType == SIG_AGGREGATE)
	{
		EmitOpCodeWithOffset(OPCODE_LOADFP, returnOffset);
	}

	EmitOpCode(OPCODE_INVOKE);
	EmitQualifiedName(member, BOND_NATIVE_GETTER_SUFFIX);

	int32_t stackDelta = -BOND_SLOT_SIZE;
	Result result;
	if (returnType == SIG_AGGREGATE)
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


void GeneratorCore::EmitPushFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset)
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


void GeneratorCore::EmitPushFramePointerIndirectValue32(int32_t offset)
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


void GeneratorCore::EmitPushFramePointerIndirectValue64(int32_t offset)
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


void GeneratorCore::EmitPushAddressIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset)
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


void GeneratorCore::EmitPushStackValue(const TypeDescriptor *typeDescriptor, int32_t offset)
{
	if (typeDescriptor->IsPointerType())
	{
		EmitAccumulateAddressOffset(offset);
	}
}


void GeneratorCore::EmitPushConstantAs(const TypeAndValue &typeAndValue, const TypeDescriptor *destType)
{
	const TypeDescriptor *sourceType = typeAndValue.GetTypeDescriptor();
	if (sourceType->IsPrimitiveType())
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
				const uint16_t stringIndex = MapString(typeAndValue.GetStringValue());
				EmitValue16(Value16(stringIndex));
			}
			break;
	}
}


void GeneratorCore::EmitPushConstantInt(int32_t value)
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
			if (IsInRange<int8_t>(value))
			{
				EmitOpCode(OPCODE_CONSTC);
				GetByteCode().push_back(uint8_t(value));
			}
			else if (IsInRange<uint8_t>(value))
			{
				EmitOpCode(OPCODE_CONSTUC);
				GetByteCode().push_back(uint8_t(value));
			}
			else if (IsInRange<int16_t>(value))
			{
				EmitOpCode(OPCODE_CONSTS);
				EmitValue16(Value16(value));
			}
			else if (IsInRange<uint16_t>(value))
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


void GeneratorCore::EmitPushConstantUInt(uint32_t value)
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
			if (IsInRange<uint8_t>(value))
			{
				EmitOpCode(OPCODE_CONSTUC);
				GetByteCode().push_back(uint8_t(value));
			}
			else if (IsInRange<uint16_t>(value))
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


void GeneratorCore::EmitPushConstantLong(int64_t value)
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


void GeneratorCore::EmitPushConstantULong(uint64_t value)
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


void GeneratorCore::EmitPushConstantFloat(float value)
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


void GeneratorCore::EmitPushConstantDouble(double value)
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


void GeneratorCore::EmitPopResultAs(const Result &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	EmitCastWithoutNarrowing32(sourceType, destType);
	EmitPopResult(result, destType);
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


void GeneratorCore::EmitPopFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset)
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


void GeneratorCore::EmitPopFramePointerIndirectValue32(int32_t offset)
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


void GeneratorCore::EmitPopFramePointerIndirectValue64(int32_t offset)
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


void GeneratorCore::EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, int32_t offset)
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


void GeneratorCore::EmitAccumulateAddressOffset(int32_t offset)
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


void GeneratorCore::EmitCastWithoutNarrowing32(const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	switch (destType->GetPrimitiveType())
	{
		case Token::KEY_CHAR:
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

		case Token::KEY_BOOL:
		case Token::KEY_UCHAR:
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

		case Token::KEY_SHORT:
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

		case Token::KEY_USHORT:
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

		default:
			// Remaining logic remains unchanged.
			EmitCast(sourceType, destType);
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

	const MemberExpression *memberExpression = nullptr;
	const NamedInitializer *namedInitializer = nullptr;
	if (((memberExpression = CastNode<MemberExpression>(lhs)) != nullptr) &&
	    ((namedInitializer = CastNode<NamedInitializer>(memberExpression->GetDefinition())) != nullptr) &&
	    namedInitializer->IsNativeStructMember())
	{
		int32_t stackDelta = 0;
		{
			UIntStack::Element stackTopElement(mStackTop, mStackTop.GetTop());
			ResultStack::Element rhResult(mResult);
			Traverse(rhs);
			EmitPushResultAs(rhResult, rhDescriptor, lhDescriptor);

			if (GetEmitOptionalTemporaries() && !lhDescriptor->IsStructType())
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
			EmitQualifiedName(lhResult.GetValue().mNativeMember, BOND_NATIVE_SETTER_SUFFIX);
		}
		ApplyStackDelta(stackDelta);
	}
	else if (lhDescriptor->IsStructType())
	{
		ResultStack::Element lhResult(mResult);
		Traverse(lhs);
		EmitPushAddressOfResult(lhResult);

		if (GetEmitOptionalTemporaries())
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

		if (GetEmitOptionalTemporaries())
		{
			EmitPushResultAs(rhResult, rhDescriptor, lhDescriptor);
			EmitOpCode(valueDupOpCode);
			EmitPopResult(lhResult, lhDescriptor);
			result.mContext = Result::CONTEXT_STACK_VALUE;
		}
		else
		{
			TypeDescriptor intermediateDescriptor = PromoteType(lhDescriptor);
			EmitPushResultAs(rhResult, rhDescriptor, &intermediateDescriptor);
			EmitPopResultAs(lhResult, &intermediateDescriptor, lhDescriptor);
		}
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
	const uint32_t stackTop = mStackTop.GetTop();

	const size_t endLabel = CreateLabel();
	const size_t opCodePos = GetByteCode().size();
	EmitJump(lhsNegated ? negatedOpCode : branchOpCode, endLabel);

	if (GetCollapseNotOperators())
	{
		ResultStack::Element rhResult(mResult);
		const bool rhsNegated = TraverseCollapseNotOperators(rhs);
		EmitPushResult(rhResult, rhDescriptor);

		// Apply De Morgan's laws if we can reduce the number of ! operators.
		if (rhsNegated)
		{
			// Patch up the opcode.
			GetByteCode()[opCodePos] = uint8_t(lhsNegated ? altBranchOpCode : negatedAltOpCode);
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
	const TypeDescriptor intermediateDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);
	const TypeAndValue &rhTav = rhs->GetTypeAndValue();
	const int64_t rhValue = rhTav.AsLongValue() * ((&opCodeSet == &SUB_OPCODES) ? -1 : 1);
	uint32_t stackTop = mStackTop.GetTop();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	const int32_t frameOffset = lhResult.GetValue().mOffset;
	const int32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if (((&opCodeSet == &ADD_OPCODES) || (&opCodeSet == &SUB_OPCODES)) &&
	    (lhResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInRange<uint8_t>(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    lhDescriptor->IsLeast32IntegerType() &&
	    rhTav.IsValueDefined() &&
	    IsInRange<int8_t>(rhValue))
	{
		EmitOpCode(INC_OPCODES.GetOpCode(*lhDescriptor));
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(uint8_t(slotIndex));
		byteCode.push_back(uint8_t(rhValue));

		if (GetEmitOptionalTemporaries())
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
		EmitCast(lhDescriptor, &intermediateDescriptor);

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult, rhDescriptor, &intermediateDescriptor);

		EmitOpCode(opCodeSet.GetOpCode(intermediateDescriptor));
		EmitCast(&intermediateDescriptor, lhDescriptor);

		if (GetEmitOptionalTemporaries())
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitQualifiedName(lhResult.GetValue().mNativeMember, BOND_NATIVE_SETTER_SUFFIX);
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

		EmitPushResultAs(lhResult, lhDescriptor, &intermediateDescriptor);

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult, rhDescriptor, &intermediateDescriptor);

		EmitOpCode(opCodeSet.GetOpCode(intermediateDescriptor));

		if (GetEmitOptionalTemporaries())
		{
			EmitCast(&intermediateDescriptor, lhDescriptor);
			EmitOpCode(valueDupOpCode);
			EmitPopResult(lhResult, lhDescriptor);
		}
		else
		{
			EmitPopResultAs(lhResult, &intermediateDescriptor, lhDescriptor);
		}
	}

	return Result(GetEmitOptionalTemporaries() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitPointerCompoundAssignmentOperator(const Expression *pointerExpression, const Expression *offsetExpression, int sign)
{
	const TypeDescriptor *pointerDescriptor = pointerExpression->GetTypeDescriptor();
	const int32_t elementSize = int32_t(sign * pointerDescriptor->GetDereferencedType().GetSize(mPointerSize));
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	const int64_t offset = offsetTav.AsLongValue() * elementSize;
	uint32_t stackTop = mStackTop.GetTop();

	ResultStack::Element pointerResult(mResult);
	Traverse(pointerExpression);
	const int32_t frameOffset = pointerResult.GetValue().mOffset;
	const int32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((pointerResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInRange<uint8_t>(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    offsetTav.IsValueDefined() &&
	    IsInRange<int8_t>(offset))
	{
		EmitOpCode(INC_OPCODES.GetPointerOpCode(mPointerSize));
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(uint8_t(slotIndex));
		byteCode.push_back(uint8_t(offset));

		if (GetEmitOptionalTemporaries())
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

		if (GetEmitOptionalTemporaries())
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitQualifiedName(pointerResult.GetValue().mNativeMember, BOND_NATIVE_SETTER_SUFFIX);
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

		if (GetEmitOptionalTemporaries())
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(pointerResult, pointerDescriptor);
	}

	return Result(GetEmitOptionalTemporaries() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitPointerArithmetic(const Expression *pointerExpression, const Expression *offsetExpression, int sign)
{
	const TypeDescriptor *pointerDescriptor = pointerExpression->GetTypeDescriptor();
	const int32_t elementSize = int32_t(sign * pointerDescriptor->GetDereferencedType().GetSize(mPointerSize));
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	const int64_t offset = offsetTav.AsLongValue() * elementSize;

	ResultStack::Element pointerResult(mResult);
	Traverse(pointerExpression);

	Result result(Result::CONTEXT_STACK_VALUE);

	if (offsetTav.IsValueDefined() && IsInRange<int32_t>(offset))
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
		result.mOffset += int32_t(offset);
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

	const uint32_t elementSize = lhDescriptor->GetDereferencedType().GetSize(mPointerSize);
	if (IsInRange<int16_t>(elementSize))
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


void GeneratorCore::EmitPointerOffset(const Expression *offsetExpression, int32_t elementSize)
{
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	if (offsetTav.IsValueDefined())
	{
		if (Is64BitPointer())
		{
			const int64_t offset = offsetTav.AsLongValue() * elementSize;
			EmitPushConstantLong(offset);
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			const int32_t offset = offsetTav.GetIntValue() * elementSize;
			EmitPushConstantInt(offset);
			EmitOpCode(OPCODE_ADDI);
		}
	}
	else
	{
		ResultStack::Element offsetResult(mResult);
		Traverse(offsetExpression);

		const TypeDescriptor *offsetDescriptor = offsetTav.GetTypeDescriptor();
		if (IsInRange<int16_t>(elementSize))
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
			EmitPushConstantLong(uint64_t(elementSize));
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
	const int32_t pointerOffset = int32_t(sign * operandDescriptor->GetDereferencedType().GetSize(mPointerSize)) * sign;
	uint32_t stackTop = mStackTop.GetTop();

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const int32_t frameOffset = operandResult.GetValue().mOffset;
	const int32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInRange<uint8_t>(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    IsInRange<int8_t>(pointerOffset))
	{
		ByteCode::Type &byteCode = GetByteCode();
		if (Is64BitPointer())
		{
			if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}

			EmitOpCode(OPCODE_INCL);
			byteCode.push_back(uint8_t(slotIndex));
			byteCode.push_back(uint8_t(pointerOffset));

			if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}
		}
		else
		{
			if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
			{
				EmitPushFramePointerIndirectValue32(frameOffset);
			}

			EmitOpCode(OPCODE_INCI);
			byteCode.push_back(uint8_t(slotIndex));
			byteCode.push_back(uint8_t(pointerOffset));

			if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
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

		if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		if (Is64BitPointer())
		{
			EmitPushConstantLong(int64_t(pointerOffset));
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(pointerOffset);
			EmitOpCode(OPCODE_ADDI);
		}

		if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitQualifiedName(operandResult.GetValue().mNativeMember, BOND_NATIVE_SETTER_SUFFIX);
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

		if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		if (Is64BitPointer())
		{
			EmitPushConstantLong(int64_t(pointerOffset));
			EmitOpCode(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(pointerOffset);
			EmitOpCode(OPCODE_ADDI);
		}

		if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitPopResult(operandResult, operandDescriptor);
	}

	return Result(GetEmitOptionalTemporaries() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign)
{
	const TypeDescriptor *operandDescriptor = operand->GetTypeDescriptor();
	const TypeDescriptor intermediateDescriptor = PromoteType(operandDescriptor);
	const OpCodeSet &constOpCodeSet = (sign > 0) ? CONST1_OPCODES : CONSTN1_OPCODES;
	uint32_t stackTop = mStackTop.GetTop();

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const int32_t frameOffset = operandResult.GetValue().mOffset;
	const int32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == Result::CONTEXT_FP_INDIRECT) &&
	    IsInRange<uint8_t>(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    operandDescriptor->IsLeast32IntegerType())
	{
		if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
		{
			EmitPushResult(operandResult, operandDescriptor);
		}

		EmitOpCode(INC_OPCODES.GetOpCode(intermediateDescriptor));
		GetByteCode().push_back(uint8_t(slotIndex));
		GetByteCode().push_back(uint8_t(sign));

		if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
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

		if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(constOpCodeSet.GetOpCode(intermediateDescriptor));
		EmitOpCode(ADD_OPCODES.GetOpCode(intermediateDescriptor));
		EmitCast(&intermediateDescriptor, operandDescriptor);

		if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
		{
			stackTop += GetStackDelta(OPCODE_DUPINS);
			EmitOpCode(OPCODE_DUPINS);
		}

		EmitOpCode(OPCODE_SWAP);
		EmitOpCode(OPCODE_INVOKE);
		EmitQualifiedName(operandResult.GetValue().mNativeMember, BOND_NATIVE_SETTER_SUFFIX);
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

		EmitPushResultAs(operandResult, operandDescriptor, &intermediateDescriptor);

		if (GetEmitOptionalTemporaries() && (fixedness == POSTFIX))
		{
			EmitOpCode(valueDupOpCode);
		}

		EmitOpCode(constOpCodeSet.GetOpCode(intermediateDescriptor));
		EmitOpCode(ADD_OPCODES.GetOpCode(intermediateDescriptor));

		if (GetEmitOptionalTemporaries() && (fixedness == PREFIX))
		{
			EmitCast(&intermediateDescriptor, operandDescriptor);
			EmitOpCode(valueDupOpCode);
			EmitPopResult(operandResult, operandDescriptor);
		}
		else
		{
			EmitPopResultAs(operandResult, &intermediateDescriptor, operandDescriptor);
		}
	}

	return Result(GetEmitOptionalTemporaries() ?
		Result::CONTEXT_STACK_VALUE :
		Result::CONTEXT_NONE);
}


GeneratorCore::Result GeneratorCore::EmitSignOperator(const UnaryExpression *unaryExpression)
{
	const UnaryExpression *unary = unaryExpression;
	const Expression *rhs = unaryExpression;
	const TypeDescriptor *resultDescriptor = unaryExpression->GetTypeDescriptor();
	bool negated = false;
	while (unary != nullptr)
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

	if (GetCollapseNotOperators())
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
	while (unary != nullptr)
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
	const Expression *rhs = unaryExpression->GetRhs();
	ResultStack::Element rhResult(mResult);
	TraverseOmitConstantFolding(rhs);
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
	if (argList != nullptr)
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


void GeneratorCore::EmitOpCodeWithOffset(OpCode opCode, int32_t offset)
{
	if (IsInRange<int16_t>(offset))
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
	GetByteCode().push_back(uint8_t(opCode));
	ApplyStackDelta(GetStackDelta(opCode));
}


void GeneratorCore::EmitQualifiedName(const Symbol *symbol, const char *suffix)
{
	const uint16_t nameIndex = MapQualifiedName(symbol, suffix);
	EmitValue16(Value16(nameIndex));
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
	const uint16_t index = MapValue32(value);
	EmitValue16(Value16(index));
}


void GeneratorCore::EmitIndexedValue64(Value64 value)
{
	const uint16_t index = MapValue64(value);
	EmitValue16(Value16(index));
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
	for (CompiledFunction &compiledFunction: mFunctionList)
	{
		ByteCode::Type &byteCode = compiledFunction.mByteCode;
		JumpList::Type &jumpList = compiledFunction.mJumpList;
		const LabelList::Type &labelList = compiledFunction.mLabelList;

		// Resolve jump targets.
		for (JumpEntry &jumpEntry: jumpList)
		{
			jumpEntry.mToPos = labelList[jumpEntry.mToLabel];
		}

		// Optmize jumps if the target is another jump instruction.
		bool requiresAnotherPass = true;
		while (requiresAnotherPass)
		{
			requiresAnotherPass = false;
			for (JumpEntry &jumpEntry: jumpList)
			{
				const JumpEntry *targetEntry = FindJumpEntry(jumpList, jumpEntry.mToPos);
				if (targetEntry != nullptr)
				{
					const uint8_t opCode = byteCode[jumpEntry.mOpCodePos];
					const uint8_t targetOpCode = byteCode[targetEntry->mOpCodePos];
					if (targetOpCode == OPCODE_GOTO)
					{
						jumpEntry.mToPos = targetEntry->mToPos;
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
										jumpEntry.mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_NBRNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRNZ:
									case OPCODE_NBRZ:
									case OPCODE_IFNZ:
										jumpEntry.mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_BRNZ:
								switch (targetOpCode)
								{
									case OPCODE_BRNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_NBRZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRZ:
									case OPCODE_NBRNZ:
									case OPCODE_IFZ:
										jumpEntry.mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_NBRZ:
								switch (targetOpCode)
								{
									case OPCODE_BRZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_BRNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRNZ:
									case OPCODE_NBRZ:
									case OPCODE_IFNZ:
										jumpEntry.mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFNZ;
										requiresAnotherPass = true;
										break;
								}
								break;

							case OPCODE_NBRNZ:
								switch (targetOpCode)
								{
									case OPCODE_BRNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										requiresAnotherPass = true;
										break;
									case OPCODE_NBRZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_BRZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_IFNZ:
										jumpEntry.mToPos = targetEntry->mToPos;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFZ;
										requiresAnotherPass = true;
										break;
									case OPCODE_BRZ:
									case OPCODE_NBRNZ:
									case OPCODE_IFZ:
										jumpEntry.mToPos = targetEntry->mOpCodePos + 3;
										byteCode[jumpEntry.mOpCodePos] = OPCODE_IFZ;
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
		for (const JumpEntry &jumpEntry: jumpList)
		{
			const int32_t offset = int32_t(jumpEntry.mToPos - jumpEntry.GetFromPos());
			if (!IsInRange<int16_t>(offset))
			{
				MapValue32(Value32(offset));
			}
		}
	}
}


const GeneratorCore::JumpEntry *GeneratorCore::FindJumpEntry(const JumpList::Type &jumpList, size_t opCodePos) const
{
	auto opCodePosComparator = [](const GeneratorCore::JumpEntry &entry, size_t pos)
		{
			return entry.mOpCodePos < pos;
		};

	auto it = lower_bound(jumpList.begin(), jumpList.end(), opCodePos, opCodePosComparator);
	return ((it < jumpList.end()) && (it->mOpCodePos == opCodePos)) ? &(*it) : nullptr;
}


void GeneratorCore::WriteCbo()
{
	WriteValue32(Value32(MAGIC_NUMBER));
	WriteValue16(Value16(MAJOR_VERSION));
	WriteValue16(Value16(MINOR_VERSION));
	WriteValue16(Value16(EncodePointerSize(0, mPointerSize)));

	const uint16_t listIndex = MapString(BOND_LIST_BLOB_ID);
	const uint16_t functionIndex = (mFunctionList.empty() && mNativeMemberList.empty()) ?
		0 : MapString(BOND_FUNCTION_BLOB_ID);
	const uint16_t dataIndex = mDataList.empty() ? 0 : MapString(BOND_DATA_BLOB_ID);

	WriteConstantTable();

	// Write the list blob header, but skip the blob size and number of elements.
	const Stream::pos_t startPos = mStream.GetPosition();
	mStream.AddOffset(sizeof(Value32));
	WriteValue16(Value16(listIndex));
	const Stream::pos_t numListElementsPos = mStream.GetPosition();
	mStream.AddOffset(sizeof(Value32));

	uint32_t numListElements = WriteFunctionList(functionIndex);
	numListElements += WriteNativeMemberList(functionIndex);
	numListElements += WriteDataList(dataIndex);

	// Patch up the blob size and number of list elements.
	const Stream::pos_t endPos = mStream.GetPosition();
	mStream.SetPosition(startPos);
	WriteValue32(Value32(uint32_t(endPos - startPos)));
	mStream.SetPosition(numListElementsPos);
	WriteValue32(Value32(numListElements));
	mStream.SetPosition(endPos);
}


void GeneratorCore::WriteConstantTable()
{
	const Stream::pos_t startPos = mStream.GetPosition();

	// Skip the 4 bytes for the table size.
	mStream.AddOffset(sizeof(Value32));

	WriteValue16(Value16(uint16_t(mValue32List.size())));
	WriteValue16(Value16(uint16_t(mValue64List.size())));
	WriteValue16(Value16(uint16_t(mStringList.size())));
	WriteValue16(Value16(uint16_t(mQualifiedNameList.size())));

	for (const Value32 &value: mValue32List)
	{
		WriteValue32(value);
	}

	for (const Value64 &value: mValue64List)
	{
		WriteValue64(value);
	}

	for (const SimpleString &simpleString: mStringList)
	{
		WriteString(simpleString);
	}

	for (const QualifiedNameEntry &qualifiedName: mQualifiedNameList)
	{
		WriteQualifiedName(qualifiedName);
	}

	// Patch up the table size.
	const Stream::pos_t endPos = mStream.GetPosition();
	mStream.SetPosition(startPos);
	WriteValue32(Value32(uint32_t(endPos - startPos)));
	mStream.SetPosition(endPos);
}


uint32_t GeneratorCore::WriteFunctionList(uint16_t functionIndex)
{
	uint32_t numFunctions = 0;

	for (const CompiledFunction &compiledFunction: mFunctionList)
	{
		const FunctionDefinition *function = compiledFunction.mDefinition;
		const ByteCode::Type &byteCode = compiledFunction.mByteCode;
		const bool isStaticInitializer = function == nullptr;

		// Skip empty static initializers.
		if (isStaticInitializer && byteCode.empty())
		{
			continue;
		}

		// Cache the blob start position and skip 4 bytes for the blob size.
		const int blobStartPos = mStream.GetPosition();
		mStream.AddOffset(sizeof(Value32));

		WriteValue16(Value16(functionIndex));
		const Value16 nameIndex(compiledFunction.mNameIndex);
		WriteValue16(nameIndex);

		if (isStaticInitializer)
		{
			const TypeDescriptor voidTypeDescriptor = TypeDescriptor::GetVoidType();
			WriteSizeAndType(&voidTypeDescriptor);
			WriteParamListSignature(nullptr, false);
		}
		else
		{
			WriteSizeAndType(function->GetPrototype()->GetReturnType());
			WriteParamListSignature(function->GetPrototype()->GetParameterList(), function->GetScope() == SCOPE_STRUCT_MEMBER);
		}

		WriteValue32(Value32(compiledFunction.mArgSize));
		WriteValue32(Value32(compiledFunction.mPackedArgSize));
		WriteValue32(Value32(compiledFunction.mLocalSize));
		WriteValue32(Value32(compiledFunction.mStackSize));
		WriteValue32(Value32(compiledFunction.mFramePointerAlignment));

		// Cache the code start position and skip 4 bytes for the code size.
		const int codeSizePos = mStream.GetPosition();
		mStream.AddOffset(sizeof(Value32));
		const int codeStartPos = mStream.GetPosition();

		const JumpList::Type &jumpList = compiledFunction.mJumpList;

		size_t byteCodeIndex = 0;
		for (const JumpEntry &jumpEntry: jumpList)
		{
			while (byteCodeIndex < jumpEntry.mOpCodePos)
			{
				mStream.Write(byteCode[byteCodeIndex++]);
			}

			uint8_t opCode = byteCode[byteCodeIndex++];
			const int32_t offset = int32_t(jumpEntry.mToPos) - int32_t(jumpEntry.GetFromPos());
			Value16 arg(offset);

			if (!IsInRange<int16_t>(offset))
			{
				++opCode;
				arg.mUShort = MapValue32(Value32(offset));
			}

			mStream.Write(opCode);
			WriteValue16(arg);
			byteCodeIndex += 2;
		}

		while (byteCodeIndex < byteCode.size())
		{
			mStream.Write(byteCode[byteCodeIndex++]);
		}

		// Patch up the code size.
		const Stream::pos_t endPos = mStream.GetPosition();
		mStream.SetPosition(codeSizePos);
		WriteValue32(Value32(uint32_t(endPos - codeStartPos)));

		// Patch up the blob size.
		mStream.SetPosition(blobStartPos);
		WriteValue32(Value32(uint32_t(endPos - blobStartPos)));
		mStream.SetPosition(endPos);

		++numFunctions;
	}

	return numFunctions;
}


uint32_t GeneratorCore::WriteNativeMemberList(uint16_t functionIndex)
{
	for (const NativeMemberEntry &entry: mNativeMemberList)
	{
		const TypeDescriptor *typeDescriptor = entry.mNativeMember->GetTypeAndValue()->GetTypeDescriptor();

		// Write the getter function.
		{
			// Cache the blob start position and skip 4 bytes for the blob size.
			const int blobStartPos = mStream.GetPosition();
			mStream.AddOffset(sizeof(Value32));
			WriteValue16(Value16(functionIndex));
			WriteValue16(Value16(entry.mGetterNameIndex));
			WriteSizeAndType(typeDescriptor);
			WriteParamListSignature(nullptr, true);
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Frame size
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Packed frame size
			WriteValue32(Value32(0));              // Local size
			WriteValue32(Value32(0));              // Stack size
			WriteValue32(Value32(BOND_SLOT_SIZE)); // Frame pointer alignment
			WriteValue32(Value32(0));              // Code size

			// Patch up the blob size.
			const Stream::pos_t endPos = mStream.GetPosition();
			mStream.SetPosition(blobStartPos);
			WriteValue32(Value32(uint32_t(endPos - blobStartPos)));
			mStream.SetPosition(endPos);
		}

		// Write the setter function.
		{
			const uint32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), uint32_t(BOND_SLOT_SIZE));
			const uint32_t offset = AlignUp(uint32_t(BOND_SLOT_SIZE) + typeDescriptor->GetSize(mPointerSize), alignment);
			const uint32_t packedOffset = uint32_t(BOND_SLOT_SIZE) + typeDescriptor->GetStackSize(mPointerSize);
			const TypeDescriptor voidDescriptor = TypeDescriptor::GetVoidType();
			const Parameter parameter(typeDescriptor, -int32_t(offset));

			// Cache the blob start position and skip 4 bytes for the blob size.
			const int blobStartPos = mStream.GetPosition();
			mStream.AddOffset(sizeof(Value32));
			WriteValue16(Value16(functionIndex));
			WriteValue16(Value16(entry.mSetterNameIndex));
			WriteSizeAndType(&voidDescriptor);
			WriteParamListSignature(&parameter, true);
			WriteValue32(Value32(offset));         // Frame size
			WriteValue32(Value32(packedOffset));   // Packed frame size
			WriteValue32(Value32(0));              // Local size
			WriteValue32(Value32(0));              // Stack size
			WriteValue32(Value32(alignment));      // Frame pointer alignment
			WriteValue32(Value32(0));              // Code size

			// Patch up the blob size.
			const Stream::pos_t endPos = mStream.GetPosition();
			mStream.SetPosition(blobStartPos);
			WriteValue32(Value32(uint32_t(endPos - blobStartPos)));
			mStream.SetPosition(endPos);
		}
	}

	return uint32_t(2 * mNativeMemberList.size());
}


uint32_t GeneratorCore::WriteDataList(uint16_t dataIndex)
{
	for (const DataEntry &entry: mDataList)
	{
		const TypeAndValue *typeAndValue = entry.mData->GetTypeAndValue();
		const TypeDescriptor *typeDescriptor = typeAndValue->GetTypeDescriptor();

		// Cache the blob start position and skip 4 bytes for the blob size.
		const int blobStartPos = mStream.GetPosition();
		mStream.AddOffset(sizeof(Value32));

		WriteValue16(Value16(dataIndex));
		WriteValue16(Value16(entry.mNameIndex));
		WriteSizeAndType(typeDescriptor);

		Value32 payload(uint32_t(0));
		if (typeAndValue->IsValueDefined())
		{
			switch (typeDescriptor->GetSignatureType())
			{
				case SIG_BOOL:
					payload.mUInt = typeAndValue->GetBoolValue() ? uint32_t(1) : uint32_t(0);
					break;
				case SIG_CHAR:
				case SIG_SHORT:
				case SIG_INT:
					payload.mInt = typeAndValue->GetIntValue();
					break;
				case SIG_UCHAR:
				case SIG_USHORT:
				case SIG_UINT:
					payload.mUInt = typeAndValue->GetUIntValue();
					break;
				case SIG_FLOAT:
					payload.mFloat = typeAndValue->GetFloatValue();
					break;
				case SIG_LONG:
				case SIG_ULONG:
				case SIG_DOUBLE:
				{
					const Value64IndexMap::Type::const_iterator index = mValue64IndexMap.find(Value64(typeAndValue->GetULongValue()));
					if (index != mValue64IndexMap.end())
					{
						payload.mUInt = uint32_t(index->second);
					}
					else
					{
						payload.mUInt = BOND_UINT_MAX;
					}
				}
				break;
				default:
					break;
			}
		}
		else
		{
			switch (typeDescriptor->GetSignatureType())
			{
				case SIG_LONG:
				case SIG_ULONG:
				case SIG_DOUBLE:
					payload.mUInt = BOND_UINT_MAX;
				break;
				case SIG_POINTER:
				case SIG_AGGREGATE:
					payload.mUInt = typeDescriptor->GetAlignment(mPointerSize);
					break;
				default:
					break;
			}
		}

		WriteValue32(payload);

		// Patch up the blob size.
		const Stream::pos_t endPos = mStream.GetPosition();
		mStream.SetPosition(blobStartPos);
		WriteValue32(Value32(uint32_t(endPos - blobStartPos)));
		mStream.SetPosition(endPos);
	}

	return uint32_t(mDataList.size());
}


void GeneratorCore::WriteQualifiedName(const QualifiedNameEntry &entry)
{
	// Cache the position for the number of elements and skip 2 bytes.
	const Stream::pos_t startPos = mStream.GetPosition();
	mStream.AddOffset(sizeof(Value16));

	WriteQualifiedNameIndices(entry.mSymbol);

	if (!entry.mSuffix.IsEmpty())
	{
			const uint16_t suffixIndex = MapString(entry.mSuffix);
			WriteValue16(Value16(suffixIndex));
	}

	// Patch up the number of elements.
	const Stream::pos_t endPos = mStream.GetPosition();
	mStream.SetPosition(startPos);
	WriteValue16(Value16(uint16_t((endPos - startPos - sizeof(Value16)) / sizeof(Value16))));
	mStream.SetPosition(endPos);
}


void GeneratorCore::WriteQualifiedNameIndices(const Symbol *symbol)
{
	if (symbol != nullptr)
	{
		WriteQualifiedNameIndices(symbol->GetParentSymbol());
		const Token *name = symbol->GetName();
		if (name != nullptr)
		{
			const uint16_t nameIndex = MapString(name->GetText());
			WriteValue16(Value16(nameIndex));
		}
	}
}


void GeneratorCore::WriteString(const SimpleString &simpleString)
{
	const size_t length = simpleString.GetLength();
	const char *str = simpleString.GetString();
	WriteValue16(Value16(uint16_t(length)));
	for (size_t i = 0; i < length; ++i)
	{
		mStream.Write(str[i]);
	}
}


void GeneratorCore::WriteSizeAndType(const TypeDescriptor *type)
{
	const uint32_t sizeAndType = EncodeSizeAndType(type->GetSize(mPointerSize), type->GetSignatureType());
	WriteValue32(Value32(sizeAndType));
}


void GeneratorCore::WriteParamListSignature(const Parameter *parameterList, bool includeThis)
{
	// Cache the position for the number of parameters and skip 2 bytes.
	const Stream::pos_t startPos = mStream.GetPosition();
	mStream.AddOffset(sizeof(Value16));

	if (includeThis)
	{
		const uint32_t sizeAndType = EncodeSizeAndType(GetPointerSize(mPointerSize), SIG_POINTER);
		WriteValue32(Value32(-BOND_SLOT_SIZE));
		WriteValue32(Value32(sizeAndType));
	}

	while (parameterList != nullptr)
	{
		const TypeDescriptor *type = parameterList->GetTypeDescriptor();
		const int32_t offset = parameterList->GetOffset();
		const uint32_t sizeAndType = EncodeSizeAndType(type->GetSize(mPointerSize), type->GetSignatureType());
		WriteValue32(Value32(offset));
		WriteValue32(Value32(sizeAndType));
		parameterList = NextNode(parameterList);
	}

	// Patch up the number of parameters.
	const Stream::pos_t endPos = mStream.GetPosition();
	mStream.SetPosition(startPos);
	WriteValue16(Value16(uint16_t((endPos - startPos - sizeof(Value16)) / (2 * sizeof(Value32)))));
	mStream.SetPosition(endPos);
}


void GeneratorCore::WriteValue16(Value16 value)
{
	ConvertBigEndian16(value.mBytes);
	mStream.Write(value.mBytes[0]);
	mStream.Write(value.mBytes[1]);
}


void GeneratorCore::WriteValue32(Value32 value)
{
	ConvertBigEndian32(value.mBytes);
	mStream.Write(value.mBytes[0]);
	mStream.Write(value.mBytes[1]);
	mStream.Write(value.mBytes[2]);
	mStream.Write(value.mBytes[3]);
}


void GeneratorCore::WriteValue64(Value64 value)
{
	ConvertBigEndian64(value.mBytes);
	mStream.Write(value.mBytes[0]);
	mStream.Write(value.mBytes[1]);
	mStream.Write(value.mBytes[2]);
	mStream.Write(value.mBytes[3]);
	mStream.Write(value.mBytes[4]);
	mStream.Write(value.mBytes[5]);
	mStream.Write(value.mBytes[6]);
	mStream.Write(value.mBytes[7]);
}


void GeneratorCore::ApplyStackDelta(int32_t delta)
{
	CompiledFunction &function = GetFunction();
	uint32_t top = mStackTop.GetTop();

	// Check for stack underflow.
	if ((delta < 0) && (uint32_t(-delta) > top))
	{
		PushError(CompilerError::INTERNAL_ERROR);
	}

	const uint32_t stackTop = mStackTop.GetTop() + delta;
	mStackTop.SetTop(stackTop);
	function.mStackSize = Max(function.mStackSize, stackTop);
}


int32_t GeneratorCore::AllocateLocal(const TypeDescriptor* typeDescriptor)
{
	CompiledFunction &function = GetFunction();
	const uint32_t alignment = Max(typeDescriptor->GetAlignment(mPointerSize), uint32_t(BOND_SLOT_SIZE));
	const uint32_t size = typeDescriptor->GetSize(mPointerSize);
	const uint32_t offset = AlignUp(mLocalOffset.GetTop(), alignment);
	const uint32_t nextOffset = AlignUp(offset + size, uint32_t(BOND_SLOT_SIZE));
	mLocalOffset.SetTop(nextOffset);
	function.mLocalSize = Max(function.mLocalSize, nextOffset);
	function.mFramePointerAlignment = Max(function.mFramePointerAlignment, alignment);
	return int32_t(offset);
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


uint16_t GeneratorCore::MapQualifiedName(const Symbol *symbol, const char *suffix)
{
	if (!IsInRange<uint16_t>(mQualifiedNameList.size()))
	{
		PushError(CompilerError::STRING_TABLE_OVERFLOW);
	}

	const uint16_t index = uint16_t(mQualifiedNameList.size());
	const SimpleString simpleSuffix(suffix);
	QualifiedNameEntry entry(symbol, simpleSuffix);
	QualifiedNameIndexMap::InsertResult insertResult = mQualifiedNameIndexMap.emplace(entry, index);
	if (insertResult.second)
	{
		mQualifiedNameList.push_back(entry);

		const Symbol *sym = symbol;
		while (sym != nullptr)
		{
			const Token *name = sym->GetName();
			if (name != nullptr)
			{
				MapString(name->GetText());
			}
			sym = sym->GetParentSymbol();
		}

		if (!simpleSuffix.IsEmpty())
		{
			MapString(simpleSuffix);
		}
	}

	return insertResult.first->second;
}


uint16_t GeneratorCore::MapString(const SimpleString &str)
{
	if (!IsInRange<uint16_t>(mStringList.size()))
	{
		PushError(CompilerError::STRING_TABLE_OVERFLOW);
	}
	if (!IsInRange<uint16_t>(str.GetLength()))
	{
		PushError(CompilerError::STRING_OVERFLOW);
	}
	const uint16_t index = uint16_t(mStringList.size());
	StringIndexMap::InsertResult insertResult = mStringIndexMap.emplace(str, index);
	if (insertResult.second)
	{
		mStringList.push_back(str);
	}
	return insertResult.first->second;
}


uint16_t GeneratorCore::MapValue32(const Value32 &value)
{
	if (!IsInRange<uint16_t>(mValue32List.size()))
	{
		PushError(CompilerError::VALUE32_TABLE_OVERFLOW);
	}
	const uint16_t index = uint16_t(mValue32List.size());
	Value32IndexMap::InsertResult insertResult = mValue32IndexMap.emplace(value, index);
	if (insertResult.second)
	{
		mValue32List.push_back(value);
	}
	return insertResult.first->second;
}


uint16_t GeneratorCore::MapValue64(const Value64 &value)
{
	if (!IsInRange<uint16_t>(mValue64List.size()))
	{
		PushError(CompilerError::VALUE64_TABLE_OVERFLOW);
	}
	const uint16_t index = uint16_t(mValue64List.size());
	Value64IndexMap::InsertResult insertResult = mValue64IndexMap.emplace(value, index);
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

}
