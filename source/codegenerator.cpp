#include "bond/autostack.h"
#include "bond/binarywriter.h"
#include "bond/cboutil.h"
#include "bond/codegenerator.h"
#include "bond/compilererror.h"
#include "bond/endian.h"
#include "bond/list.h"
#include "bond/opcodes.h"
#include "bond/parsenodeutil.h"
#include "bond/parsenodetraverser.h"
#include "bond/map.h"
#include "bond/vector.h"
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

	struct GeneratorResult
	{
		enum Context
		{
			CONTEXT_NONE,
			CONTEXT_FP_INDIRECT,
			CONTEXT_FP_OFFSET,
			CONTEXT_STACK_VALUE,
			CONTEXT_ADDRESS_INDIRECT,
			CONTEXT_CONSTANT_VALUE,
		};

		GeneratorResult(): mContext(CONTEXT_NONE), mTypeAndValue(NULL), mOffset(0) {}
		GeneratorResult(Context context): mContext(context), mTypeAndValue(NULL), mOffset(0) {}
		GeneratorResult(Context context, bi32_t offset): mContext(context), mTypeAndValue(NULL), mOffset(offset) {}

		GeneratorResult(const TypeAndValue *typeAndValue):
			mContext(CONTEXT_CONSTANT_VALUE),
			mTypeAndValue(typeAndValue),
			mOffset(0)
		{}

		Context mContext;
		const TypeAndValue *mTypeAndValue;
		bi32_t mOffset;
	};

	struct JumpEntry
	{
		JumpEntry(size_t opCodePos, size_t fromPos, size_t toLabel):
			mOpCodePos(opCodePos),
			mFromPos(fromPos),
			mToLabel(toLabel)
		{}
		size_t mOpCodePos;
		size_t mFromPos;
		size_t mToLabel;
	};

	typedef Vector<bu8_t> ByteCode;
	typedef Vector<size_t> LabelList;
	typedef Vector<JumpEntry> JumpList;

	struct CompiledFunction
	{
		CompiledFunction(const FunctionDefinition *definition, Allocator &allocator):
			mDefinition(definition),
			mByteCode(ByteCode::Allocator(&allocator)),
			mLabelList(LabelList::Allocator(&allocator)),
			mJumpList(JumpList::Allocator(&allocator))
		{}
		const FunctionDefinition *mDefinition;
		ByteCode::Type mByteCode;
		LabelList::Type mLabelList;
		JumpList::Type mJumpList;
	};

	typedef Map<HashedString, bu16_t> StringIndexMap;
	typedef Map<Value32, bu16_t> Value32IndexMap;
	typedef Map<Value64, bu16_t> Value64IndexMap;
	typedef List<HashedString> StringList;
	typedef Vector<Value32> Value32List;
	typedef Vector<Value64> Value64List;
	typedef List<CompiledFunction> FunctionList;
	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<CompiledFunction *> FunctionStack;
	typedef AutoStack<GeneratorResult> ResultStack;
	typedef AutoStack<size_t> LabelStack;

	virtual void Traverse(const ParseNode *parseNode);
	void TraverseOmitOptionalTemporaries(const Expression *expression);

	virtual void Visit(const StructDeclaration *structDeclaration);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const NamedInitializer *namedInitializer);
	virtual void Visit(const IfStatement *ifStatement);
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

	void EmitPushResultAs(const GeneratorResult &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType);
	void EmitPushResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor);
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

	void EmitPopResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor);
	void EmitPopFramePointerIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPopFramePointerIndirectValue32(bi32_t offset);
	void EmitPopFramePointerIndirectValue64(bi32_t offset);
	void EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	GeneratorResult EmitAccumulateAddressOffset(const GeneratorResult &result);
	void EmitAccumulateAddressOffset(bi32_t offset);

	void EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType);

	GeneratorResult EmitCommaOperator(const BinaryExpression *binaryExpression);
	GeneratorResult EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);
	GeneratorResult EmitAssignmentOperator(const BinaryExpression *binaryExpression);
	GeneratorResult EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);
	void EmitLogicalOperator(const BinaryExpression *binaryExpression, OpCode branchOpCode);
	GeneratorResult EmitPointerArithmetic(const Expression *pointerExpression, const Expression *offsetExpression, int sign);

	GeneratorResult EmitPointerIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign);
	GeneratorResult EmitIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign);
	GeneratorResult EmitSignOperator(const UnaryExpression *unaryExpression);
	GeneratorResult EmitNotOperator(const UnaryExpression *unaryExpression);
	GeneratorResult EmitBitwiseNotOperator(const UnaryExpression *unaryExpression);
	GeneratorResult EmitAddressOfOperator(const UnaryExpression *unaryExpression);
	GeneratorResult EmitDereferenceOperator(const UnaryExpression *unaryExpression);

	void EmitJump(OpCode opCode, size_t toLabel);

	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitValue16(Value16 value);
	void EmitIndexedValue32(Value32 value);
	void EmitIndexedValue64(Value64 value);

	void WriteConstantTable();
	void WriteFunctionList(bu16_t functionIndex);
	void WriteQualifiedSymbolName(const Symbol *symbol);
	void WriteReturnSignature(const TypeDescriptor *type);
	void WriteParamListSignature(const Parameter *parameterList);
	void WriteSymbolNameIndices(const Symbol *symbol);
	void WriteValue16(Value16 value);
	void WriteValue32(Value32 value);
	void WriteValue64(Value64 value);

	bool Is64BitPointer() const { return mPointerSize == POINTER_64BIT; }
	ByteCode::Type &GetByteCode();
	size_t CreateLabel();
	void SetLabelValue(size_t label, size_t value);
	void MapQualifiedSymbolName(const Symbol *symbol);
	void MapLongJumpOffsets();
	bu16_t MapString(const HashedString &str);
	bu16_t MapValue32(const Value32 &value32);
	bu16_t MapValue64(const Value64 &value32);

	void PushError(CompilerError::Type type);

	StringIndexMap::Type mStringIndexMap;
	StringList::Type mStringList;
	Value32IndexMap::Type mValue32IndexMap;
	Value32List::Type mValue32List;
	Value64IndexMap::Type mValue64IndexMap;
	Value64List::Type mValue64List;
	FunctionList::Type mFunctionList;
	ResultStack mResult;
	StructStack mStruct;
	FunctionStack mFunction;
	LabelStack mContinueLabel;
	LabelStack mBreakLabel;
	BoolStack mEmitOptionalTemporaries;
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
	StructStack::Element structElement(mStruct, NULL);
	Traverse(mTranslationUnitList);

	WriteValue32(Value32(MAGIC_NUMBER));
	WriteValue16(Value16(MAJOR_VERSION));
	WriteValue16(Value16(MINOR_VERSION));
	WriteValue16(Value16(EncodePointerSize(0, mPointerSize)));

	const bu16_t listIndex = MapString("List");
	const bu16_t functionIndex = mFunctionList.empty() ? 0 : MapString("Func");

	MapLongJumpOffsets();
	WriteConstantTable();

	// Cache the start position and skip 4 bytes for the blob size.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value32));

	WriteValue16(Value16(listIndex));
	WriteValue32(Value32(/* mDefinitionList.size() + */ mFunctionList.size()));

	WriteFunctionList(functionIndex);

	// Patch up the blob size.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue32(Value32(endPos - startPos));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::Traverse(const ParseNode *parseNode)
{
	BoolStack::Element emitOptionalTemporariesElement(mEmitOptionalTemporaries, true);
	ParseNodeTraverser::Traverse(parseNode);
}


