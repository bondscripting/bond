#include "bond/autostack.h"
#include "bond/binarywriter.h"
#include "bond/codegenerator.h"
#include "bond/endian.h"
#include "bond/list.h"
#include "bond/opcodes.h"
#include "bond/parsenodeutil.h"
#include "bond/parsenodetraverser.h"
#include "bond/map.h"
#include "bond/vector.h"
#include "bond/version.h"
#include <stdio.h>

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

	OpCode GetOpCode(Token::TokenType type) const
	{
		switch (type)
		{
			case Token::KEY_INT:
				return mOpCodeInt;
			case Token::KEY_UINT:
				return mOpCodeUInt;
			case Token::KEY_FLOAT:
				return mOpCodeFloat;
				// TODO Handle long and double.
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
static const OpCodeSet NOT_OPCODES(OPCODE_NOTI, OPCODE_NOTL);
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
			const TranslationUnit *translationUnitList,
			bu32_t pointerSize):
		mStringIndexMap(StringIndexMap::Compare(), StringIndexMap::Allocator(&allocator)),
		mStringList(StringList::Allocator(&allocator)),
		mFunctionList(FunctionList::Allocator(&allocator)),
		mAllocator(allocator),
		mWriter(writer),
		mTranslationUnitList(translationUnitList),
		mPointerSize(pointerSize)
	{}

	void Generate();

private:
	struct GeneratorResult
	{
		enum Context
		{
			CONTEXT_NONE,
			CONTEXT_FP_VALUE,
			CONTEXT_FP_ADDRESS,
			CONTEXT_STACK_VALUE,
			CONTEXT_STACK_ADDRESS,
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

	typedef Vector<unsigned char> ByteCode;
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
	typedef List<HashedString> StringList;
	typedef List<CompiledFunction> FunctionList;
	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<CompiledFunction *> FunctionStack;
	typedef AutoStack<GeneratorResult> ResultStack;

	virtual void Visit(const StructDeclaration *structDeclaration);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const NamedInitializer *namedInitializer);
	virtual void Visit(const IfStatement *ifStatement);
	virtual void Visit(const WhileStatement *whileStatement);
	virtual void Visit(const ForStatement *forStatement);
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

	void EmitPushResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor);
	void EmitPushFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPushFramePointerRelativeValue32(bi32_t offset);
	void EmitPushFramePointerRelativeValue64(bi32_t offset);
	void EmitPushAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);

	void EmitPushConstant(const TypeAndValue &typeAndValue);
	void EmitPushConstantInt(bi32_t value);
	void EmitPushConstantUInt(bu32_t value);
	void EmitPushConstantFloat(bf32_t value);

	void EmitPopResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor);
	void EmitPopFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPopFramePointerRelativeValue32(bi32_t offset);
	void EmitPopFramePointerRelativeValue64(bi32_t offset);
	void EmitPopAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);

	void EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType);

	void EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);
	void EmitAssignmentOperator(const BinaryExpression *binaryExpression);
	void EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet);
	void EmitLogicalOperator(const BinaryExpression *binaryExpression, OpCode branchOpCode);

	void EmitJump(OpCode opCode, size_t toLabel);

	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitValue16(Value16 value);
	void EmitValue16(Value16 value, size_t pos);
	void EmitValue32(Value32 value);
	void EmitValue32(Value32 value, size_t pos);

	void WriteValue16(Value16 value);
	void WriteValue32(Value32 value);
	void WriteStringTable();
	void WriteFunctionList(bu16_t functionIndex);

	bool Is64BitPointer() const { return mPointerSize == 8; }
	ByteCode::Type &GetByteCode();
	size_t CreateLabel();
	void SetLabelValue(size_t label, size_t value);
	bu16_t MapString(const HashedString &str);

	StringIndexMap::Type mStringIndexMap;
	StringList::Type mStringList;
	FunctionList::Type mFunctionList;
	ResultStack mResult;
	StructStack mStruct;
	FunctionStack mFunction;
	Allocator &mAllocator;
	BinaryWriter &mWriter;
	const TranslationUnit *mTranslationUnitList;
	bu32_t mPointerSize;
};


