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
		/*
		GeneratorResult(Context context, const TypeDescriptor *typeDescriptor, const Value &value):
			mTypeAndValue(typeDescriptor, value),
			mContext(context),
			mOffset(0)
		{}
		*/

		Context mContext;
		const TypeAndValue *mTypeAndValue;
		bi32_t mOffset;
	};

	typedef Vector<unsigned char> ByteCode;

	struct CompiledFunction
	{
		CompiledFunction(const FunctionDefinition *definition, Allocator &allocator):
			mDefinition(definition),
			mByteCode(ByteCode::Allocator(&allocator))
		{}
		const FunctionDefinition *mDefinition;
		ByteCode::Type mByteCode;
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

	void EmitArithmeticBinaryOperator(
		const BinaryExpression *binaryExpression,
		OpCode opCodeInt,
		OpCode opCodeUInt,
		OpCode opCodeFloat);

	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitValue16(Value16 value);
	void EmitValue32(Value32 value);

	void WriteValue16(Value16 value);
	void WriteValue32(Value32 value);
	void WriteStringTable();
	void WriteFunctionList(bu16_t functionIndex);

	bool Is64BitPointer() const { return mPointerSize == 8; }
	ByteCode::Type &GetByteCode();
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
	WriteFunctionList(functionIndex);

	// Cache the start position and skip 4 bytes for the blob size.
	const int startPos = mWriter.GetPosition();
	mWriter.AddOffset(4);

	WriteValue16(Value16(listIndex));
	WriteValue32(Value32(static_cast<bu32_t>(/* mDefinitionList + */ mFunctionList.size())));

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
			if (initializer->GetExpression() != NULL)
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
				break;

			case Token::ASSIGN:
				break;

			case Token::ASSIGN_LEFT:
			case Token::ASSIGN_RIGHT:
			case Token::ASSIGN_MOD:
			case Token::ASSIGN_AND:
			case Token::ASSIGN_OR:
			case Token::ASSIGN_XOR:
				break;

			case Token::ASSIGN_PLUS:
			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
				}
				else if (rhDescriptor->IsPointerType())
				{
				}
				else
				{
				}

				break;

			case Token::ASSIGN_MULT:
			case Token::ASSIGN_DIV:
				break;

			case Token::OP_AND:
			case Token::OP_OR:
				break;

			case Token::OP_AMP:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_ANDI, OPCODE_ANDI, OPCODE_ANDI);
				break;
			case Token::OP_BIT_OR:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_ORI, OPCODE_ORI, OPCODE_ORI);
				break;
			case Token::OP_BIT_XOR:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_XORI, OPCODE_XORI, OPCODE_XORI);
				break;
			case Token::OP_MOD:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_REMI, OPCODE_REMUI, OPCODE_REMI);
				break;
			case Token::OP_LEFT:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_LSHI, OPCODE_LSHI, OPCODE_LSHI);
				break;
			case Token::OP_RIGHT:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_RSHI, OPCODE_RSHUI, OPCODE_RSHI);
				break;

			case Token::OP_LT:
			case Token::OP_LTE:
			case Token::OP_GT:
			case Token::OP_GTE:
			case Token::OP_EQUAL:
			case Token::OP_NOT_EQUAL:
				break;

			case Token::OP_PLUS:
			{
				if (lhDescriptor->IsPointerType())
				{
				}
				else if (rhDescriptor->IsPointerType())
				{
				}
				else
				{
					EmitArithmeticBinaryOperator(binaryExpression, OPCODE_ADDI, OPCODE_ADDI, OPCODE_ADDF);
				}
			}
			break;

			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType())
				{
				}
				else if (rhDescriptor->IsPointerType())
				{
				}
				else
				{
					EmitArithmeticBinaryOperator(binaryExpression, OPCODE_SUBI, OPCODE_SUBI, OPCODE_SUBF);
				}
				break;

			case Token::OP_STAR:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_MULI, OPCODE_MULUI, OPCODE_MULF);
				break;
			case Token::OP_DIV:
				EmitArithmeticBinaryOperator(binaryExpression, OPCODE_DIVI, OPCODE_DIVUI, OPCODE_DIVF);
				break;

			default:
				break;
		}

		mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_STACK_VALUE));
	}

	//ParseNodeTraverser::Visit(binaryExpression);
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


void GeneratorCore::EmitArithmeticBinaryOperator(
	const BinaryExpression *binaryExpression,
	OpCode opCodeInt,
	OpCode opCodeUInt,
	OpCode opCodeFloat)
{
	const Expression *lhs = binaryExpression->GetLhs();
	const Expression *rhs = binaryExpression->GetRhs();
	const TypeDescriptor *lhDescriptor = lhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *rhDescriptor = rhs->GetTypeAndValue().GetTypeDescriptor();
	const TypeDescriptor *resultDescriptor = binaryExpression->GetTypeAndValue().GetTypeDescriptor();

	ResultStack::Element lhResult(mResult);
	Traverse(lhs);
	EmitPushResult(lhResult.GetValue(), lhDescriptor);
	EmitCast(lhDescriptor, resultDescriptor);

	ResultStack::Element rhResult(mResult);
	Traverse(binaryExpression->GetRhs());
	EmitPushResult(rhResult.GetValue(), rhDescriptor);
	EmitCast(rhDescriptor, resultDescriptor);

	ByteCode::Type &byteCode = GetByteCode();
	switch (resultDescriptor->GetPrimitiveType())
	{
		case Token::KEY_INT:
			byteCode.push_back(opCodeInt);
			break;
		case Token::KEY_UINT:
			byteCode.push_back(opCodeUInt);
			break;
		case Token::KEY_FLOAT:
			byteCode.push_back(opCodeFloat);
			break;
		default:
			break;
	}
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


void GeneratorCore::EmitValue32(Value32 value)
{
	ConvertBigEndian32(value.mBytes);
	ByteCode::Type &byteCode = GetByteCode();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
	byteCode.push_back(value.mBytes[2]);
	byteCode.push_back(value.mBytes[3]);
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
		// Cache the start position and skip 4 bytes for the blob size.
		const int startPos = mWriter.GetPosition();
		mWriter.AddOffset(4);

		WriteValue16(Value16(static_cast<bu16_t>(functionIndex)));
		WriteValue32(Value32(flit->mDefinition->GetGlobalHashCode()));
		WriteValue32(Value32(static_cast<bu32_t>(flit->mByteCode.size())));

		ByteCode::Type::const_iterator end = flit->mByteCode.end();
		for (ByteCode::Type::const_iterator bcit = flit->mByteCode.begin(); bcit < end; ++bcit)
		{
			mWriter.Write(*bcit);
		}

		// Patch up the blob size.
		const int endPos = mWriter.GetPosition();
		mWriter.SetPosition(startPos);
		WriteValue32(Value32(static_cast<bu32_t>(endPos - startPos)));
		mWriter.SetPosition(endPos);
	}
}


GeneratorCore::ByteCode::Type &GeneratorCore::GetByteCode()
{
	return mFunction.GetTop()->mByteCode;
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