void GeneratorCore::TraverseOmitOptionalTemporaries(const Expression *expression)
{
	BoolStack::Element emitOptionalTemporariesElement(mEmitOptionalTemporaries, false);
	ParseNodeTraverser::Traverse(expression);

	// Remove any temporaries that may have been left on the stack.
	const GeneratorResult::Context context = mResult.GetTop().mContext;
	if (context == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
	{
		EmitOpCodeWithOffset(OPCODE_MOVESP, -BOND_SLOT_SIZE);
	}
	else if (context == GeneratorResult::CONTEXT_STACK_VALUE)
	{
		const bu32_t typeSize = expression->GetTypeDescriptor()->GetStackSize(mPointerSize);
		EmitOpCodeWithOffset(OPCODE_MOVESP, -static_cast<bi32_t>(typeSize));
	}
}


void GeneratorCore::Visit(const StructDeclaration *structDeclaration)
{
	if (structDeclaration->GetVariant() == StructDeclaration::VARIANT_BOND)
	{
		StructStack::Element structElement(mStruct, structDeclaration);
		ParseNodeTraverser::Visit(structDeclaration);
	}
}


void GeneratorCore::Visit(const FunctionDefinition *functionDefinition)
{
	if (!functionDefinition->IsNative())
	{
		CompiledFunction &function =
			*mFunctionList.insert(mFunctionList.end(), CompiledFunction(functionDefinition, mAllocator));
		FunctionStack::Element functionElement(mFunction, &function);
		MapQualifiedSymbolName(functionDefinition);
		ParseNodeTraverser::Visit(functionDefinition);

		if (functionDefinition->GetPrototype()->GetReturnType()->IsVoidType())
		{
			ByteCode::Type &byteCode = GetByteCode();
			byteCode.push_back(OPCODE_RETURN);
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
			const Initializer *initializer = namedInitializer->GetInitializer();
			// TODO: Omit code generation and stack allocation for non lvalue foldable constants.
			if ((initializer != NULL) && (initializer->GetExpression() != NULL))
			{
				const TypeDescriptor *lhDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
				const TypeDescriptor *rhDescriptor = initializer->GetExpression()->GetTypeDescriptor();
				ResultStack::Element rhResult(mResult);
				GeneratorResult lhResult(GeneratorResult::CONTEXT_FP_INDIRECT, namedInitializer->GetOffset());
				Traverse(initializer);
				EmitPushResultAs(rhResult.GetValue(), rhDescriptor, lhDescriptor);
				EmitPopResult(lhResult, lhDescriptor);
			}
			else
			{
			}
		}
		break;

		case SCOPE_STRUCT_MEMBER:
			// Nothing to do.
			break;
	}
	//ParseNodeTraverser::Visit(namedInitializer);
}


void GeneratorCore::Visit(const IfStatement *ifStatement)
{
	ByteCode::Type &byteCode = GetByteCode();
	const Expression *condition = ifStatement->GetCondition();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();
	const TypeAndValue &conditionTav = condition->GetTypeAndValue();

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
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

		const size_t thenEndLabel = CreateLabel();
		EmitJump(OPCODE_IFZ, thenEndLabel);

		Traverse(ifStatement->GetThenStatement());
		size_t thenEndPos = byteCode.size();

		if (ifStatement->GetElseStatement() != NULL)
		{
			size_t elseEndLabel = CreateLabel();
			EmitJump(OPCODE_GOTO, elseEndLabel);
			thenEndPos = byteCode.size();

			Traverse(ifStatement->GetElseStatement());
			SetLabelValue(elseEndLabel, byteCode.size());
		}

		SetLabelValue(thenEndLabel, thenEndPos);
	}
}


void GeneratorCore::Visit(const WhileStatement *whileStatement)
{
	ByteCode::Type &byteCode = GetByteCode();
	const size_t loopStartLabel = CreateLabel();
	const size_t loopEndLabel = CreateLabel();
	SetLabelValue(loopStartLabel, byteCode.size());

	LabelStack::Element continueElement(mContinueLabel, loopStartLabel);
	LabelStack::Element breakElement(mBreakLabel, loopEndLabel);

	const Expression *condition = whileStatement->GetCondition();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();

	if (whileStatement->IsDoLoop())
	{
		Traverse(whileStatement->GetBody());

		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);
		EmitJump(OPCODE_IFNZ, loopStartLabel);
	}
	else
	{
		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);
		EmitJump(OPCODE_IFZ, loopEndLabel);

		Traverse(whileStatement->GetBody());

		EmitJump(OPCODE_GOTO, loopStartLabel);
	}

	SetLabelValue(loopEndLabel, byteCode.size());
}


void GeneratorCore::Visit(const ForStatement *forStatement)
{
	Traverse(forStatement->GetInitializer());

	ByteCode::Type &byteCode = GetByteCode();
	const size_t loopStartLabel = CreateLabel();
	const size_t loopEndLabel = CreateLabel();
	SetLabelValue(loopStartLabel, byteCode.size());

	LabelStack::Element continueElement(mContinueLabel, loopStartLabel);
	LabelStack::Element breakElement(mBreakLabel, loopEndLabel);

	const Expression *condition = forStatement->GetCondition();
	if (condition != NULL)
	{
		const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();
		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);
		EmitJump(OPCODE_IFZ, loopEndLabel);
	}

	Traverse(forStatement->GetBody());

	ResultStack::Element countingResult(mResult);
	TraverseOmitOptionalTemporaries(forStatement->GetCountingExpression());

	EmitJump(OPCODE_GOTO, loopStartLabel);
	SetLabelValue(loopEndLabel, byteCode.size());
}