void CodeGenerator::Generate(const TranslationUnit *translationUnitList, BinaryWriter &writer)
{
	GeneratorCore generator(mAllocator, writer, translationUnitList, mPointerSize);
	generator.Generate();
}


void GeneratorCore::Generate()
{
	StructStack::Element structElement(mStruct, NULL);
	Traverse(mTranslationUnitList);

	WriteValue32(Value32(MAGIC_NUMBER));
	WriteValue16(Value16(static_cast<bu16_t>(MAJOR_VERSION | (Is64BitPointer() ? 0x80 : 0))));
	WriteValue16(Value16(MINOR_VERSION));

	bu16_t listIndex = MapString("List");
	bu16_t functionIndex = mFunctionList.empty() ? 0 : MapString("Func");

	WriteStringTable();

	// Cache the start position and skip 4 bytes for the blob size.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(4);

	WriteValue16(Value16(listIndex));
	WriteValue32(Value32(static_cast<bu32_t>(/* mDefinitionList.size() + */ mFunctionList.size())));

	WriteFunctionList(functionIndex);

	// Patch up the blob size.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue32(Value32(static_cast<bu32_t>(endPos - startPos)));
	mWriter.SetPosition(endPos);
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
	CompiledFunction &function =
		*mFunctionList.insert(mFunctionList.end(), CompiledFunction(functionDefinition, mAllocator));
	FunctionStack::Element functionElement(mFunction, &function);
	//MapString(functionDefinition->GetName()->GetHashedText());
	ParseNodeTraverser::Visit(functionDefinition);
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
			if ((initializer != NULL) && (initializer->GetExpression() != NULL))
			{
				const TypeDescriptor *lhDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
				const TypeDescriptor *rhDescriptor =
					initializer->GetExpression()->GetTypeAndValue().GetTypeDescriptor();
				ResultStack::Element rhResult(mResult);
				GeneratorResult lhResult(GeneratorResult::CONTEXT_FP_VALUE, namedInitializer->GetOffset());
				Traverse(initializer);
				EmitPushResult(rhResult.GetValue(), rhDescriptor);
				EmitCast(rhDescriptor, lhDescriptor);
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
	const TypeDescriptor *conditionDescriptor = condition->GetTypeAndValue().GetTypeDescriptor();
	ResultStack::Element conditionResult(mResult);
	Traverse(condition);
	EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

	// Jump past the then statement if the condition fails.
	size_t thenEndLabel = CreateLabel();
	EmitJump(OPCODE_IFZW, thenEndLabel);

	Traverse(ifStatement->GetThenStatement());
	size_t thenEndPos = byteCode.size();

	if (ifStatement->GetElseStatement() != NULL)
	{
		// If there's an else statement, the then statement must end with a jump past the else.
		size_t elseEndLabel = CreateLabel();
		EmitJump(OPCODE_GOTOW, elseEndLabel);
		thenEndPos = byteCode.size();
		Traverse(ifStatement->GetElseStatement());
		SetLabelValue(elseEndLabel, byteCode.size());
	}

	SetLabelValue(thenEndLabel, thenEndPos);
}


void GeneratorCore::Visit(const WhileStatement *whileStatement)
{
	// TODO: Handle continue and break statements.
	ByteCode::Type &byteCode = GetByteCode();
	const size_t loopStartLabel = CreateLabel();
	SetLabelValue(loopStartLabel, byteCode.size());
	const Expression *condition = whileStatement->GetCondition();
	const TypeDescriptor *conditionDescriptor = condition->GetTypeAndValue().GetTypeDescriptor();

	if (whileStatement->IsDoLoop())
	{
		Traverse(whileStatement->GetBody());

		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

		// Jump back to the top of the loop if the condition does not fail.
		EmitJump(OPCODE_IFNZW, loopStartLabel);
	}
	else
	{
		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

		// Jump out of the loop if the condition fails.
		const size_t loopEndLabel = CreateLabel();
		EmitJump(OPCODE_IFZW, loopEndLabel);

		Traverse(whileStatement->GetBody());

		// Jump back to the top of the loop.
		EmitJump(OPCODE_GOTOW, loopStartLabel);
	}
}


void GeneratorCore::Visit(const ForStatement *forStatement)
{
	// TODO: Handle continue and break statements.
	Traverse(forStatement->GetInitializer());

	ByteCode::Type &byteCode = GetByteCode();
	const size_t loopStartLabel = CreateLabel();
	SetLabelValue(loopStartLabel, byteCode.size());

	const Expression *condition = forStatement->GetCondition();
	size_t loopEndLabel = 0;
	if (condition != NULL)
	{
		const TypeDescriptor *conditionDescriptor = condition->GetTypeAndValue().GetTypeDescriptor();
		ResultStack::Element conditionResult(mResult);
		Traverse(condition);
		EmitPushResult(conditionResult.GetValue(), conditionDescriptor);

		// Jump out of the loop if the condition fails.
		loopEndLabel = CreateLabel();
		EmitJump(OPCODE_IFZW, loopEndLabel);
	}

	Traverse(forStatement->GetBody());
	Traverse(forStatement->GetCountingExpression());

	// Jump back to the top of the loop.
	byteCode.push_back(OPCODE_GOTOW);

	if (condition != NULL)
	{
		SetLabelValue(loopEndLabel, byteCode.size());
	}
}


void GeneratorCore::Visit(const ConditionalExpression *conditionalExpression)
{
	ParseNodeTraverser::Visit(conditionalExpression);
}


void GeneratorCore::Visit(const BinaryExpression *binaryExpression)
{
	//ByteCode::Type &byteCode = GetByteCode();
	if (!ProcessConstantExpression(binaryExpression))
	{
		const TypeAndValue &lhTav = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhTav = binaryExpression->GetRhs()->GetTypeAndValue();
		//const TypeAndValue &resultTav = binaryExpression->GetTypeAndValue();
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
		//const TypeDescriptor *resultDescriptor = resultTav.GetTypeDescriptor();
		const Token *op = binaryExpression->GetOperator();

		switch (op->GetTokenType())
		{
			case Token::COMMA:
				// TODO
				break;
			case Token::ASSIGN:
				EmitAssignmentOperator(binaryExpression);
				break;
			case Token::ASSIGN_LEFT:
				EmitCompoundAssignmentOperator(binaryExpression, LSH_OPCODES);
				break;
			case Token::ASSIGN_RIGHT:
				EmitCompoundAssignmentOperator(binaryExpression, RSH_OPCODES);
				break;
			case Token::ASSIGN_MOD:
				EmitCompoundAssignmentOperator(binaryExpression, REM_OPCODES);
				break;
			case Token::ASSIGN_AND:
				EmitCompoundAssignmentOperator(binaryExpression, AND_OPCODES);
				break;
			case Token::ASSIGN_OR:
				EmitCompoundAssignmentOperator(binaryExpression, OR_OPCODES);
				break;
			case Token::ASSIGN_XOR:
				EmitCompoundAssignmentOperator(binaryExpression, XOR_OPCODES);
				break;

			case Token::ASSIGN_PLUS:
				if (lhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else if (rhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else
				{
					EmitCompoundAssignmentOperator(binaryExpression, ADD_OPCODES);
				}
				break;

			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else if (rhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else
				{
					EmitCompoundAssignmentOperator(binaryExpression, SUB_OPCODES);
				}
				break;

			case Token::ASSIGN_MULT:
				EmitCompoundAssignmentOperator(binaryExpression, MUL_OPCODES);
				break;
			case Token::ASSIGN_DIV:
				EmitCompoundAssignmentOperator(binaryExpression, DIV_OPCODES);
				break;
			case Token::OP_AND:
				EmitLogicalOperator(binaryExpression, OPCODE_BRZW);
				break;
			case Token::OP_OR:
				EmitLogicalOperator(binaryExpression, OPCODE_BRNZW);
				break;
			case Token::OP_AMP:
				EmitSimpleBinaryOperator(binaryExpression, AND_OPCODES);
				break;
			case Token::OP_BIT_OR:
				EmitSimpleBinaryOperator(binaryExpression, OR_OPCODES);
				break;
			case Token::OP_BIT_XOR:
				EmitSimpleBinaryOperator(binaryExpression, XOR_OPCODES);
				break;
			case Token::OP_MOD:
				EmitSimpleBinaryOperator(binaryExpression, REM_OPCODES);
				break;
			case Token::OP_LEFT:
				EmitSimpleBinaryOperator(binaryExpression, LSH_OPCODES);
				break;
			case Token::OP_RIGHT:
				EmitSimpleBinaryOperator(binaryExpression, RSH_OPCODES);
				break;
			case Token::OP_LT:
				EmitSimpleBinaryOperator(binaryExpression, CMPLT_OPCODES);
				break;
			case Token::OP_LTE:
				EmitSimpleBinaryOperator(binaryExpression, CMPLE_OPCODES);
				break;
			case Token::OP_GT:
				EmitSimpleBinaryOperator(binaryExpression, CMPGT_OPCODES);
				break;
			case Token::OP_GTE:
				EmitSimpleBinaryOperator(binaryExpression, CMPGE_OPCODES);
				break;
			case Token::OP_EQUAL:
				EmitSimpleBinaryOperator(binaryExpression, CMPEQ_OPCODES);
				break;
			case Token::OP_NOT_EQUAL:
				EmitSimpleBinaryOperator(binaryExpression, CMPNEQ_OPCODES);
				break;

			case Token::OP_PLUS:
			{
				if (lhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else if (rhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else
				{
					EmitSimpleBinaryOperator(binaryExpression, ADD_OPCODES);
				}
			}
			break;

			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else if (rhDescriptor->IsPointerType())
				{
					// TODO: Pointer arithmetic.
				}
				else
				{
					EmitSimpleBinaryOperator(binaryExpression, SUB_OPCODES);
				}
				break;

			case Token::OP_STAR:
				EmitSimpleBinaryOperator(binaryExpression, MUL_OPCODES);
				break;
			case Token::OP_DIV:
				EmitSimpleBinaryOperator(binaryExpression, DIV_OPCODES);
				break;

			default:
				break;
		}

		mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE));
	}
}


void GeneratorCore::Visit(const UnaryExpression *unaryExpression)
{
	ParseNodeTraverser::Visit(unaryExpression);
}


void GeneratorCore::Visit(const PostfixExpression *postfixExpression)
{
	ParseNodeTraverser::Visit(postfixExpression);
}


void GeneratorCore::Visit(const MemberExpression *memberExpression)
{
	ParseNodeTraverser::Visit(memberExpression);
}


void GeneratorCore::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	ParseNodeTraverser::Visit(arraySubscriptExpression);
}


void GeneratorCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	ParseNodeTraverser::Visit(functionCallExpression);
}


void GeneratorCore::Visit(const CastExpression *castExpression)
{
	if (!ProcessConstantExpression(castExpression))
	{
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
					mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, namedInitializer->GetOffset()));
					break;
				case SCOPE_STRUCT_MEMBER:
					// TODO
					break;
			}
		}
		else if ((parameter = CastNode<Parameter>(symbol)) != NULL)
		{
			mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, parameter->GetOffset()));
		}
		else if ((functionDefinition = CastNode<FunctionDefinition>(symbol)) != NULL)
		{
			// TODO
		}
	}
}


