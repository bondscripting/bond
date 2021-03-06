#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/systems/math.h"
#include "private/valueevaluationpass.h"

namespace Bond
{

namespace Operators
{

template <typename Operator>
Value NumericBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type, Operator op)
{
	const Value l = CastValue(lhs, type);
	const Value r = CastValue(rhs, type);
	Value resultValue;

	switch (type.GetPrimitiveType())
	{
		case Token::KEY_INT:
			resultValue.mInt = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = op(l.mUInt, r.mUInt);
			break;
		case Token::KEY_LONG:
			resultValue.mLong = op(l.mLong, r.mLong);
			break;
		case Token::KEY_ULONG:
			resultValue.mULong = op(l.mULong, r.mULong);
			break;
		case Token::KEY_FLOAT:
			resultValue.mFloat = op(l.mFloat, r.mFloat);
			break;
		case Token::KEY_DOUBLE:
			resultValue.mDouble = op(l.mDouble, r.mDouble);
			break;
		default:
			break;
	}

	return resultValue;
}


template <typename Operator>
Value IntegerBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type, Operator op)
{
	const Value l = CastValue(lhs, type);
	const Value r = CastValue(rhs, type);
	Value resultValue;

	switch (type.GetPrimitiveType())
	{
		case Token::KEY_INT:
			resultValue.mInt = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = op(l.mUInt, r.mUInt);
			break;
		case Token::KEY_LONG:
			resultValue.mLong = op(l.mLong, r.mLong);
			break;
		case Token::KEY_ULONG:
			resultValue.mULong = op(l.mULong, r.mULong);
			break;
		default:
			break;
	}

	return resultValue;
}


template <typename Operator>
Value ComparisonBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, Operator op)
{
	TypeDescriptor type = CombineOperandTypes(lhs.GetTypeDescriptor(), rhs.GetTypeDescriptor());
	const Value l = CastValue(lhs, type);
	const Value r = CastValue(rhs, type);
	Value resultValue;

	switch (type.GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			resultValue.mBool = op(l.mBool, r.mBool);
			break;
		case Token::KEY_INT:
			resultValue.mBool = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mBool = op(l.mUInt, r.mUInt);
			break;
		case Token::KEY_LONG:
			resultValue.mBool = op(l.mLong, r.mLong);
			break;
		case Token::KEY_ULONG:
			resultValue.mBool = op(l.mULong, r.mULong);
			break;
		case Token::KEY_FLOAT:
			resultValue.mBool = op(l.mFloat, r.mFloat);
			break;
		case Token::KEY_DOUBLE:
			resultValue.mBool = op(l.mDouble, r.mDouble);
			break;
		default:
			break;
	}

	return resultValue;
}


struct BinaryAddOperator { template <typename T> T operator()(T a, T b) { return a + b; } };
inline Value BinaryAdd(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryAddOperator());
}


struct BinarySubOperator { template <typename T> T operator()(T a, T b) { return a - b; } };
inline Value BinarySub(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinarySubOperator());
}


struct BinaryMultOperator { template <typename T> T operator()(T a, T b) { return a * b; } };
inline Value BinaryMult(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryMultOperator());
}


struct BinaryDivOperator { template <typename T> T operator()(T a, T b) { return a / b; } };
inline Value BinaryDiv(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryDivOperator());
}


struct BinaryModOperator { template <typename T> T operator()(T a, T b) { return a % b; } };
inline Value BinaryMod(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryModOperator());
}


struct BinaryLeftOperator { template <typename T> T operator()(T a, T b) { return a << b; } };
inline Value BinaryLeft(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryLeftOperator());
}


struct BinaryRightOperator { template <typename T> T operator()(T a, T b) { return a >> b; } };
inline Value BinaryRight(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryRightOperator());
}


struct BinaryBitAndOperator { template <typename T> T operator()(T a, T b) { return a & b; } };
inline Value BinaryBitAnd(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitAndOperator());
}