void GeneratorCore::Visit(const JumpStatement *jumpStatement)
{
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
		const TypeDescriptor *returnDescriptor = mFunction.GetTop()->mDefinition->GetPrototype()->GetReturnType();
		ByteCode::Type &byteCode = GetByteCode();
		OpCode opCode = OPCODE_RETURN;

		if (!returnDescriptor->IsVoidType())
		{
			const Expression *rhs = jumpStatement->GetRhs();
			const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
			ResultStack::Element rhResult(mResult);
			Traverse(rhs);
			EmitPushResultAs(rhResult.GetValue(), rhDescriptor, returnDescriptor);

			if (returnDescriptor->IsPointerType())
			{
				if (Is64BitPointer())
				{
					opCode = OPCODE_RETURN64;
				}
				else
				{
					opCode = OPCODE_RETURN32;
				}
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
						opCode = OPCODE_RETURN32;
						break;

					case Token::KEY_LONG:
					case Token::KEY_ULONG:
					case Token::KEY_DOUBLE:
						opCode = OPCODE_RETURN64;
						break;

					default:
						// TODO
						break;
				}
			}
		}
		byteCode.push_back(opCode);
	}
}


void GeneratorCore::Visit(const ExpressionStatement *expressionStatement)
{
	ResultStack::Element expressionResult(mResult);
	TraverseOmitOptionalTemporaries(expressionStatement->GetExpression());
}


void GeneratorCore::Visit(const ConditionalExpression *conditionalExpression)
{
	ByteCode::Type &byteCode = GetByteCode();
	const Expression *condition = conditionalExpression->GetCondition();
	const Expression *trueExpression = conditionalExpression->GetTrueExpression();
	const Expression *falseExpression = conditionalExpression->GetFalseExpression();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeDescriptor();
	const TypeDescriptor *trueDescriptor = trueExpression->GetTypeDescriptor();
	const TypeDescriptor *falseDescriptor = falseExpression->GetTypeDescriptor();
	const TypeDescriptor *resultDescriptor = conditionalExpression->GetTypeDescriptor();

	{
		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

		const size_t trueEndLabel = CreateLabel();
		EmitJump(OPCODE_IFZ, trueEndLabel);

		ResultStack::Element trueResult(mResult);
		Traverse(trueExpression);
		EmitPushResultAs(trueResult.GetValue(), trueDescriptor, resultDescriptor);

		const size_t falseEndLabel = CreateLabel();
		EmitJump(OPCODE_GOTO, falseEndLabel);
		SetLabelValue(trueEndLabel, byteCode.size());

		ResultStack::Element falseResult(mResult);
		Traverse(falseExpression);
		EmitPushResultAs(falseResult.GetValue(), falseDescriptor, resultDescriptor);

		SetLabelValue(falseEndLabel, byteCode.size());
	}

	mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE));
}


void GeneratorCore::Visit(const BinaryExpression *binaryExpression)
{
	if (!ProcessConstantExpression(binaryExpression))
	{
		const Expression *lhs = binaryExpression->GetLhs();
		const Expression *rhs = binaryExpression->GetRhs();
		const TypeAndValue &lhTav = lhs->GetTypeAndValue();
		const TypeAndValue &rhTav = rhs->GetTypeAndValue();
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
		const Token *op = binaryExpression->GetOperator();

		GeneratorResult result = GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE);

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
					// TODO: Pointer arithmetic.
				}
				else
				{
					result = EmitCompoundAssignmentOperator(binaryExpression, ADD_OPCODES);
				}
				break;

			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
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
				EmitLogicalOperator(binaryExpression, OPCODE_BRZ);
				break;
			case Token::OP_OR:
				EmitLogicalOperator(binaryExpression, OPCODE_BRNZ);
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
				result = EmitSimpleBinaryOperator(binaryExpression, CMPEQ_OPCODES);
				break;
			case Token::OP_NOT_EQUAL:
				result = EmitSimpleBinaryOperator(binaryExpression, CMPNEQ_OPCODES);
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
				if (lhDescriptor->IsPointerType())
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
		const TypeAndValue &rhTav = rhs->GetTypeAndValue();
		const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
		const Token *op = unaryExpression->GetOperator();
		GeneratorResult result;

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
	const TypeAndValue &lhTav = lhs->GetTypeAndValue();
	const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
	const Token *op = postfixExpression->GetOperator();
	GeneratorResult result;

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
	// TODO
	ParseNodeTraverser::Visit(memberExpression);
}


void GeneratorCore::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	// TODO
	ParseNodeTraverser::Visit(arraySubscriptExpression);
}


void GeneratorCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	// TODO
	ParseNodeTraverser::Visit(functionCallExpression);
}


void GeneratorCore::Visit(const CastExpression *castExpression)
{
	if (!ProcessConstantExpression(castExpression))
	{
		// TODO
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
			switch (namedInitializer->GetScope())
			{
				case SCOPE_GLOBAL:
					// TODO
					break;
				case SCOPE_LOCAL:
					if (namedInitializer->GetTypeAndValue()->GetTypeDescriptor()->IsArrayType())
					{
						mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_OFFSET, namedInitializer->GetOffset()));
					}
					else
					{
						mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_INDIRECT, namedInitializer->GetOffset()));
					}
					break;
				case SCOPE_STRUCT_MEMBER:
					// TODO
					break;
			}
		}
		else if ((parameter = CastNode<Parameter>(symbol)) != NULL)
		{
			mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_INDIRECT, parameter->GetOffset()));
		}
		else if ((functionDefinition = CastNode<FunctionDefinition>(symbol)) != NULL)
		{
			// TODO
		}
	}
}


void GeneratorCore::Visit(const ThisExpression *thisExpression)
{
	mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_INDIRECT, -BOND_SLOT_SIZE));
}


bool GeneratorCore::ProcessConstantExpression(const Expression *expression)
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsValueDefined())
	{
		mResult.SetTop(GeneratorResult(&tav));
		return true;
	}
	return false;
}


void GeneratorCore::EmitPushResultAs(const GeneratorResult &result, const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_INDIRECT:
			EmitPushFramePointerIndirectValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case GeneratorResult::CONTEXT_FP_OFFSET:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case GeneratorResult::CONTEXT_ADDRESS_INDIRECT:
			EmitPushAddressIndirectValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			EmitPushConstantAs(*result.mTypeAndValue, destType);
			break;

		case GeneratorResult::CONTEXT_STACK_VALUE:
			EmitPushStackValue(sourceType, result.mOffset);
			EmitCast(sourceType, destType);
			break;

		case GeneratorResult::CONTEXT_NONE:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}
}