void GeneratorCore::Visit(const ThisExpression *thisExpression)
{
	mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, -mPointerSize));
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


void GeneratorCore::EmitPushResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_VALUE:
			EmitPushFramePointerRelativeValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_FP_ADDRESS:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
			EmitPushAddressRelativeValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			EmitPushConstant(*result.mTypeAndValue);
			break;

		case GeneratorResult::CONTEXT_STACK_VALUE:
			// Nothing to do; the value has already been pushed onto the stack.
			break;

		case GeneratorResult::CONTEXT_NONE:
			// TODO: Assert that getting here is bad because it does not make sense.
			break;
	}

	//result.mContext = GeneratorResult::CONTEXT_STACK_VALUE;
	//result.mOffset = 0;
}


void GeneratorCore::EmitPushFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
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
				EmitPushFramePointerRelativeValue32(offset);
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
			EmitPushFramePointerRelativeValue64(offset);
		}
		else
		{
			EmitPushFramePointerRelativeValue32(offset);
		}
	}
}


void GeneratorCore::EmitPushFramePointerRelativeValue32(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -16:
			byteCode.push_back(OPCODE_PUSH32_P3);
			break;
		case -12:
			byteCode.push_back(OPCODE_PUSH32_P2);
			break;
		case -8:
			byteCode.push_back(OPCODE_PUSH32_P1);
			break;
		case -4:
			byteCode.push_back(OPCODE_PUSH32_P0);
			break;
		case 0:
			byteCode.push_back(OPCODE_PUSH32_L0);
			break;
		case 4:
			byteCode.push_back(OPCODE_PUSH32_L1);
			break;
		case 8:
			byteCode.push_back(OPCODE_PUSH32_L2);
			break;
		case 12:
			byteCode.push_back(OPCODE_PUSH32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH32, offset);
			break;
	}
}


