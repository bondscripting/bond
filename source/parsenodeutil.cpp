#include "bond/parsenodeutil.h"
#include "bond/symboltable.h"

namespace Bond
{

bu32_t GetLength(const ListParseNode *list)
{
	bu32_t length = 0;
	const ListParseNode *current = list;
	while (current != NULL)
	{
		++length;
		current = current->GetNextNode();
	}
	return length;
}


bool AreComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
	return
		(typeA->IsPointerType() && typeB->IsPointerType()) ||
		(typeA->IsNumericType() && typeB->IsNumericType()) ||
		(typeA->IsBooleanType() && typeB->IsBooleanType());
}


bool AreConvertibleTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType)
{
	return AreComparableTypes(fromType, toType);
}


TypeDescriptor PromoteType(const TypeDescriptor *type)
{
	const Token::TokenType t = type->GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (type->IsPointerType())
	{
		result = *type;
		result.ConvertToPointerIntrinsic();
		result.SetRValue();
	}
	else if (t == Token::KEY_FLOAT)
	{
		result = TypeDescriptor::GetFloatType();
	}
	else if (SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(t))
	{
		result = TypeDescriptor::GetIntType();
	}
	else if (UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(t))
	{
		result = TypeDescriptor::GetUIntType();
	}
	else if (t == Token::KEY_BOOL)
	{
		result = TypeDescriptor::GetBoolType();
	}

	return result;
}


TypeDescriptor CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
 	const Token::TokenType a = typeA->GetPrimitiveType();
	const Token::TokenType b = typeB->GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (typeA->IsPointerType())
	{
		result = *typeA;
		result.ConvertToPointerIntrinsic();
		result.SetRValue();
	}
	else if (typeB->IsPointerType())
	{
		result = *typeB;
		result.ConvertToPointerIntrinsic();
		result.SetRValue();
	}
	else if ((a == Token::KEY_FLOAT) || (b == Token::KEY_FLOAT))
	{
		result = TypeDescriptor::GetFloatType();
	}
	else if (SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	         SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
	{
		result = TypeDescriptor::GetIntType();
	}
	else if (UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	         UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
	{
		result = TypeDescriptor::GetUIntType();
	}
	else if ((a == Token::KEY_BOOL) || (b == Token::KEY_BOOL))
	{
		result = TypeDescriptor::GetBoolType();
	}

	return result;
}


Value CastValue(const Value &value, Token::TokenType sourceType, Token::TokenType destType)
{
	Value resultValue;

	switch (destType)
	{
		case Token::KEY_BOOL:
		case Token::CONST_BOOL:
			switch (sourceType)
			{
				case Token::KEY_BOOL:
				case Token::CONST_BOOL:
					resultValue.mBool = static_cast<bool>(value.mBool);
					break;
				default:
					break;
			}

		case Token::KEY_CHAR:
		case Token::CONST_CHAR:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mInt = static_cast<char>(value.mInt);
					break;
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<char>(value.mUInt);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<char>(value.mFloat);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_FLOAT:
		case Token::CONST_FLOAT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mFloat = static_cast<bf32_t>(value.mInt);
					break;
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mFloat = static_cast<bf32_t>(value.mUInt);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mFloat = static_cast<bf32_t>(value.mFloat);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_INT:
		case Token::CONST_INT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mInt = static_cast<bi32_t>(value.mInt);
					break;
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<bi32_t>(value.mUInt);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<bi32_t>(value.mFloat);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
		case Token::CONST_UINT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mUInt = static_cast<bu32_t>(value.mInt);
					break;
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = static_cast<bu32_t>(value.mUInt);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = static_cast<bu32_t>(value.mFloat);
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}

	return resultValue;
}


Value CastValue(const TypeAndValue &value, const TypeDescriptor *destType)
{
	// This function assumes that it is only called for valid casts on defined values.
	Token::TokenType sourceType = value.GetTypeDescriptor()->GetPrimitiveType();
	return CastValue(value.GetValue(), sourceType, destType->GetPrimitiveType());
}


Value UnaryMinus(const TypeAndValue &value)
{
	const TypeDescriptor *type = value.GetTypeDescriptor();
	Value resultValue;

	switch (type->GetPrimitiveType())
	{
		case Token::KEY_FLOAT:
			resultValue.mFloat = -value.GetFloatValue();
			break;
		case Token::KEY_INT:
			resultValue.mInt = -value.GetIntValue();
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = -value.GetUIntValue();
			break;
		default:
			break;
	}

	return resultValue;
}


Value UnaryBitNot(const TypeAndValue &value)
{
	const TypeDescriptor *type = value.GetTypeDescriptor();
	Value resultValue;

	if (value.IsValueDefined())
	{
		switch (type->GetPrimitiveType())
		{
			case Token::KEY_INT:
				resultValue.mInt = ~value.GetIntValue();
				break;
			case Token::KEY_UINT:
				resultValue.mUInt = ~value.GetUIntValue();
				break;
			default:
				break;
		}
	}

	return resultValue;
}

}
