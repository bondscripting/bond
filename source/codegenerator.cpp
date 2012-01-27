#include "bond/autostack.h"
#include "bond/codegenerator.h"
#include "bond/endian.h"
#include "bond/opcodes.h"
#include "bond/parsenodes.h"
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
			CONTEXT_FRAME_POINTER_OFFSET,
			CONTEXT_STACK_ADDRESS,
			CONTEXT_CONSTANT_VALUE,
			CONTEXT_STACK_VALUE,
		};

		GeneratorResult(): mContext(CONTEXT_NONE) {}
		GeneratorResult(Context context): mContext(context) {}
		GeneratorResult(Context context, const TypeAndValue &value): mTypeAndValue(value), mContext(context) {}

		GeneratorResult(Context context, const TypeDescriptor *typeDescriptor, const Value &value):
			mTypeAndValue(typeDescriptor, value),
			mContext(context)
		{}

		GeneratorResult(Context context, const TypeDescriptor *typeDescriptor, bi32_t offset):
			mTypeAndValue(typeDescriptor, Value(offset)),
			mContext(context)
		{}

		TypeAndValue mTypeAndValue;
		Context mContext;
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
	void EmitPushAddress(GeneratorResult &result);
	void EmitPushValue(GeneratorResult &result);
	void EmitPushValue32(bi32_t offset);
	void EmitPushValue64(bi32_t offset);
	void EmitOpCodeWithOffset(OpCode opCode, bi32_t offset);
	void EmitPushConstantInt(bi32_t value);
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
	if (namedInitializer->GetScope() == SCOPE_GLOBAL)
	{
		mStringPool.insert("Decl");
		mStringPool.insert(namedInitializer->GetName()->GetHashedText());
	}
	ParseNodeTraverser::Visit(namedInitializer);
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
		// TODO
	}
}


void GeneratorCore::Visit(const ThisExpression *thisExpression)
{
	mResult.SetTop(GeneratorResult(GeneratorResult::CONTEXT_FRAME_POINTER_OFFSET, thisExpression->GetTypeDescriptor(), -mPointerSize));
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


void GeneratorCore::EmitPushAddress(GeneratorResult &result)
{
	ByteCode::Type &byteCode = mByteCode.GetTop();
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FRAME_POINTER_OFFSET:
		// Push the frame pointer with the accumulated offset.
		{
			const bi32_t offset = result.mTypeAndValue.GetIntValue();
			EmitOpCodeWithOffset(OPCODE_LOADFP, offset);
			result.mTypeAndValue.SetIntValue(0);
			result.mContext = GeneratorResult::CONTEXT_STACK_ADDRESS;
		}
		break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
		// There is already an address on the stack. Add the accumulated offset.
		{
			const bi32_t offset = result.mTypeAndValue.GetIntValue();
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
				result.mTypeAndValue.SetIntValue(0);
			}
		}
		break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
		case GeneratorResult::CONTEXT_STACK_VALUE:
			// Such things cannot result in an address.
			break;
	}
}


void GeneratorCore::EmitPushValue(GeneratorResult &result)
{
	const TypeDescriptor *typeDescriptor = result.mTypeAndValue.GetTypeDescriptor();
	switch (result.mContext)
	{
		case GeneratorResult::CONTEXT_FRAME_POINTER_OFFSET:
		{
			const bi32_t offset = result.mTypeAndValue.GetIntValue();
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
						EmitPushValue32(offset);
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
					EmitPushValue64(offset);
				}
				else
				{
					EmitPushValue32(offset);
				}
			}
		}
		break;

		case GeneratorResult::CONTEXT_STACK_ADDRESS:
			break;

		case GeneratorResult::CONTEXT_CONSTANT_VALUE:
			break;

		case GeneratorResult::CONTEXT_NONE:
		case GeneratorResult::CONTEXT_STACK_VALUE:
			break;
	}
}


void GeneratorCore::EmitPushValue32(bi32_t offset)
{
	ByteCode::Type &byteCode = mByteCode.GetTop();
	switch (offset)
	{
		case -16:
			byteCode.push_back(OPCODE_PUSH32_A3);
			break;
		case -12:
			byteCode.push_back(OPCODE_PUSH32_A2);
			break;
		case -8:
			byteCode.push_back(OPCODE_PUSH32_A1);
			break;
		case -4:
			byteCode.push_back(OPCODE_PUSH32_A0);
			break;
		case 0:
			byteCode.push_back(OPCODE_PUSH32_V0);
			break;
		case 4:
			byteCode.push_back(OPCODE_PUSH32_V1);
			break;
		case 8:
			byteCode.push_back(OPCODE_PUSH32_V2);
			break;
		case 12:
			byteCode.push_back(OPCODE_PUSH32_V3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH32, offset);
			break;
	}
}


void GeneratorCore::EmitPushValue64(bi32_t offset)
{
	ByteCode::Type &byteCode = mByteCode.GetTop();
	switch (offset)
	{
		case -32:
			byteCode.push_back(OPCODE_PUSH64_A3);
			break;
		case -24:
			byteCode.push_back(OPCODE_PUSH64_A2);
			break;
		case -16:
			byteCode.push_back(OPCODE_PUSH64_A1);
			break;
		case -8:
			byteCode.push_back(OPCODE_PUSH64_A0);
			break;
		case 0:
			byteCode.push_back(OPCODE_PUSH64_V0);
			break;
		case 8:
			byteCode.push_back(OPCODE_PUSH64_V1);
			break;
		case 16:
			byteCode.push_back(OPCODE_PUSH64_V2);
			break;
		case 24:
			byteCode.push_back(OPCODE_PUSH64_V3);
			break;
		default:
			EmitOpCodeWithOffset(OPCODE_PUSH64, offset);
			break;
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
				byteCode.push_back(static_cast<char>(value));
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