void GeneratorCore::EmitPushFramePointerRelativeValue64(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -32:
			byteCode.push_back(OPCODE_PUSH64_P3);
			break;
		case -24:
			byteCode.push_back(OPCODE_PUSH64_P2);
			break;
		case -16:
			byteCode.push_back(OPCODE_PUSH64_P1);
			break;
		case -8:
			byteCode.push_back(OPCODE_PUSH64_P0);
			break;
		case 0:
			byteCode.push_back(OPCODE_PUSH64_L0);
			break;
		case 8:
			byteCode.push_back(OPCODE_PUSH64_L1);
			break;
		case 16:
			byteCode.push_back(OPCODE_PUSH64_L2);
			break;
		case 24:
			byteCode.push_back(OPCODE_PUSH64_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH64, offset);
			break;
	}
}


void GeneratorCore::EmitPushAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	// Add the accumulated offset to the address that is already on the stack. Then push the value
	// at the resulting address.
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

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
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


void GeneratorCore::EmitPushConstant(const TypeAndValue &typeAndValue)
{
	ByteCode::Type &byteCode = GetByteCode();
	const TypeDescriptor *typeDescriptor = typeAndValue.GetTypeDescriptor();
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
		case Token::KEY_USHORT:
		case Token::KEY_UINT:
			EmitPushConstantUInt(typeAndValue.GetUIntValue());
			break;
		case Token::KEY_FLOAT:
			EmitPushConstantFloat(typeAndValue.GetFloatValue());
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
			if ((value >= BOND_CHAR_MIN) && (value <= BOND_CHAR_MAX))
			{
				byteCode.push_back(OPCODE_CONSTC);
				byteCode.push_back(static_cast<unsigned char>(value));
			}
			else if ((value >= BOND_SHORT_MIN) && (value <= BOND_SHORT_MAX))
			{
				byteCode.push_back(OPCODE_CONSTS);
				EmitValue16(Value16(static_cast<bi16_t>(value)));
			}
			else
			{
				byteCode.push_back(OPCODE_CONST32);
				EmitValue32(Value32(value));
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
			if (value <= BOND_UCHAR_MAX)
			{
				byteCode.push_back(OPCODE_CONSTUC);
				byteCode.push_back(static_cast<unsigned char>(value));
			}
			else if (value <= BOND_USHORT_MAX)
			{
				byteCode.push_back(OPCODE_CONSTUS);
				EmitValue16(Value16(static_cast<bi16_t>(value)));
			}
			else
			{
				byteCode.push_back(OPCODE_CONST32);
				EmitValue32(Value32(value));
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
		EmitValue32(Value32(value));
	}
}


void GeneratorCore::EmitPopResult(const GeneratorResult &result, const TypeDescriptor *typeDescriptor)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_VALUE:
			EmitPopFramePointerRelativeValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
			EmitPopAddressRelativeValue(typeDescriptor, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_FP_ADDRESS:
		case GeneratorResult::CONTEXT_STACK_VALUE:
		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			// TODO: Assert that getting here is bad because it does not make sense.
			break;
	}
}


