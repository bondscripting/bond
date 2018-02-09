#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/symboltable.h"

namespace Bond
{

uint32_t GetLength(const ListParseNode *list)
{
	uint32_t length = 0;
	const ListParseNode *current = list;
	while (current != nullptr)
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
		 (fromType->GetTypeSpecifier()->GetDefinition() != nullptr) &&
		 (toType->GetTypeSpecifier()->GetDefinition() != nullptr) &&
		 (fromType->GetTypeSpecifier()->GetDefinition() == toType->GetTypeSpecifier()->GetDefinition()));
}


bool AreSameTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType)
{
	if (fromType->IsConst() != toType->IsConst())
	{
		return false;
	}
	else if (fromType->IsPointerType() && toType->IsPointerType())
	{
		if (fromType->IsArrayType() && toType->IsArrayType())
		{
			const uint32_t fromLength = fromType->GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
			const uint32_t toLength = toType->GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
			if (fromLength != toLength)
			{
				return false;
			}
		}
		const TypeDescriptor fromParent = fromType->GetDereferencedType();
		const TypeDescriptor toParent = toType->GetDereferencedType();
		return AreSameTypes(&fromParent, &toParent);
	}
	else if (fromType->IsValueType() && toType->IsValueType())
	{
		return
			(fromType->GetPrimitiveType() == toType->GetPrimitiveType()) &&
			(fromType->GetTypeSpecifier()->GetDefinition() == toType->GetTypeSpecifier()->GetDefinition());
	}
	return false;
}


bool AreAssignableTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType)
{
	if (!toType->IsAssignable())
	{
		return false;
	}
	if (fromType->IsPointerType() && toType->IsPointerType())
	{
		if (fromType->IsImplicitlyConvertiblePointerType() || toType->IsVoidPointerType())
		{
			return true;
		}
		else
		{
			const TypeDescriptor fromParent = fromType->GetDereferencedType();
			const TypeDescriptor toParent = toType->GetDereferencedType();
			return AreSameTypes(&fromParent, &toParent);
		}
	}
	else if (fromType->IsValueType() && toType->IsValueType())
	{
		return AreConvertibleTypes(fromType, toType);
	}
	return false;
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
	else if (SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	         SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
	{
		if (LONG_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	      LONG_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
		{
			result = TypeDescriptor::GetLongType();
		}
		else
		{
			result = TypeDescriptor::GetIntType();
		}
	}
	else if (UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	         UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
	{
		if (LONG_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(a) ||
	      LONG_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(b))
		{
			result = TypeDescriptor::GetULongType();
		}
		else
		{
			result = TypeDescriptor::GetUIntType();
		}
	}
	else if ((a == Token::KEY_BOOL) || (b == Token::KEY_BOOL))
	{
		result = TypeDescriptor::GetBoolType();
	}
	else if (AreConvertibleTypes(typeA, typeB))
	{
		result = *typeA;
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
					resultValue.mInt = int8_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = int8_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = int8_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = int8_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = int8_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = int8_t(value.mDouble);
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
					resultValue.mUInt = uint8_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = uint8_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = uint8_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = uint8_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = uint8_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = uint8_t(value.mDouble);
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
					resultValue.mInt = int16_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = int16_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = int16_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = int16_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = int16_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = int16_t(value.mDouble);
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
					resultValue.mUInt = uint16_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = uint16_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = uint16_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = uint16_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = uint16_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = uint16_t(value.mDouble);
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
					resultValue.mInt = int32_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = int32_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mInt = int32_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mInt = int32_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = int32_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mInt = int32_t(value.mDouble);
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
					resultValue.mUInt = uint32_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = uint32_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mUInt = uint32_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mUInt = uint32_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = uint32_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mUInt = uint32_t(value.mDouble);
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
					resultValue.mLong = int64_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mLong = int64_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mLong = int64_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mLong = int64_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mLong = int64_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mLong = int64_t(value.mDouble);
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
					resultValue.mULong = uint64_t(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mULong = uint64_t(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mULong = uint64_t(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mULong = uint64_t(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mULong = uint64_t(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mULong = uint64_t(value.mDouble);
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
					resultValue.mFloat = float(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mFloat = float(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mFloat = float(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mFloat = float(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mFloat = float(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mFloat = float(value.mDouble);
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
					resultValue.mDouble = double(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mDouble = double(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mDouble = double(value.mLong);
					break;
				case Token::KEY_ULONG:
				case Token::CONST_ULONG:
					resultValue.mDouble = double(value.mULong);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mDouble = double(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
				case Token::CONST_DOUBLE:
					resultValue.mDouble = double(value.mDouble);
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


bool IsNegativeIntegerConstant(const Token *token)
{
	return
		SIGNED_INTEGER_CONSTANTS_TYPESET.Contains(token->GetTokenType()) &&
		(CastValue(token->GetValue(), token->GetTokenType(), Token::CONST_INT).mInt < 0);
}

}