struct BinaryBitOrOperator { template <typename T> T operator()(T a, T b) { return a | b; } };
inline Value BinaryBitOr(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitOrOperator());
}


struct BinaryBitXOrOperator { template <typename T> T operator()(T a, T b) { return a ^ b; } };
inline Value BinaryBitXOr(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor &type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitXOrOperator());
}


struct BinaryLTOperator { template <typename T> bool operator()(T a, T b) { return a < b; } };
inline Value BinaryLT(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryLTOperator());
}


struct BinaryLTEOperator { template <typename T> bool operator()(T a, T b) { return a <= b; } };
inline Value BinaryLTE(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryLTEOperator());
}


struct BinaryGTOperator { template <typename T> bool operator()(T a, T b) { return a > b; } };
inline Value BinaryGT(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryGTOperator());
}


struct BinaryGTEOperator { template <typename T> bool operator()(T a, T b) { return a >= b; } };
inline Value BinaryGTE(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryGTEOperator());
}


struct BinaryEqualOperator { template <typename T> bool operator()(T a, T b) { return a == b; } };
inline Value BinaryEqual(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryEqualOperator());
}


struct BinaryNotEqualOperator { template <typename T> bool operator()(T a, T b) { return a != b; } };
inline Value BinaryNotEqual(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryNotEqualOperator());
}


Value UnaryMinus(const TypeAndValue &value)
{
	const TypeDescriptor &type = value.GetTypeDescriptor();
	Value resultValue;

	switch (type.GetPrimitiveType())
	{
		case Token::KEY_INT:
			resultValue.mInt = -value.GetIntValue();
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = uint32_t(-int32_t(value.GetUIntValue()));
			break;
		case Token::KEY_LONG:
			resultValue.mLong = -value.GetLongValue();
			break;
		case Token::KEY_ULONG:
			resultValue.mULong = uint64_t(-int64_t(value.GetULongValue()));
			break;
		case Token::KEY_FLOAT:
			resultValue.mFloat = -value.GetFloatValue();
			break;
		case Token::KEY_DOUBLE:
			resultValue.mDouble = -value.GetDoubleValue();
			break;
		default:
			break;
	}

	return resultValue;
}


Value UnaryBitNot(const TypeAndValue &value)
{
	const TypeDescriptor &type = value.GetTypeDescriptor();
	Value resultValue;

	if (value.IsValueDefined())
	{
		switch (type.GetPrimitiveType())
		{
			case Token::KEY_INT:
				resultValue.mInt = ~value.GetIntValue();
				break;
			case Token::KEY_UINT:
				resultValue.mUInt = ~value.GetUIntValue();
				break;
			case Token::KEY_LONG:
				resultValue.mLong = ~value.GetLongValue();
				break;
			case Token::KEY_ULONG:
				resultValue.mULong = ~value.GetULongValue();
				break;
			default:
				break;
		}
	}

	return resultValue;
}

}

struct AlignmentComparator
{
	explicit AlignmentComparator(PointerSize pointerSize): mPointerSize(pointerSize) {}
	bool operator()(const DeclarativeStatement &a, const DeclarativeStatement &b) const;
	PointerSize mPointerSize;
};


bool AlignmentComparator::operator()(const DeclarativeStatement &a, const DeclarativeStatement &b) const
{
	const TypeDescriptor *aType = a.GetTypeDescriptor();
	const TypeDescriptor *bType = b.GetTypeDescriptor();
	const uint32_t aAlign = aType->GetAlignment(mPointerSize);
	const uint32_t bAlign = bType->GetAlignment(mPointerSize);
	return aAlign > bAlign;
}


void ValueEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	do
	{
		mHasResolvedItems = false;
		mHasUnresolvedItems = false;
		mUnresolvedErrorBuffer.Reset();
		SemanticAnalysisPass::Analyze(translationUnitList);
	}
	while (mHasResolvedItems && mHasUnresolvedItems && !mErrorBuffer.HasErrors());

	if (!mErrorBuffer.HasErrors())
	{
		// If something has not been resolved but no errors were reported, then
		// the compiler did something wrong.
		if (mHasUnresolvedItems && !mUnresolvedErrorBuffer.HasErrors())
		{
			mErrorBuffer.PushError(CompilerError::INTERNAL_ERROR);
		}

		mErrorBuffer.CopyFrom(mUnresolvedErrorBuffer);
	}
}


void ValueEvaluationPass::Visit(EnumDeclaration *enumDeclaration)
{
	mPrevEnumerator = nullptr;
	ParseNodeTraverser::Visit(enumDeclaration);
	mPrevEnumerator = nullptr;
}


void ValueEvaluationPass::Visit(Enumerator *enumerator)
{
	TypeAndValue &tav = *enumerator->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(enumerator);

		if (enumerator->GetValue() != nullptr)
		{
			const TypeAndValue &valueTav = enumerator->GetValue()->GetTypeAndValue();
			if (valueTav.IsValueDefined())
			{
				Resolve(tav);
				const TypeDescriptor &resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(valueTav, resultType));
			}
		}
		else
		{
			if (mPrevEnumerator != nullptr)
			{
				const TypeAndValue &prevTav = *mPrevEnumerator->GetTypeAndValue();
				if (prevTav.IsValueDefined())
				{
					Resolve(tav);
					tav.SetIntValue(prevTav.GetIntValue() + 1);
				}
			}
			else
			{
				Resolve(tav);
				tav.SetIntValue(0);
			}
		}

		CheckUnresolved(tav);
	}

	mPrevEnumerator = enumerator;
}


void ValueEvaluationPass::Visit(StructDeclaration *structDeclaration)
{
	SemanticAnalysisPass::Visit(structDeclaration);
	if (!structDeclaration->IsResolved())
	{
		switch (structDeclaration->GetVariant())
		{
			case StructDeclaration::VARIANT_BOND:
			{
				bool membersResolved = true;
				DeclarativeStatement *memberList = structDeclaration->GetMemberVariableList();
				while (memberList != nullptr)
				{
					if (!memberList->GetTypeDescriptor()->IsResolved())
					{
						membersResolved = false;
						break;
					}
					memberList = NextNode(memberList);
				}

				if (membersResolved)
				{
					memberList = structDeclaration->GetMemberVariableList();
					//memberList = SortNodes<DeclarativeStatement, AlignmentComparator>(memberList, AlignmentComparator(mPointerSize));
					//structDeclaration->SetMemberVariableList(memberList);

					uint32_t structSize = 0;
					uint32_t structAlign = BOND_DEFAULT_STRUCT_ALIGN;
					while (memberList != nullptr)
					{
						const TypeDescriptor *memberDescriptor = memberList->GetTypeDescriptor();
						const uint32_t memberSize = memberDescriptor->GetSize(mPointerSize);
						const uint32_t memberAlign = memberDescriptor->GetAlignment(mPointerSize);

						structSize = AlignUp(structSize, memberAlign);
						structAlign = Max(structAlign, memberAlign);

						NamedInitializer *initializerList = memberList->GetNamedInitializerList();
						while (initializerList != nullptr)
						{
							// TODO: Ensure that the offset does not overflow.
							initializerList->SetOffset(int32_t(structSize));
							structSize += memberSize;
							initializerList = NextNode(initializerList);
						}

						memberList = NextNode(memberList);
					}

					structSize = AlignUp(Max(structSize, BOND_DEFAULT_STRUCT_SIZE), structAlign);
					structDeclaration->SetSize(structSize);
					structDeclaration->SetAlignment(structAlign);
				}
			}
			break;

			case StructDeclaration::VARIANT_NATIVE:
			{
				bool hasError = false;
				const Token *sizeToken = structDeclaration->GetSizeSpecifier()->GetSizeToken();
				const uint32_t size = CastValue(*sizeToken, Token::KEY_UINT).mUInt;

				if (IsNegativeIntegerConstant(*sizeToken) || (size < 1))
				{
					hasError = true;
					mErrorBuffer.PushError(CompilerError::INVALID_STRUCT_SIZE, sizeToken);
				}
				else
				{
					structDeclaration->SetSize(size);
				}

				const Token *alignToken = structDeclaration->GetSizeSpecifier()->GetAlignmentToken();
				if (alignToken != nullptr)
				{
					const uint32_t align = CastValue(*alignToken, Token::KEY_UINT).mUInt;
					if (IsNegativeIntegerConstant(*alignToken) || (align <= 1) || !IsPowerOfTwo(align))
					{
						hasError = true;
						mErrorBuffer.PushError(CompilerError::INVALID_STRUCT_ALIGNMENT, alignToken);
					}
					else
					{
						structDeclaration->SetAlignment(align);
					}
				}

				if (!hasError && ((structDeclaration->GetSize() % structDeclaration->GetAlignment()) != 0))
				{
					mErrorBuffer.PushError(CompilerError::STRUCT_SIZE_ALIGNMENT_MISMATCH, sizeToken);
				}
			}
			break;

			default:
				break;
		}
	}
}