void GeneratorCore::EmitPopFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
				EmitOpCodeWithOffset(OPCODE_POPC, offset);
				break;
			case Token::KEY_SHORT:
			case Token::KEY_USHORT:
				EmitOpCodeWithOffset(OPCODE_POPS, offset);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				EmitPopFramePointerRelativeValue32(offset);
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
			EmitPopFramePointerRelativeValue64(offset);
		}
		else
		{
			EmitPopFramePointerRelativeValue32(offset);
		}
	}
}


void GeneratorCore::EmitPopFramePointerRelativeValue32(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -16:
			byteCode.push_back(OPCODE_POP32_P3);
			break;
		case -12:
			byteCode.push_back(OPCODE_POP32_P2);
			break;
		case -8:
			byteCode.push_back(OPCODE_POP32_P1);
			break;
		case -4:
			byteCode.push_back(OPCODE_POP32_P0);
			break;
		case 0:
			byteCode.push_back(OPCODE_POP32_L0);
			break;
		case 4:
			byteCode.push_back(OPCODE_POP32_L1);
			break;
		case 8:
			byteCode.push_back(OPCODE_POP32_L2);
			break;
		case 12:
			byteCode.push_back(OPCODE_POP32_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP32, offset);
			break;
	}
}


void GeneratorCore::EmitPopFramePointerRelativeValue64(bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (offset)
	{
		case -32:
			byteCode.push_back(OPCODE_POP64_P3);
			break;
		case -24:
			byteCode.push_back(OPCODE_POP64_P2);
			break;
		case -16:
			byteCode.push_back(OPCODE_POP64_P1);
			break;
		case -8:
			byteCode.push_back(OPCODE_POP64_P0);
			break;
		case 0:
			byteCode.push_back(OPCODE_POP64_L0);
			break;
		case 8:
			byteCode.push_back(OPCODE_POP64_L1);
			break;
		case 16:
			byteCode.push_back(OPCODE_POP64_L2);
			break;
		case 24:
			byteCode.push_back(OPCODE_POP64_L3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_POP64, offset);
			break;
	}
}