void GeneratorCore::EmitPushResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_INDIRECT:
			EmitPushFramePointerIndirectValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_FP_OFFSET:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_ADDRESS_INDIRECT:
			EmitPushAddressIndirectValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			EmitPushConstant(*result.mTypeAndValue);
			break;

		case GeneratorResult::CONTEXT_STACK_VALUE:
			EmitPushStackValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_NONE:
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
				// TODO: Deal with non-primitive values.
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
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH32, offset);
			break;
	}
}


void GeneratorCore::EmitPushFramePointerIndirectValue64(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_PUSH64_L3);
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
	ByteCode::Type &byteCode = GetByteCode();
	EmitAccumulateAddressOffset(offset);

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_UCHAR:
				byteCode.push_back(OPCODE_LOADUC);
				break;
			case Token::KEY_CHAR:
				byteCode.push_back(OPCODE_LOADUC);
				break;
			case Token::KEY_SHORT:
				byteCode.push_back(OPCODE_LOADS);
				break;
			case Token::KEY_USHORT:
				byteCode.push_back(OPCODE_LOADUS);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				byteCode.push_back(OPCODE_LOAD32);
				break;
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_DOUBLE:
				byteCode.push_back(OPCODE_LOAD64);
				break;
			default:
				// TODO: Deal with non-primitive values.
				break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		if (Is64BitPointer())
		{
			byteCode.push_back(OPCODE_LOAD64);
		}
		else
		{
			byteCode.push_back(OPCODE_LOAD32);
		}
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
	ByteCode::Type &byteCode = GetByteCode();
	const TypeDescriptor *typeDescriptor = typeAndValue.GetTypeDescriptor();
	// TODO: Deal with string literals.
	switch (typeDescriptor->GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			byteCode.push_back(typeAndValue.GetBoolValue() ? OPCODE_CONSTI_1 : OPCODE_CONSTI_0);
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
				byteCode.push_back(Is64BitPointer() ? OPCODE_CONSTL_0 : OPCODE_CONSTI_0);
			}
			// TODO: Deal with non-primitive values.
			break;
	}
}


void GeneratorCore::EmitPushConstantInt(bi32_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (value)
	{
		case -2:
			byteCode.push_back(OPCODE_CONSTI_N2);
			break;
		case -1:
			byteCode.push_back(OPCODE_CONSTI_N1);
			break;
		case 0:
			byteCode.push_back(OPCODE_CONSTI_0);
			break;
		case 1:
			byteCode.push_back(OPCODE_CONSTI_1);
			break;
		case 2:
			byteCode.push_back(OPCODE_CONSTI_2);
			break;
		case 3:
			byteCode.push_back(OPCODE_CONSTI_3);
			break;
		case 4:
			byteCode.push_back(OPCODE_CONSTI_4);
			break;
		case 5:
			byteCode.push_back(OPCODE_CONSTI_5);
			break;
		case 6:
			byteCode.push_back(OPCODE_CONSTI_6);
			break;
		case 7:
			byteCode.push_back(OPCODE_CONSTI_7);
			break;
		case 8:
			byteCode.push_back(OPCODE_CONSTI_8);
			break;
		default:
			if (IsInCharRange(value))
			{
				byteCode.push_back(OPCODE_CONSTC);
				byteCode.push_back(static_cast<bu8_t>(value));
			}
			else if (IsInUCharRange(value))
			{
				byteCode.push_back(OPCODE_CONSTUC);
				byteCode.push_back(static_cast<bu8_t>(value));
			}
			else if (IsInShortRange(value))
			{
				byteCode.push_back(OPCODE_CONSTS);
				EmitValue16(Value16(value));
			}
			else if (IsInUShortRange(value))
			{
				byteCode.push_back(OPCODE_CONSTUS);
				EmitValue16(Value16(value));
			}
			else
			{
				byteCode.push_back(OPCODE_CONST32);
				EmitIndexedValue32(Value32(value));
			}
			break;
	}
}


void GeneratorCore::EmitPushConstantUInt(bu32_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (value)
	{
		case 0:
			byteCode.push_back(OPCODE_CONSTI_0);
			break;
		case 1:
			byteCode.push_back(OPCODE_CONSTI_1);
			break;
		case 2:
			byteCode.push_back(OPCODE_CONSTI_2);
			break;
		case 3:
			byteCode.push_back(OPCODE_CONSTI_3);
			break;
		case 4:
			byteCode.push_back(OPCODE_CONSTI_4);
			break;
		case 5:
			byteCode.push_back(OPCODE_CONSTI_5);
			break;
		case 6:
			byteCode.push_back(OPCODE_CONSTI_6);
			break;
		case 7:
			byteCode.push_back(OPCODE_CONSTI_7);
			break;
		case 8:
			byteCode.push_back(OPCODE_CONSTI_8);
			break;
		default:
			if (IsInUCharRange(value))
			{
				byteCode.push_back(OPCODE_CONSTUC);
				byteCode.push_back(static_cast<bu8_t>(value));
			}
			else if (IsInUShortRange(value))
			{
				byteCode.push_back(OPCODE_CONSTUS);
				EmitValue16(Value16(value));
			}
			else
			{
				byteCode.push_back(OPCODE_CONST32);
				EmitIndexedValue32(Value32(value));
			}
			break;
	}
}


void GeneratorCore::EmitPushConstantLong(bi64_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (value)
	{
		case -1:
			byteCode.push_back(OPCODE_CONSTL_N1);
			break;
		case 0:
			byteCode.push_back(OPCODE_CONSTL_0);
			break;
		case 1:
			byteCode.push_back(OPCODE_CONSTL_1);
			break;
		default:
		{
			byteCode.push_back(OPCODE_CONST64);
			EmitIndexedValue64(Value64(value));
		}
		break;
	}
}


void GeneratorCore::EmitPushConstantULong(bu64_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (value)
	{
		case 0:
			byteCode.push_back(OPCODE_CONSTL_0);
			break;
		case 1:
			byteCode.push_back(OPCODE_CONSTL_1);
			break;
		default:
		{
			byteCode.push_back(OPCODE_CONST64);
			EmitIndexedValue64(Value64(value));
		}
		break;
	}
}


void GeneratorCore::EmitPushConstantFloat(bf32_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	if (value == -2.0f)
	{
		byteCode.push_back(OPCODE_CONSTF_N2);
	}
	else if (value == -1.0f)
	{
		byteCode.push_back(OPCODE_CONSTF_N1);
	}
	else if (value == -0.5f)
	{
		byteCode.push_back(OPCODE_CONSTF_NH);
	}
	else if (value == 0.0f)
	{
		byteCode.push_back(OPCODE_CONSTF_0);
	}
	else if (value == 0.5f)
	{
		byteCode.push_back(OPCODE_CONSTF_H);
	}
	else if (value == 1.0f)
	{
		byteCode.push_back(OPCODE_CONSTF_1);
	}
	else if (value == 2.0f)
	{
		byteCode.push_back(OPCODE_CONSTF_2);
	}
	else
	{
		byteCode.push_back(OPCODE_CONST32);
		EmitIndexedValue32(Value32(value));
	}
}