void ValueEvaluationPass::Visit(FunctionDefinition *functionDefinition)
{
	ParseNodeTraverser::Visit(functionDefinition);
	TypeAndValue &tav = *functionDefinition->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		Resolve(tav);
	}
}


void ValueEvaluationPass::Visit(Parameter *parameter)
{
	ParseNodeTraverser::Visit(parameter);
	TypeAndValue &tav = *parameter->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		Resolve(tav);
	}
}


void ValueEvaluationPass::Visit(TypeDescriptor *typeDescriptor)
{
	if (!typeDescriptor->IsResolved())
	{
		ParseNodeTraverser::Visit(typeDescriptor);

		Expression *expressionList = typeDescriptor->GetLengthExpressionList();
		while (expressionList != nullptr)
		{
			// Validate the value and cast it to an unsigned integar.
			TypeAndValue &tav = expressionList->GetTypeAndValue();
			if (tav.IsResolved())
			{
				const bool isUInt = tav.GetTypeDescriptor().GetPrimitiveType() == Token::KEY_UINT;

				if (tav.IsValueDefined())
				{
					if (!isUInt)
					{
						const TypeDescriptor uintType = TypeDescriptor::GetUIntType();
						const Value length = CastValue(tav, uintType);
						tav.SetValue(length);
					}

					if (tav.GetUIntValue() == 0)
					{
						mErrorBuffer.PushError(CompilerError::ARRAY_SIZE_IS_ZERO, expressionList->GetContextToken());
					}
				}
				else
				{
					mErrorBuffer.PushError(CompilerError::ARRAY_SIZE_IS_NOT_CONST_INTEGER, expressionList->GetContextToken());
					tav.SetUIntValue(1);
				}

				if (!isUInt)
				{
					expressionList->SetTypeDescriptor(TypeDescriptor::GetUIntType());
				}
			}
			expressionList = NextNode(expressionList);
		}
	}
}


void ValueEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(namedInitializer);
		const TypeDescriptor &typeDescriptor = tav.GetTypeDescriptor();
		const Initializer *initializer = namedInitializer->GetInitializer();

		if (initializer == nullptr)
		{
			Resolve(tav);
		}
		else if (initializer->IsResolved() && typeDescriptor.IsResolved())
		{
			Resolve(tav);

			// TODO: Handle non-primitive types (e.g. arrays of primitive types or string literals).
			if (typeDescriptor.IsPrimitiveType() && typeDescriptor.IsConst())
			{
				const TypeAndValue initializerTav = initializer->GetExpression()->GetTypeAndValue();
				if (initializerTav.IsValueDefined())
				{
					tav.SetValue(CastValue(initializerTav, typeDescriptor));
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(SwitchLabel *switchLabel)
{
	ParseNodeTraverser::Visit(switchLabel);
	const Expression *expression = switchLabel->GetExpression();
	if (expression != nullptr)
	{
		const TypeAndValue &tav = expression->GetTypeAndValue();
		if (tav.IsResolved() && !tav.IsValueDefined())
		{
			mErrorBuffer.PushError(CompilerError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, expression->GetContextToken());
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	TypeDescriptor *typeDescriptor = declarativeStatement->GetTypeDescriptor();
	if (typeDescriptor->IsArrayType() &&
	    (CastNode<EmptyExpression>(typeDescriptor->GetLengthExpressionList()) != nullptr) &&
	    !typeDescriptor->GetLengthExpressionList()->GetTypeAndValue().IsValueDefined())
	{
		uint32_t length = 0;
		const NamedInitializer *current = declarativeStatement->GetNamedInitializerList();
		const TypeDescriptor elementDescriptor = typeDescriptor->GetDereferencedType();
		const bool isCharArray = elementDescriptor.IsCharType();

		while (current != nullptr)
		{
			const Initializer *initializer = current->GetInitializer();
			if (initializer != nullptr)
			{
				const Expression *expression = initializer->GetExpression();
				const Initializer *initializerList = initializer->GetInitializerList();
				if (initializerList != nullptr)
				{
					const uint32_t initializerListLength = GetLength(initializer->GetInitializerList());
					length = Max(length, initializerListLength);
				}
				else if (expression != nullptr)
				{
					const ConstantLiteralExpression *constantExpression = CastNode<ConstantLiteralExpression>(expression);
					const bool isStringInitializer =
						isCharArray &&
						(constantExpression != nullptr) &&
						constantExpression->GetTypeDescriptor().IsStringType();

					if (isStringInitializer)
					{
						const uint32_t stringLength = uint32_t(constantExpression->GetValueToken()->GetStringLength() + 1);
						length = Max(length, stringLength);
					}
				}
			}
			current = NextNode(current);
		}

		if (length == 0)
		{
			mErrorBuffer.PushError(CompilerError::ARRAY_SIZE_IS_UNSPECIFIED, typeDescriptor->GetContextToken(), typeDescriptor);
		}
		typeDescriptor->GetLengthExpressionList()->GetTypeAndValue().SetUIntValue(length);
	}
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValueEvaluationPass::Visit(ConditionalExpression *conditionalExpression)
{
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(conditionalExpression);
		const TypeAndValue &condTav = conditionalExpression->GetCondition()->GetTypeAndValue();
		const TypeAndValue &trueTav = conditionalExpression->GetTrueExpression()->GetTypeAndValue();
		const TypeAndValue &falseTav = conditionalExpression->GetFalseExpression()->GetTypeAndValue();

		if (condTav.IsResolved() && trueTav.IsResolved() && falseTav.IsResolved())
		{
			Resolve(tav);

			if (condTav.IsValueDefined() && trueTav.IsValueDefined() && falseTav.IsValueDefined())
			{
				const bool cond = condTav.GetBoolValue();
				const TypeDescriptor &resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(cond ? trueTav : falseTav, resultType));
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(BinaryExpression *binaryExpression)
{
	using namespace Bond::Operators;

	// TODO: Constant folding is currently very naive and will only perform operations on adjacent constants.
	// For example, the expression 1 + 2 will be evaluated to the constant 3. However, the expression
	// 1 + b + 2, will not be evaluated to 3 + b, since it would first require the expression to be rearranged
	// to 1 + 2 + b. Moreover, there are likely more intricate cases involving negations, subtractions and
	// divisions that could be further improved.
	TypeAndValue &tav = binaryExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(binaryExpression);
		const TypeAndValue &lhs = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhs = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhs.IsResolved() && rhs.IsResolved())
		{
			Resolve(tav);
			const Token *op = binaryExpression->GetOperator();

			if (lhs.IsValueDefined() && rhs.IsValueDefined())
			{
				const TypeDescriptor &resultType = tav.GetTypeDescriptor();

				switch (op->GetTokenType())
				{
					case Token::COMMA:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_PLUS:
						tav.SetValue(BinaryAdd(lhs, rhs, resultType));
						break;
					case Token::OP_MINUS:
						tav.SetValue(BinarySub(lhs, rhs, resultType));
						break;
					case Token::OP_STAR:
						tav.SetValue(BinaryMult(lhs, rhs, resultType));
						break;
					case Token::OP_DIV:
						tav.SetValue(BinaryDiv(lhs, rhs, resultType));
						break;
					case Token::OP_MOD:
						tav.SetValue(BinaryMod(lhs, rhs, resultType));
						break;
					case Token::OP_LEFT:
						tav.SetValue(BinaryLeft(lhs, rhs, resultType));
						break;
					case Token::OP_RIGHT:
						tav.SetValue(BinaryRight(lhs, rhs, resultType));
						break;
					case Token::OP_AMP:
						tav.SetValue(BinaryBitAnd(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_OR:
						tav.SetValue(BinaryBitOr(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_XOR:
						tav.SetValue(BinaryBitXOr(lhs, rhs, resultType));
						break;
					case Token::OP_LT:
						tav.SetValue(BinaryLT(lhs, rhs));
						break;
					case Token::OP_LTE:
						tav.SetValue(BinaryLTE(lhs, rhs));
						break;
					case Token::OP_GT:
						tav.SetValue(BinaryGT(lhs, rhs));
						break;
					case Token::OP_GTE:
						tav.SetValue(BinaryGTE(lhs, rhs));
						break;
					case Token::OP_EQUAL:
						tav.SetValue(BinaryEqual(lhs, rhs));
						break;
					case Token::OP_NOT_EQUAL:
						tav.SetValue(BinaryNotEqual(lhs, rhs));
						break;
					case Token::OP_AND:
						tav.SetBoolValue(lhs.GetBoolValue() && rhs.GetBoolValue());
						break;
					case Token::OP_OR:
						tav.SetBoolValue(lhs.GetBoolValue() || rhs.GetBoolValue());
						break;
					default:
						break;
				}
			}
			else if (lhs.IsValueDefined())
			{
				switch (op->GetTokenType())
				{
					case Token::OP_AND:
						if (!lhs.GetBoolValue())
						{
							tav.SetBoolValue(false);
						}
						break;
					case Token::OP_OR:
						if (lhs.GetBoolValue())
						{
							tav.SetBoolValue(true);
						}
						break;
					default:
						break;
				}
			}
			else if (rhs.IsValueDefined())
			{
				switch (op->GetTokenType())
				{
					case Token::OP_AND:
						if (!rhs.GetBoolValue())
						{
							tav.SetBoolValue(false);
						}
						break;
					case Token::OP_OR:
						if (rhs.GetBoolValue())
						{
							tav.SetBoolValue(true);
						}
						break;
					default:
						break;
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	using namespace Bond::Operators;

	TypeAndValue &tav = unaryExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsResolved())
		{
			Resolve(tav);

			if (rhs.IsValueDefined())
			{
				const Token *op = unaryExpression->GetOperator();

				switch (op->GetTokenType())
				{
					case Token::OP_PLUS:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_MINUS:
						tav.SetValue(UnaryMinus(rhs));
						break;
					case Token::OP_NOT:
						tav.SetBoolValue(!rhs.GetBoolValue());
						break;
					case Token::OP_BIT_NOT:
						tav.SetValue(UnaryBitNot(rhs));
						break;
					case Token::OP_STAR:
						// TODO
						break;
					default:
						break;
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(PostfixExpression *postfixExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = postfixExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(postfixExpression);
		const TypeAndValue &lhs = postfixExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(MemberExpression *memberExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = memberExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(memberExpression);
		const TypeAndValue &lhs = memberExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	// TODO: A constant index into a constant array could be be evaluated at compile time.
	// Mark as resolved when array and index are resolved.
	TypeAndValue &tav = arraySubscriptExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(arraySubscriptExpression);
		const TypeAndValue &lhs = arraySubscriptExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &indexTav = arraySubscriptExpression->GetIndex()->GetTypeAndValue();
		if (lhs.IsResolved() && indexTav.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when lhs and all arguments are resolved.
	TypeAndValue &tav = functionCallExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(functionCallExpression);
		const TypeAndValue &lhs = functionCallExpression->GetLhs()->GetTypeAndValue();
		if (!lhs.IsResolved())
		{
			return;
		}

		const Expression *argument = functionCallExpression->GetArgumentList();
		while (argument != nullptr)
		{
			const TypeAndValue &argTav = argument->GetTypeAndValue();
			if (!argTav.IsResolved())
			{
				return;
			}
			argument = NextNode(argument);
		}
		Resolve(tav);
	}
}


void ValueEvaluationPass::Visit(CastExpression *castExpression)
{
	TypeAndValue &tav = castExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(castExpression);
		const TypeAndValue &rhs = castExpression->GetRhs()->GetTypeAndValue();
		if (rhs.IsResolved())
		{
			Resolve(tav);
			if (rhs.IsValueDefined())
			{
				const TypeDescriptor &resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(rhs, resultType));
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(PropertyofExpression *propertyofExpression)
{
	TypeAndValue &tav = propertyofExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(propertyofExpression);
		const TypeDescriptor *typeDescriptor = nullptr;

		if (propertyofExpression->GetRhs() != nullptr)
		{
			const TypeAndValue &rhTav = propertyofExpression->GetRhs()->GetTypeAndValue();
			if (rhTav.IsResolved())
			{
				typeDescriptor = &rhTav.GetTypeDescriptor();
			}
		}
		else
		{
			typeDescriptor = propertyofExpression->GetTargetTypeDescriptor();
		}

		if ((typeDescriptor != nullptr) && typeDescriptor->IsResolved())
		{
			Resolve(tav);
			if (propertyofExpression->IsAlignof())
			{
				tav.SetUIntValue(typeDescriptor->GetAlignment(mPointerSize));
			}
			else
			{
				tav.SetUIntValue(typeDescriptor->GetSize(mPointerSize));
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(ConstantLiteralExpression *constantExpression)
{
	TypeAndValue &tav = constantExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const Token *token = constantExpression->GetValueToken();
		Resolve(tav);
		tav.SetValue(token->GetValue());
	}
}


void ValueEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	TypeAndValue &tav = identifierExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const TypeAndValue &definitionTav = *identifierExpression->GetDefinition()->GetTypeAndValue();
		if (definitionTav.IsResolved())
		{
			Resolve(tav);
			if (definitionTav.IsValueDefined())
			{
				tav.SetValue(definitionTav.GetValue());
			}
		}
		else
		{
			mUnresolvedErrorBuffer.PushError(CompilerError::CANNOT_RESOLVE_SYMBOL_VALUE, identifierExpression->GetContextToken());
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(ThisExpression *thisExpression)
{
	TypeAndValue &tav = thisExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		Resolve(tav);
	}
}


void ValueEvaluationPass::Resolve(TypeAndValue &tav)
{
	tav.Resolve();
	mHasResolvedItems = true;
}


void ValueEvaluationPass::CheckUnresolved(const TypeAndValue &tav)
{
	mHasUnresolvedItems = mHasUnresolvedItems || !tav.IsResolved();
}

}