void GeneratorCore::EmitPopAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset)
{
	// Add the accumulated offset to the address that is already on the stack. Then push the value
	// at the resulting address.
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

	if (typeDescriptor->IsValueType())
	{
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
			case Token::KEY_CHAR:
				byteCode.push_back(OPCODE_STOREC);
				break;
			case Token::KEY_SHORT:
			case Token::KEY_USHORT:
				byteCode.push_back(OPCODE_STORES);
				break;
			case Token::KEY_INT:
			case Token::KEY_UINT:
			case Token::KEY_FLOAT:
				byteCode.push_back(OPCODE_STORE32);
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


void GeneratorCore::EmitCast(const TypeDescriptor *sourceType, const TypeDescriptor *destType)
{
	ByteCode::Type &byteCode = GetByteCode();
	switch (destType->GetPrimitiveType())
	{
		case Token::KEY_BOOL:
		case Token::KEY_CHAR:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_SHORT:
				case Token::KEY_USHORT:
				case Token::KEY_INT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_ITOC);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
					byteCode.push_back(OPCODE_ITOC);
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
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
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
					byteCode.push_back(OPCODE_ITOS);
					break;
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUI);
					byteCode.push_back(OPCODE_ITOS);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_INT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOI);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
			switch (sourceType->GetPrimitiveType())
			{
				case Token::KEY_FLOAT:
					byteCode.push_back(OPCODE_FTOUI);
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
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					byteCode.push_back(OPCODE_UITOF);
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}
}


