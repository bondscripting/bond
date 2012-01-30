#include "bond/autostack.h"
#include "bond/codegenerator.h"
#include "bond/endian.h"
#include "bond/opcodes.h"
#include "bond/parsenodeutil.h"
#include "bond/parsenodetraverser.h"
#include "bond/set.h"
#include "bond/vector.h"
#include "bond/version.h"
#include <stdio.h>

namespace Bond
{

class GeneratorCore: private ParseNodeTraverser
{
public:
	GeneratorCore(Allocator &allocator, const TranslationUnit *translationUnitList, bu32_t pointerSize):
		mStringPool((StringPool::Compare()), (StringPool::Allocator(&allocator))),
		mAllocator(allocator),
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

		GeneratorResult(): mContext(CONTEXT_NONE), mOffset(0) {}
		GeneratorResult(Context context): mContext(context), mOffset(0) {}

		GeneratorResult(Context context, const TypeAndValue &typeAndValue):
			mTypeAndValue(typeAndValue),
			mContext(context),
			mOffset(0)
		{}

		GeneratorResult(Context context, const TypeDescriptor *typeDescriptor, const Value &value):
			mTypeAndValue(typeDescriptor, value),
			mContext(context),
			mOffset(0)
		{}

		GeneratorResult(Context context, const TypeDescriptor *typeDescriptor, bi32_t offset):
			mTypeAndValue(typeDescriptor),
			mContext(context),
			mOffset(offset)
		{}

		TypeAndValue mTypeAndValue;
		Context mContext;
		bi32_t mOffset;
	};

	typedef Set<HashedString> StringPool;
	typedef Vector<unsigned char> ByteCode;
	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;
	typedef AutoStack<GeneratorResult> ResultStack;
	typedef AutoStack<ByteCode::Type> ByteCodeStack;

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
	void EmitPushResult(const GeneratorResult &result);
	void EmitPushFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPushFramePointerRelativeValue32(bi32_t offset);
	void EmitPushFramePointerRelativeValue64(bi32_t offset);
	void EmitPushAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPushConstant(const TypeAndValue &typeAndValue);
	void EmitPushConstantInt(bi32_t value);
	void EmitPushConstantUInt(bu32_t value);
	void EmitPushConstantFloat(bf32_t value);
	void EmitPopResult(const GeneratorResult &result);
	void EmitPopFramePointerRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitPopFramePointerRelativeValue32(bi32_t offset);
	void EmitPopFramePointerRelativeValue64(bi32_t offset);
	void EmitPopAddressRelativeValue(const TypeDescriptor *typeDescriptor, bi32_t offset);
	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitValue16(Value16 value);
	void EmitValue32(Value32 value);
	bool Is64BitPointer() const { return mPointerSize == 8; }

	StringPool::Type mStringPool;
	ResultStack mResult;
	StructStack mStruct;
	FunctionStack mFunction;
	ByteCodeStack mByteCode;
	Allocator &mAllocator;
	const TranslationUnit *mTranslationUnitList;
	bu32_t mPointerSize;
};


void CodeGenerator::Generate(const TranslationUnit *translationUnitList)
{
	GeneratorCore generator(mAllocator, translationUnitList, mPointerSize);
	generator.Generate();
}


void GeneratorCore::Generate()
{
	StructStack::Element structElement(mStruct, NULL);
	mStringPool.insert("List");
	Traverse(mTranslationUnitList);

	printf("%p %u\n", mTranslationUnitList, mStringPool.size());
	for (StringPool::Type::const_iterator it = mStringPool.begin(); it != mStringPool.end(); ++it)
	{
		printf("%s\n", it->GetString());
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
	FunctionStack::Element functionElement(mFunction, functionDefinition);
	ByteCodeStack::Element byteCodeElement(mByteCode);
	mStringPool.insert("Func");
	mStringPool.insert(functionDefinition->GetName()->GetHashedText());
	ParseNodeTraverser::Visit(functionDefinition);
}


void GeneratorCore::Visit(const NamedInitializer *namedInitializer)
{
	switch (namedInitializer->GetScope())
	{
		case SCOPE_GLOBAL:
			mStringPool.insert("Decl");
			mStringPool.insert(namedInitializer->GetName()->GetHashedText());
			// TODO: output initializer data.
			break;

		case SCOPE_LOCAL:
		{
			ResultStack::Element rhResult(mResult);
			GeneratorResult lhResult(GeneratorResult::CONTEXT_FP_VALUE, namedInitializer->GetTypeAndValue()->GetTypeDescriptor(), namedInitializer->GetOffset());
			Traverse(namedInitializer->GetInitializer());
			EmitPushResult(rhResult.GetValue());
			// TODO: handle type conversions.
			EmitPopResult(lhResult);
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
	ParseNodeTraverser::Visit(binaryExpression);
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
		const TypeDescriptor *typeDescriptor = symbol->GetTypeAndValue()->GetTypeDescriptor();
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
					mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, typeDescriptor, namedInitializer->GetOffset()));
					break;
				case SCOPE_STRUCT_MEMBER:
					// TODO
					break;
			}
		}
		else if ((parameter = CastNode<Parameter>(symbol)) != NULL)
		{
			mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, typeDescriptor, parameter->GetOffset()));
		}
		else if ((functionDefinition = CastNode<FunctionDefinition>(symbol)) != NULL)
		{
			// TODO
		}
	}
}


void GeneratorCore::Visit(const ThisExpression *thisExpression)
{
	mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FP_VALUE, thisExpression->GetTypeDescriptor(), -mPointerSize));
}


bool GeneratorCore::ProcessConstantExpression(const Expression *expression)
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsValueDefined())
	{
		mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_CONSTANT_VALUE, tav));
		return true;
	}
	return false;
}


void GeneratorCore::EmitPushResult(const GeneratorResult &result)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_VALUE:
			EmitPushFramePointerRelativeValue(result.mTypeAndValue.GetTypeDescriptor(), result.mOffset);
			break;

		case GeneratorResult::CONTEXT_FP_ADDRESS:
			EmitOpCodeWithOffset(OPCODE_LOADFP, result.mOffset);
			break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
			EmitPushAddressRelativeValue(result.mTypeAndValue.GetTypeDescriptor(), result.mOffset);
			break;

		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			EmitPushConstant(result.mTypeAndValue);
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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


void GeneratorCore::EmitPopResult(const GeneratorResult &result)
{
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FP_VALUE:
			EmitPopFramePointerRelativeValue(result.mTypeAndValue.GetTypeDescriptor(), result.mOffset);
			break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
			EmitPopAddressRelativeValue(result.mTypeAndValue.GetTypeDescriptor(), result.mOffset);
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
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


void GeneratorCore::EmitOpCodeWithOffset(OpCode opCode, bi32_t offset)
{
	ByteCode::Type &byteCode = mByteCode.GetTop();
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
	ByteCode::Type &byteCode = mByteCode.GetTop();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
}


void GeneratorCore::EmitValue32(Value32 value)
{
	ConvertBigEndian16(value.mBytes);
	ByteCode::Type &byteCode = mByteCode.GetTop();
	byteCode.push_back(value.mBytes[0]);
	byteCode.push_back(value.mBytes[1]);
	byteCode.push_back(value.mBytes[2]);
	byteCode.push_back(value.mBytes[3]);
}

}