void GeneratorCore::EmitPushConstantDouble(bf64_t value)
{
	ByteCode::Type &byteCode = GetByteCode();
	if (value == -2.0)
	{
		byteCode.push_back(OPCODE_CONSTD_N2);
	}
	else if (value == -1.0)
	{
		byteCode.push_back(OPCODE_CONSTD_N1);
	}
	else if (value == -0.5)
	{
		byteCode.push_back(OPCODE_CONSTD_NH);
	}
	else if (value == 0.0)
	{
		byteCode.push_back(OPCODE_CONSTD_0);
	}
	else if (value == 0.5)
	{
		byteCode.push_back(OPCODE_CONSTD_H);
	}
	else if (value == 1.0)
	{
		byteCode.push_back(OPCODE_CONSTD_1);
	}
	else if (value == 2.0)
	{
		byteCode.push_back(OPCODE_CONSTD_2);
	}
	else
	{
		byteCode.push_back(OPCODE_CONST64);
		EmitIndexedValue64(Value64(value));
	}
}


void GeneratorCore::EmitPopResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_INDIRECT:
			EmitPopFramePointerIndirectValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_ADDRESS_INDIRECT:
			EmitPopAddressIndirectValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_FP_OFFSET:
		case GeneratorResult::CONTEXT_STACK_VALUE:
		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
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
				// TODO: Deal with non-primitive values.
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
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP32, offset);
			break;
	}
}


void GeneratorCore::EmitPopFramePointerIndirectValue64(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -4 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_P3);
			break;
		case -3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_P2);
			break;
		case -2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_P1);
			break;
		case -1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_P0);
			break;
		case 0 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_L0);
			break;
		case 1 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_L1);
			break;
		case 2 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_L2);
			break;
		case 3 * BOND_SLOT_SIZE:
			byteCode.push_back(OPCODE_POP64_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP64, offset);
			break;
	}
}


void GeneratorCore::EmitPopAddressIndirectValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	EmitAccumulateAddressOffset(offset);

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
			case Token::KEY_UCHAR:
				byteCode.push_back(OPCODE_STOREC);
				break;
			case Token::KEY_SHORT:
			case Token::KEY_USHORT:
				byteCode.push_back(OPCODE_STORES);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_LONG:
			case Token::KEY_ULONG:
			case Token::KEY_FLOAT:
				byteCode.push_back(OPCODE_STORE32);
				break;
			case Token::KEY_DOUBLE:
				byteCode.push_back(OPCODE_STORE64);
				break;
			default:
				// TODO: Deal with non-primitive values.
				break;
		}
	}
	else if (typeDescriptor->IsPointerType())
	{
		if (Is64BitPointer())
		{
			byteCode.push_back(OPCODE_STORE64);
		}
		else
		{
			byteCode.push_back(OPCODE_STORE32);
		}
	}
}


GeneratorCore::GeneratorResult GeneratorCore::EmitAccumulateAddressOffset(const GeneratorResult &result)
{
	GeneratorResult output = result;
	if (result.mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
	{
		EmitAccumulateAddressOffset(result.mOffset);
		output.mOffset = 0;
	}
	return output;
}


void GeneratorCore::EmitAccumulateAddressOffset(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	if (offset != 0)
	{
		EmitPushConstantInt(offset);
		if (Is64BitPointer())
		{
			byteCode.push_back(OPCODE_ITOL);
			byteCode.push_back(OPCODE_ADDL);
		}
		else
		{
			byteCode.push_back(OPCODE_ADDI);
		}
	}
}


void GeneratorCore::EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (destType->GetPrimitiveType())
	{
		case Token::KEY_CHAR:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_SHORT:
				case Token::KEY_USHORT:
				case Token::KEY_INT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_ITOC);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_LTOI);
					byteCode.push_back(OPCODE_ITOC);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
					byteCode.push_back(OPCODE_ITOC);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOI);
					byteCode.push_back(OPCODE_ITOC);
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
					byteCode.push_back(OPCODE_UITOUC);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_LTOI);
					byteCode.push_back(OPCODE_UITOUC);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUI);
					byteCode.push_back(OPCODE_UITOUC);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOUI);
					byteCode.push_back(OPCODE_UITOUC);
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
					byteCode.push_back(OPCODE_ITOS);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_LTOI);
					byteCode.push_back(OPCODE_ITOS);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
					byteCode.push_back(OPCODE_ITOS);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOI);
					byteCode.push_back(OPCODE_ITOS);
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
					byteCode.push_back(OPCODE_UITOUS);
					break;
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_LTOI);
					byteCode.push_back(OPCODE_UITOUS);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUI);
					byteCode.push_back(OPCODE_UITOUS);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOUI);
					byteCode.push_back(OPCODE_UITOUS);
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
					byteCode.push_back(OPCODE_LTOI);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOI);
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
					byteCode.push_back(OPCODE_LTOI);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUI);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOUI);
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
					byteCode.push_back(OPCODE_ITOL);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_UITOUL);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOL);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOL);
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
					byteCode.push_back(OPCODE_ITOL);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_UITOUL);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUL);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_DTOUL);
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
					byteCode.push_back(OPCODE_ITOF);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_UITOF);
					break;
				case Token::KEY_LONG:
					byteCode.push_back(OPCODE_LTOF);
					break;
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_ULTOF);
					break;
				case Token::KEY_DOUBLE:
					byteCode.push_back(OPCODE_FTOD);
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
					byteCode.push_back(OPCODE_ITOD);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_UITOD);
					break;
				case Token::KEY_LONG:
					byteCode.push_back(OPCODE_LTOD);
					break;
				case Token::KEY_ULONG:
					byteCode.push_back(OPCODE_ULTOD);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_DTOF);
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}
}


GeneratorCore::GeneratorResult GeneratorCore::EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResultAs(lhResult.GetValue(), lhDescriptor, &resultDescriptor);

	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResultAs(rhResult.GetValue(), rhDescriptor, &resultDescriptor);

	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(opCodeSet.GetOpCode(resultDescriptor));

	return GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitAssignmentOperator(const BinaryExpression *binaryExpression)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	GeneratorResult result;

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);

	OpCode valueDupOpCode = OPCODE_DUP;
	if (lhResult.GetValue().mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
	{
		lhResult = EmitAccumulateAddressOffset(lhResult);
		valueDupOpCode = OPCODE_DUPINS;
	}

	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResultAs(rhResult.GetValue(), rhDescriptor, lhDescriptor);

	if (mEmitOptionalTemporaries.GetTop())
	{
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(valueDupOpCode);
		result.mContext = GeneratorResult::CONTEXT_STACK_VALUE;
	}

	EmitPopResult(lhResult, lhDescriptor);
	return result;
}


