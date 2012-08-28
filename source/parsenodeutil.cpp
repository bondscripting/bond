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
	return
		AreComparableTypes(fromType, toType) ||
		(fromType->IsValueType() &&
		 toType->IsValueType() &&
		 (fromType->GetTypeSpecifier()->GetDefinition() != NULL) &&
		 (toType->GetTypeSpecifier()->GetDefinition() != NULL) &&
		 (fromType->GetTypeSpecifier()->GetDefinition() == toType->GetTypeSpecifier()->GetDefinition()));
}


TypeDescriptor PromoteType(const TypeDescriptor *type)
{
	const Token::TokenType t = type->GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (type->IsPointerType())
	{
		result = *type;
		result.ConvertToPointerIntrinsic();
		result.ClearLValue();
	}
	else if (t == Token::KEY_FLOAT)
	{
		result = TypeDescriptor::GetFloatType();
	}
	else if (t == Token::KEY_DOUBLE)
	{
		result = TypeDescriptor::GetDoubleType();
	}
	else if (t == Token::KEY_LONG)
	{
		result = TypeDescriptor::GetLongType();
	}
	else if (t == Token::KEY_ULONG)
	{
		result = TypeDescriptor::GetULongType();
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

	// TODO: Revise this function using implicit conversion rules.
	if (typeA->IsPointerType())
	{
		result = *typeA;
		result.ConvertToPointerIntrinsic();
		result.ClearLValue();
	}
	else if (typeB->IsPointerType())
	{
		result = *typeB;
		result.ConvertToPointerIntrinsic();
		result.ClearLValue();
	}
	else if ((a == Token::KEY_DOUBLE) || (b == Token::KEY_DOUBLE))
	{
		result = TypeDescriptor::GetDoubleType();
	}
	else if ((a == Token::KEY_FLOAT) || (b == Token::KEY_FLOAT))
	{
		result = TypeDescriptor::GetFloatType();
	}
	else if ((a == Token::KEY_LONG) || (b == Token::KEY_LONG))
	{
		result = TypeDescriptor::GetLongType();
	}
	else if ((a == Token::KEY_ULONG) || (b == Token::KEY_ULONG))
	{
		result = TypeDescriptor::GetULongType();
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
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<char>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = static_cast<char>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = static_cast<char>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<char>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = static_cast<char>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UCHAR:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mUInt = static_cast<unsigned char>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = static_cast<unsigned char>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = static_cast<unsigned char>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = static_cast<unsigned char>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = static_cast<unsigned char>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = static_cast<unsigned char>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_SHORT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mInt = static_cast<bi16_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<bi16_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = static_cast<bi16_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = static_cast<bi16_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<bi16_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = static_cast<bi16_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_USHORT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mUInt = static_cast<bu16_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = static_cast<bu16_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = static_cast<bu16_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = static_cast<bu16_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = static_cast<bu16_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = static_cast<bu16_t>(value.mDouble);
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
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<bi32_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = static_cast<bi32_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = static_cast<bi32_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<bi32_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = static_cast<bi32_t>(value.mDouble);
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
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = static_cast<bu32_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = static_cast<bu32_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = static_cast<bu32_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = static_cast<bu32_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = static_cast<bu32_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_LONG:
		case Token::CONST_LONG:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mLong = static_cast<bi64_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mLong = static_cast<bi64_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mLong = static_cast<bi64_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mLong = static_cast<bi64_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mLong = static_cast<bi64_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mLong = static_cast<bi64_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_ULONG:
		case Token::CONST_ULONG:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mULong = static_cast<bu64_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mULong = static_cast<bu64_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mULong = static_cast<bu64_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mULong = static_cast<bu64_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mULong = static_cast<bu64_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mULong = static_cast<bu64_t>(value.mDouble);
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
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mFloat = static_cast<bf32_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mFloat = static_cast<bf32_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mFloat = static_cast<bf32_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mFloat = static_cast<bf32_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mFloat = static_cast<bf32_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_DOUBLE:
		case Token::CONST_DOUBLE:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
				case Token::CONST_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mDouble = static_cast<bf64_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mDouble = static_cast<bf64_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mDouble = static_cast<bf64_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mDouble = static_cast<bf64_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mDouble = static_cast<bf64_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mDouble = static_cast<bf64_t>(value.mDouble);
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
	const Token::TokenType sourceType = value.GetTypeDescriptor()->GetPrimitiveType();
	return CastValue(value.GetValue(), sourceType, destType->GetPrimitiveType());
}


Value UnaryMinus(const TypeAndValue &value)
{
	const TypeDescriptor *type = value.GetTypeDescriptor();
	Value resultValue;

	switch (type->GetPrimitiveType())
	{
		case Token::KEY_INT:
			resultValue.mInt = -value.GetIntValue();
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = -static_cast<bi32_t>(value.GetUIntValue());
			break;
		case Token::KEY_LONG:
			resultValue.mLong = -value.GetLongValue();
			break;
		case Token::KEY_ULONG:
			resultValue.mULong = -static_cast<bi64_t>(value.GetULongValue());
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