void GeneratorCore::EmitSimpleBinaryOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult.GetValue(), lhDescriptor);
	EmitCast(lhDescriptor, &resultDescriptor);

	ResultStack::Element rhResult(mResult);
	Traverse(binaryExpression->GetRhs());
	EmitPushResult(rhResult.GetValue(), rhDescriptor);
	EmitCast(rhDescriptor, &resultDescriptor);

	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(opCodeSet.GetOpCode(resultDescriptor.GetPrimitiveType()));
}


void GeneratorCore::EmitAssignmentOperator(const BinaryExpression *binaryExpression)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeAndValue().GetTypeDescriptor();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);

	ResultStack::Element rhResult(mResult);
	Traverse(binaryExpression->GetRhs());
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	EmitCast(rhDescriptor, lhDescriptor);
	EmitPopResult(lhResult, lhDescriptor);

	// TODO: Check if a temporary needs to be left on the stack.
}


void GeneratorCore::EmitCompoundAssignmentOperator(const BinaryExpression *binaryExpression, const OpCodeSet &opCodeSet)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult.GetValue(), lhDescriptor);
	EmitCast(lhDescriptor, &resultDescriptor);

	// TODO: If the result produces an address, it needs to be duped.

	ResultStack::Element rhResult(mResult);
	Traverse(binaryExpression->GetRhs());
	EmitPushResult(rhResult.GetValue(), rhDescriptor);
	EmitCast(rhDescriptor, &resultDescriptor);

	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(opCodeSet.GetOpCode(resultDescriptor.GetPrimitiveType()));

	EmitCast(&resultDescriptor, lhDescriptor);
	EmitPopResult(lhResult, lhDescriptor);

	// TODO: Check if a temporary needs to be left on the stack.
}


void GeneratorCore::EmitLogicalOperator(const BinaryExpression *binaryExpression, OpCode branchOpCode)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor resultDescriptor = CombineOperandTypes(lhDescriptor, rhDescriptor);

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult.GetValue(), lhDescriptor);

	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(branchOpCode);

	// Cache the jump offset position and skip 4 bytes for it.
	const size_t offsetPos = byteCode.size();
	EmitValue32(Value32(0));

	ResultStack::Element rhResult(mResult);
	Traverse(binaryExpression->GetRhs());
	EmitPushResult(rhResult.GetValue(), rhDescriptor);

	// Patch up the offset position.
	const size_t targetPos = byteCode.size();
	EmitValue32(Value32(static_cast<bi32_t>(targetPos - offsetPos - 4)), offsetPos);
}


void GeneratorCore::EmitJump(OpCode opCode, size_t toLabel)
{
	CompiledFunction *function = mFunction.GetTop();
	ByteCode::Type &byteCode = function->mByteCode;
	const Value32 jumpId(static_cast<bu32_t>(function->mJumpList.size()));
	const size_t opCodePos = byteCode.size();
	byteCode.push_back(opCode);
	byteCode.push_back(jumpId.mBytes[0]);
	byteCode.push_back(jumpId.mBytes[1]);
	byteCode.push_back(jumpId.mBytes[2]);
	byteCode.push_back(jumpId.mBytes[3]);
	const size_t fromPos = byteCode.size();
	function->mJumpList.push_back(JumpEntry(opCodePos, fromPos, toLabel));
}


void GeneratorCore::EmitOpCodeWithOffset(OpCode opCode, bi32_t offset)
{
	ByteCode::Type &byteCode = GetByteCode();
	if ((offset >= BOND_SHORT_MIN) && (offset <= BOND_SHORT_MAX))
	{
		byteCode.push_back(opCode);
		EmitValue16(Value16(static_cast<bi16_t>(offset)));
	}
	else
	{
		byteCode.push_back(opCode + 1);
		EmitValue32(Value32(offset));
	}
}