GeneratorCore::GeneratorResult GeneratorCore::EmitCommaOperator(const BinaryExpression *binaryExpression)
{
	TraverseOmitOptionalTemporaries(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
	return mResult.GetTop();
}


GeneratorCore::GeneratorResult GeneratorCore::EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);
	const Token::TokenType lhType = lhDescriptor->GetPrimitiveType();
	const Token::TokenType rhType = rhDescriptor->GetPrimitiveType();
	const TypeAndValue &rhTav = rhs->GetTypeAndValue();
	ByteCode::Type &byteCode = GetByteCode();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	const bi32_t slotIndex = lhResult.GetValue().mOffset / BOND_SLOT_SIZE;

	if (((&opCodeSet == &ADD_OPCODES) || (&opCodeSet == &SUB_OPCODES)) &&
	    (lhResult.GetValue().mContext == GeneratorResult::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((lhResult.GetValue().mOffset % BOND_SLOT_SIZE) == 0) &&
	    LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(lhType) &&
	    MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(rhType) &&
	    rhTav.IsValueDefined() &&
	    IsInCharRange(rhTav.GetIntValue()))
	{
		const bi32_t value = rhTav.GetIntValue() * ((&opCodeSet == &SUB_OPCODES) ? -1 : 1);
		byteCode.push_back(INC_OPCODES.GetOpCode(lhType));
		byteCode.push_back(static_cast<bu8_t>(slotIndex));
		byteCode.push_back(static_cast<bu8_t>(value));

		if (mEmitOptionalTemporaries.GetTop())
		{
			EmitPushResult(lhResult.GetValue(), lhDescriptor);
		}
	}

	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (lhResult.GetValue().mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
		{
			lhResult = EmitAccumulateAddressOffset(lhResult);
			byteCode.push_back(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResultAs(lhResult.GetValue(), lhDescriptor, &resultDescriptor);

		ResultStack::Element rhResult(mResult);
		Traverse(rhs);
		EmitPushResultAs(rhResult.GetValue(), rhDescriptor, &resultDescriptor);

		byteCode.push_back(opCodeSet.GetOpCode(resultDescriptor));
		EmitCast(&resultDescriptor, lhDescriptor);

		if (mEmitOptionalTemporaries.GetTop())
		{
			byteCode.push_back(valueDupOpCode);
		}

		EmitPopResult(lhResult, lhDescriptor);
	}

	return GeneratorResult(mEmitOptionalTemporaries.GetTop() ?
		GeneratorResult::CONTEXT_STACK_VALUE :
		GeneratorResult::CONTEXT_NONE);
}


void GeneratorCore::EmitLogicalOperator(const BinaryExpression *binaryExpression, OpCode branchOpCode)
{
	// TODO: Collapse ! operators.
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeDescriptor();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult.GetValue(), lhDescriptor);

	const size_t endLabel = CreateLabel();
	EmitJump(branchOpCode, endLabel);

	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	SetLabelValue(endLabel, GetByteCode().size());
}


GeneratorCore::GeneratorResult GeneratorCore::EmitPointerArithmetic(const Expression *pointerExpression, const Expression *offsetExpression, int sign)
{
	const TypeDescriptor *pointerDescriptor = pointerExpression->GetTypeDescriptor();
	const bi32_t elementSize = static_cast<bi32_t>(sign * pointerDescriptor->GetDereferencedType().GetSize(mPointerSize));
	const TypeAndValue &offsetTav = offsetExpression->GetTypeAndValue();
	const TypeDescriptor *offsetDescriptor = offsetTav.GetTypeDescriptor();

	ResultStack::Element pointerResult(mResult);
	Traverse(pointerExpression);

	GeneratorResult result(GeneratorResult::CONTEXT_STACK_VALUE);

	if (offsetTav.IsValueDefined() &&
	    MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(offsetDescriptor->GetPrimitiveType()))
	{
		if ((pointerResult.GetValue().mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT) ||
		    (pointerResult.GetValue().mContext == GeneratorResult::CONTEXT_FP_INDIRECT))
		{
			EmitPushResult(pointerResult.GetValue(), pointerDescriptor);
		}
		else
		{
			result = pointerResult;
		}

		result.mOffset += (offsetTav.GetIntValue() * elementSize);
	}
	else
	{
		EmitPushResult(pointerResult.GetValue(), pointerDescriptor);

		ResultStack::Element offsetResult(mResult);
		Traverse(offsetExpression);

		ByteCode::Type &byteCode = GetByteCode();
		if (IsInShortRange(elementSize))
		{
			const TypeDescriptor intTypeDescriptor = TypeDescriptor::GetIntType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &intTypeDescriptor);
			byteCode.push_back(OPCODE_PTROFF);
			EmitValue16(Value16(elementSize));
		}
		else if (Is64BitPointer())
		{
			const TypeDescriptor longTypeDescriptor = TypeDescriptor::GetLongType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &longTypeDescriptor);
			EmitPushConstantLong(static_cast<bu64_t>(elementSize));
			byteCode.push_back(OPCODE_MULL);
			byteCode.push_back(OPCODE_ADDL);
		}
		else
		{
			const TypeDescriptor intTypeDescriptor = TypeDescriptor::GetIntType();
			EmitPushResultAs(offsetResult, offsetDescriptor, &intTypeDescriptor);
			EmitPushConstantInt(elementSize);
			byteCode.push_back(OPCODE_MULI);
			byteCode.push_back(OPCODE_ADDI);
		}
	}

	return result;
}