void GeneratorCore::EmitValue16(Value16 value)
{
	ConvertBigEndian16(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
}


void GeneratorCore::EmitValue16(Value16 value, size_t pos)
{
	ConvertBigEndian16(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode[pos] = value.mBytes[0];
	byteCode[pos + 1] = value.mBytes[1];
}


void GeneratorCore::EmitValue32(Value32 value)
{
	ConvertBigEndian32(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
	byteCode.push_back(value.mBytes[2]);
	byteCode.push_back(value.mBytes[3]);
}


void GeneratorCore::EmitValue32(Value32 value, size_t pos)
{
	ConvertBigEndian32(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode[pos] = value.mBytes[0];
	byteCode[pos + 1] = value.mBytes[1];
	byteCode[pos + 2] = value.mBytes[2];
	byteCode[pos + 3] = value.mBytes[3];
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


void GeneratorCore::WriteStringTable()
{
	const int startPos = mWriter.GetPosition();

	// Skip the 4 bytes for the table size.
	mWriter.AddOffset(4);

	WriteValue16(Value16(static_cast<bu16_t>(mStringList.size())));

	for (StringList::Type::const_iterator it = mStringList.begin(); it != mStringList.end(); ++it)
	{
		const int length = it->GetLength();
		const char *str = it->GetString();
		WriteValue16(Value16(static_cast<bu16_t>(length)));
		for (int i = 0; i < length; ++i)
		{
			mWriter.Write(str[i]);
		}
	}

	// Patch up the table size.
	const int endPos = mWriter.GetPosition();
	mWriter.SetPosition(startPos);
	WriteValue32(Value32(static_cast<bu32_t>(endPos - startPos)));
	mWriter.SetPosition(endPos);
}


void GeneratorCore::WriteFunctionList(bu16_t functionIndex)
{
	for (FunctionList::Type::const_iterator flit = mFunctionList.begin(); flit != mFunctionList.end(); ++flit)
	{
		// Cache the blob start position and skip 4 bytes for the blob size.
		const int blobStartPos = mWriter.GetPosition();
		mWriter.AddOffset(4);

		WriteValue16(Value16(static_cast<bu16_t>(functionIndex)));
		WriteValue32(Value32(flit->mDefinition->GetGlobalHashCode()));

		// Cache the code start position and skip 4 bytes for the code size.
		const int codeSizePos = mWriter.GetPosition();
		mWriter.AddOffset(4);
		const int codeStartPos = mWriter.GetPosition();

		const ByteCode::Type &byteCode = flit->mByteCode;
		const JumpList::Type &jumpList = flit->mJumpList;
		const LabelList::Type &labelList = flit->mLabelList;

		size_t byteCodeIndex = 0;
		for (JumpList::Type::const_iterator jlit = jumpList.begin(); jlit != jumpList.end(); ++jlit)
		{
			while (byteCodeIndex <= jlit->mOpCodePos)
			{
				mWriter.Write(byteCode[byteCodeIndex++]);
			}
			const Value32 offset(static_cast<bu32_t>(labelList[jlit->mToLabel] - jlit->mFromPos));
			WriteValue32(offset);
			byteCodeIndex += 4;
		}

		while (byteCodeIndex < byteCode.size())
		{
			mWriter.Write(byteCode[byteCodeIndex++]);
		}

		// Patch up the code size.
		const int endPos = mWriter.GetPosition();
		mWriter.SetPosition(codeSizePos);
		WriteValue32(Value32(static_cast<bu32_t>(endPos - codeStartPos)));

		// Patch up the blob size.
		mWriter.SetPosition(blobStartPos);
		WriteValue32(Value32(static_cast<bu32_t>(endPos - blobStartPos)));
		mWriter.SetPosition(endPos);
	}
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


bu16_t GeneratorCore::MapString(const HashedString &str)
{
	// TODO: Verify that the 16 bit index does not overflow.
	// TODO: Verify that the string's length fits in 16 bits.
	const bu16_t index = static_cast<bu16_t>(mStringList.size());
	StringIndexMap::InsertResult insertResult = mStringIndexMap.insert(StringIndexMap::KeyValue(str, index));
	if (insertResult.second)
	{
		mStringList.push_back(str);
	}
	return insertResult.first->second;
}

}