GeneratorCore::GeneratorResult GeneratorCore::EmitPointerIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign)
{
	ByteCode::Type &byteCode = GetByteCode();
	const TypeDescriptor *operandDescriptor = operand->GetTypeDescriptor();
	const bi32_t pointerOffset = static_cast<bi32_t>(sign * operandDescriptor->GetDereferencedType().GetSize(mPointerSize)) * sign;

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const bi32_t frameOffset = operandResult.GetValue().mOffset;
	const bi32_t slotIndex = frameOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == GeneratorResult::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((frameOffset % BOND_SLOT_SIZE) == 0) &&
	    IsInCharRange(pointerOffset))
	{
		if (Is64BitPointer())
		{
			if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
			{
				EmitPushFramePointerIndirectValue64(frameOffset);
			}

			byteCode.push_back(OPCODE_INCL);
			byteCode.push_back(static_cast<bu8_t>(slotIndex));
			byteCode.push_back(static_cast<bu8_t>(pointerOffset));

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

			byteCode.push_back(OPCODE_INCI);
			byteCode.push_back(static_cast<bu8_t>(slotIndex));
			byteCode.push_back(static_cast<bu8_t>(pointerOffset));

			if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
			{
				EmitPushFramePointerIndirectValue32(frameOffset);
			}
		}
	}
	else
	{
		OpCode valueDupOpCode = OPCODE_DUP;
		if (operandResult.GetValue().mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
		{
			operandResult = EmitAccumulateAddressOffset(operandResult);
			byteCode.push_back(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResult(operandResult.GetValue(), operandDescriptor);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			byteCode.push_back(valueDupOpCode);
		}

		if (Is64BitPointer())
		{
			EmitPushConstantLong(static_cast<bi64_t>(pointerOffset));
			byteCode.push_back(OPCODE_ADDL);
		}
		else
		{
			EmitPushConstantInt(pointerOffset);
			byteCode.push_back(OPCODE_ADDI);
		}

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			byteCode.push_back(valueDupOpCode);
		}

		EmitPopResult(operandResult, operandDescriptor);
	}

	return GeneratorResult(mEmitOptionalTemporaries.GetTop() ?
		GeneratorResult::CONTEXT_STACK_VALUE :
		GeneratorResult::CONTEXT_NONE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitIncrementOperator(const Expression *expression, const Expression *operand, Fixedness fixedness, int sign)
{
	ByteCode::Type &byteCode = GetByteCode();
	const TypeDescriptor *operandDescriptor = operand->GetTypeDescriptor();
	const TypeDescriptor *resultDescriptor = expression->GetTypeDescriptor();
	const Token::TokenType operandType = operandDescriptor->GetPrimitiveType();
	const Token::TokenType resultType = resultDescriptor->GetPrimitiveType();

	ResultStack::Element operandResult(mResult);
	Traverse(operand);
	const bi32_t slotIndex = operandResult.GetValue().mOffset / BOND_SLOT_SIZE;

	if ((operandResult.GetValue().mContext == GeneratorResult::CONTEXT_FP_INDIRECT) &&
	    IsInUCharRange(slotIndex) &&
	    ((operandResult.GetValue().mOffset % BOND_SLOT_SIZE) == 0) &&
	    LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(resultType) &&
	    (operandType == resultType))
	{
		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			EmitPushResult(operandResult.GetValue(), operandDescriptor);
		}

		byteCode.push_back(INC_OPCODES.GetOpCode(resultType));
		byteCode.push_back(static_cast<bu8_t>(slotIndex));
		byteCode.push_back(static_cast<bu8_t>(sign));

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			EmitPushResult(operandResult.GetValue(), operandDescriptor);
		}
	}
	else
	{
		const OpCodeSet &constOpCodeSet = (sign > 0) ? CONST1_OPCODES : CONSTN1_OPCODES;
		OpCode valueDupOpCode = OPCODE_DUP;
		if (operandResult.GetValue().mContext == GeneratorResult::CONTEXT_ADDRESS_INDIRECT)
		{
			operandResult = EmitAccumulateAddressOffset(operandResult);
			byteCode.push_back(OPCODE_DUP);
			valueDupOpCode = OPCODE_DUPINS;
		}

		EmitPushResultAs(operandResult.GetValue(), operandDescriptor, resultDescriptor);

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == POSTFIX))
		{
			byteCode.push_back(valueDupOpCode);
		}

		byteCode.push_back(constOpCodeSet.GetOpCode(resultType));
		byteCode.push_back(ADD_OPCODES.GetOpCode(resultType));

		if (mEmitOptionalTemporaries.GetTop() && (fixedness == PREFIX))
		{
			byteCode.push_back(valueDupOpCode);
		}

		EmitPopResult(operandResult, operandDescriptor);
	}

	return GeneratorResult(mEmitOptionalTemporaries.GetTop() ?
		GeneratorResult::CONTEXT_STACK_VALUE :
		GeneratorResult::CONTEXT_NONE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitSignOperator(const UnaryExpression *unaryExpression)
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
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	if (negated)
	{
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(NEG_OPCODES.GetOpCode(*resultDescriptor));
	}

	return GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitNotOperator(const UnaryExpression *unaryExpression)
{
	const UnaryExpression *unary = unaryExpression;
	const Expression *rhs = unaryExpression;
	bool negated = false;
	while (unary != NULL)
	{
		const Token::TokenType op = unary->GetOperator()->GetTokenType();
		if (op == Token::OP_NOT)
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
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	if (negated)
	{
		ByteCode::Type &byteCode = GetByteCode();
		byteCode.push_back(OPCODE_NOT);
	}

	return GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitBitwiseNotOperator(const UnaryExpression *unaryExpression)
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
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	if (negated)
	{
		if (resultDescriptor->GetPrimitiveType() == Token::KEY_LONG)
		{
			ByteCode::Type &byteCode = GetByteCode();
			byteCode.push_back(OPCODE_CONSTL_1);
			byteCode.push_back(OPCODE_XORL);
		}
		else
		{
			ByteCode::Type &byteCode = GetByteCode();
			byteCode.push_back(OPCODE_CONSTI_1);
			byteCode.push_back(OPCODE_XORI);
		}
	}

	return GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE);
}


GeneratorCore::GeneratorResult GeneratorCore::EmitAddressOfOperator(const UnaryExpression *unaryExpression)
{
	// TODO: Non-literal constants must not be resolved to constants. Must be FP indirect, for example.
	const Expression *rhs = unaryExpression->GetRhs();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	GeneratorResult result = rhResult;

	switch (rhResult.GetValue().mContext)
	{
		case GeneratorResult::CONTEXT_FP_INDIRECT:
			result.mContext = GeneratorResult::CONTEXT_FP_OFFSET;
			break;

		case GeneratorResult::CONTEXT_ADDRESS_INDIRECT:
			result.mContext = GeneratorResult::CONTEXT_STACK_VALUE;
			break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_FP_OFFSET:
		case GeneratorResult::CONTEXT_STACK_VALUE:
		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}

	return result;
}


GeneratorCore::GeneratorResult GeneratorCore::EmitDereferenceOperator(const UnaryExpression *unaryExpression)
{
	// TODO: Non-literal constants must not be resolved to constants. Must be FP indirect, for example.
	const Expression *rhs = unaryExpression->GetRhs();
	ResultStack::Element rhResult(mResult);
	Traverse(rhs);
	GeneratorResult result = rhResult;

	switch (rhResult.GetValue().mContext)
	{
		case GeneratorResult::CONTEXT_FP_OFFSET:
			result.mContext = GeneratorResult::CONTEXT_FP_INDIRECT;
			break;

		case GeneratorResult::CONTEXT_STACK_VALUE:
			result.mContext = GeneratorResult::CONTEXT_ADDRESS_INDIRECT;
			break;

		case GeneratorResult::CONTEXT_FP_INDIRECT:
		case GeneratorResult::CONTEXT_ADDRESS_INDIRECT:
			EmitPushResult(rhResult, rhs->GetTypeDescriptor());
			result = GeneratorResult(GeneratorResult::CONTEXT_ADDRESS_INDIRECT);
			break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			PushError(CompilerError::INTERNAL_ERROR);
			break;
	}

	return result;
}


void GeneratorCore::EmitJump(OpCode opCode, size_t toLabel)
{
	CompiledFunction *function = mFunction.GetTop();
	ByteCode::Type &byteCode = function->mByteCode;
	const size_t opCodePos = byteCode.size();
	byteCode.push_back(opCode);
	byteCode.push_back(0);
	byteCode.push_back(0);
	const size_t fromPos = byteCode.size();
	function->mJumpList.push_back(JumpEntry(opCodePos, fromPos, toLabel));
}


void GeneratorCore::EmitOpCodeWithOffset(OpCode opCode, bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	if (IsInShortRange(offset))
	{
		byteCode.push_back(opCode);
		EmitValue16(Value16(offset));
	}
	else
	{
		byteCode.push_back(opCode + 1);
		EmitIndexedValue32(Value32(offset));
	}
}


void GeneratorCore::EmitValue16(Value16 value)
{
	ConvertBigEndian16(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
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


void GeneratorCore::WriteConstantTable()
{
	const int startPos = mWriter.GetPosition();

	// Skip the 4 bytes for the table size.
	mWriter.AddOffset(sizeof(Value32));

	WriteValue16(Value16(mValue32List.size()));
	WriteValue16(Value16(mValue64List.size()));
	WriteValue16(Value16(mStringList.size()));

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
		const int length = it->GetLength();
		const char *str = it->GetString();
		WriteValue16(Value16(length));
		for (int i = 0; i < length; ++i)
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
	for (FunctionList::Type::const_iterator flit = mFunctionList.begin(); flit != mFunctionList.end(); ++flit)
	{
		const FunctionDefinition *function = flit->mDefinition;

		// Cache the blob start position and skip 4 bytes for the blob size.
		const int blobStartPos = mWriter.GetPosition();
		mWriter.AddOffset(sizeof(Value32));

		WriteValue16(Value16(functionIndex));
		WriteReturnSignature(function->GetPrototype()->GetReturnType());
		WriteQualifiedSymbolName(function);
		WriteParamListSignature(function->GetPrototype()->GetParameterList());
		WriteValue32(Value32(function->GetGlobalHashCode()));
		WriteValue32(Value32(function->GetFrameSize()));
		WriteValue32(Value32(function->GetPackedFrameSize()));
		WriteValue32(Value32(function->GetLocalSize()));
		WriteValue32(Value32(function->GetFramePointerAlignment()));

		// Cache the code start position and skip 4 bytes for the code size.
		const int codeSizePos = mWriter.GetPosition();
		mWriter.AddOffset(sizeof(Value32));
		const int codeStartPos = mWriter.GetPosition();

		const ByteCode::Type &byteCode = flit->mByteCode;
		const JumpList::Type &jumpList = flit->mJumpList;
		const LabelList::Type &labelList = flit->mLabelList;

		size_t byteCodeIndex = 0;
		for (JumpList::Type::const_iterator jlit = jumpList.begin(); jlit != jumpList.end(); ++jlit)
		{
			while (byteCodeIndex < jlit->mOpCodePos)
			{
				mWriter.Write(byteCode[byteCodeIndex++]);
			}

			bu8_t opCode = byteCode[byteCodeIndex++];
			const bi32_t offset = labelList[jlit->mToLabel] - jlit->mFromPos;
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


void GeneratorCore::WriteQualifiedSymbolName(const Symbol *symbol)
{
	// Cache the position for the number of elements and skip 2 bytes.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value16));

	WriteSymbolNameIndices(symbol);

	// Patch up the number of elements.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue16(Value16((endPos - startPos - sizeof(Value16)) / sizeof(Value16)));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::WriteReturnSignature(const TypeDescriptor *type)
{
	const bu32_t sizeAndType = EncodeSizeAndType(type->GetSize(mPointerSize), type->GetSignatureType());
	WriteValue32(Value32(sizeAndType));
}


void GeneratorCore::WriteParamListSignature(const Parameter *parameterList)
{
	// Cache the position for the number of parameters and skip 2 bytes.
	// TODO: Consider implicit this pointer.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(sizeof(Value16));

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
	WriteValue16(Value16((endPos - startPos - sizeof(Value16)) / (2 * sizeof(Value32))));
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


GeneratorCore::ByteCode::Type &GeneratorCore::GetByteCode()
{
	return mFunction.GetTop()->mByteCode;
}


size_t GeneratorCore::CreateLabel()
{
	LabelList::Type &labelList = mFunction.GetTop()->mLabelList;
	size_t label = labelList.size();
	labelList.push_back(0);
	return label;
}


void GeneratorCore::SetLabelValue(size_t label, size_t value)
{
	mFunction.GetTop()->mLabelList[label] = value;
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


void GeneratorCore::MapLongJumpOffsets()
{
	for (FunctionList::Type::const_iterator flit = mFunctionList.begin(); flit != mFunctionList.end(); ++flit)
	{
		const JumpList::Type &jumpList = flit->mJumpList;
		const LabelList::Type &labelList = flit->mLabelList;

		for (JumpList::Type::const_iterator jlit = jumpList.begin(); jlit != jumpList.end(); ++jlit)
		{
			const bi32_t offset = labelList[jlit->mToLabel] - jlit->mFromPos;
			if (!IsInShortRange(offset))
			{
				MapValue32(Value32(offset));
			}
		}
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
	const bu16_t index = static_cast<bu16_t>(mStringList.size());
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
	const bu16_t index = static_cast<bu16_t>(mValue32List.size());
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
	const bu16_t index = static_cast<bu16_t>(mValue64List.size());
	Value64IndexMap::InsertResult insertResult = mValue64IndexMap.insert(Value64IndexMap::KeyValue(value, index));
	if (insertResult.second)
	{
		mValue64List.push_back(value);
	}
	return insertResult.first->second;
}


void GeneratorCore::PushError(CompilerError::Type type)
{
	if (!mErrorBuffer.HasErrors())
	{
		mErrorBuffer.PushError(type);
	}
}

}
