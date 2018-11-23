#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/symboltable.h"

namespace Bond
{

Value CastValue(const Value &value, Token::TokenType fromType, Token::TokenType toType);

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


Token::TokenType GetPrimitiveType(const Token &token)
{
	// Assumes that the token is a literal value.
	switch (token.GetTokenType())
	{
		case Token::CONST_BOOL:
			return Token::KEY_BOOL;
		case Token::CONST_CHAR:
			return Token::KEY_CHAR;
		case Token::CONST_INT:
			return Token::KEY_INT;
		case Token::CONST_UINT:
			return Token::KEY_UINT;
		case Token::CONST_LONG:
			return Token::KEY_LONG;
		case Token::CONST_ULONG:
			return Token::KEY_ULONG;
		case Token::CONST_FLOAT:
			return Token::KEY_FLOAT;
		case Token::CONST_DOUBLE:
			return Token::KEY_DOUBLE;
		default:
			break;
	}
	return Token::INVALID;
}


bool AreComparableTypes(const TypeDescriptor &typeA, const TypeDescriptor &typeB)
{
	return
		(typeA.IsPointerType() && typeB.IsPointerType()) ||
		(typeA.IsNumericType() && typeB.IsNumericType()) ||
		(typeA.IsBooleanType() && typeB.IsBooleanType());
}


bool AreConvertibleTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType)
{
	return
		AreComparableTypes(fromType, toType) ||
		(fromType.IsValueType() &&
		 toType.IsValueType() &&
		 (fromType.GetTypeSpecifier()->GetDefinition() != nullptr) &&
		 (toType.GetTypeSpecifier()->GetDefinition() != nullptr) &&
		 (fromType.GetTypeSpecifier()->GetDefinition() == toType.GetTypeSpecifier()->GetDefinition()));
}


bool AreSameTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType)
{
	if (fromType.IsConst() && !toType.IsConst())
	{
		return false;
	}
	else if (fromType.IsPointerType() && toType.IsPointerType())
	{
		if (fromType.IsArrayType() && toType.IsArrayType())
		{
			const uint32_t fromLength = fromType.GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
			const uint32_t toLength = toType.GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
			if (fromLength != toLength)
			{
				return false;
			}
		}
		const TypeDescriptor fromParent = fromType.GetDereferencedType();
		const TypeDescriptor toParent = toType.GetDereferencedType();
		return AreSameTypes(fromParent, toParent);
	}
	else if (fromType.IsValueType() && toType.IsValueType())
	{
		return
			(fromType.GetPrimitiveType() == toType.GetPrimitiveType()) &&
			(fromType.GetTypeSpecifier()->GetDefinition() == toType.GetTypeSpecifier()->GetDefinition());
	}
	return false;
}


bool AreAssignableTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType)
{
	if (!toType.IsAssignable())
	{
		return false;
	}
	if (fromType.IsPointerType() && toType.IsPointerType())
	{
		if (fromType.IsImplicitlyConvertiblePointerType() || toType.IsVoidPointerType())
		{
			return true;
		}
		else
		{
			const TypeDescriptor fromParent = fromType.GetDereferencedType();
			const TypeDescriptor toParent = toType.GetDereferencedType();
			return AreSameTypes(fromParent, toParent);
		}
	}
	else if (fromType.IsValueType() && toType.IsValueType())
	{
		return AreConvertibleTypes(fromType, toType);
	}
	return false;
}


bool IsLossyConversion(Token::TokenType fromType, Token::TokenType toType)
{
	switch (toType)
	{
		case Token::KEY_CHAR:
			switch (fromType)
			{
				case Token::KEY_CHAR:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_UCHAR:
			switch (fromType)
			{
				case Token::KEY_UCHAR:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_SHORT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_UCHAR:
				case Token::KEY_SHORT:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_USHORT:
			switch (fromType)
			{
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_INT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_UCHAR:
				case Token::KEY_SHORT:
				case Token::KEY_USHORT:
				case Token::KEY_INT:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_UINT:
			switch (fromType)
			{
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return false;
				default:
					return true;
			}
			break;

		case Token::KEY_LONG:
			switch (fromType)
			{
				case Token::KEY_ULONG:
				case Token::KEY_FLOAT:
				case Token::KEY_DOUBLE:
					return true;
				default:
					return false;
			}
			break;

		case Token::KEY_ULONG:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
				case Token::KEY_LONG:
				case Token::KEY_FLOAT:
				case Token::KEY_DOUBLE:
					return true;
				default:
					return false;
			}
			break;
		case Token::KEY_FLOAT:
			switch (fromType)
			{
				case Token::KEY_INT:
				case Token::KEY_UINT:
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
				case Token::KEY_DOUBLE:
					return true;
				default:
					return false;
			}
			break;

		case Token::KEY_DOUBLE:
			switch (fromType)
			{
				case Token::KEY_LONG:
				case Token::KEY_ULONG:
					return true;
				default:
					return false;
			}
			break;

		default:
			break;
	}
	return false;
}


bool IsLossyConversion(const Value &value, Token::TokenType fromType, Token::TokenType toType)
{
	switch (toType)
	{
		case Token::KEY_CHAR:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<int8_t>(value.mInt);
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<int8_t>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<int8_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<int8_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<int8_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<int8_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_UCHAR:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<uint8_t>(value.mInt);
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<uint8_t>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<uint8_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<uint8_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<uint8_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<uint8_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_SHORT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<int16_t>(value.mInt);
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<int16_t>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<int16_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<int16_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<int16_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<int16_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_USHORT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<uint16_t>(value.mInt);
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<uint16_t>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<uint16_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<uint16_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<uint16_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<uint16_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_INT:
			switch (fromType)
			{
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<int32_t>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<int32_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<int32_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<int32_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<int32_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<uint32_t>(value.mInt);
				case Token::KEY_LONG:
					return !IsInRange<uint32_t>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<uint32_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<uint32_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<uint32_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_LONG:
			switch (fromType)
			{
				case Token::KEY_ULONG:
					return !IsInRange<int64_t>(value.mULong);
				case Token::KEY_FLOAT:
					return !IsInRange<int64_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<int64_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_ULONG:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<uint64_t>(value.mInt);
				case Token::KEY_LONG:
					return !IsInRange<uint64_t>(value.mLong);
				case Token::KEY_FLOAT:
					return !IsInRange<uint64_t>(value.mFloat);
				case Token::KEY_DOUBLE:
					return !IsInRange<uint64_t>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_FLOAT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					return !IsInRange<float>(value.mInt);
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					return !IsInRange<float>(value.mUInt);
				case Token::KEY_LONG:
					return !IsInRange<float>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<float>(value.mULong);
				case Token::KEY_DOUBLE:
					return !IsInRange<float>(value.mDouble);
				default:
					break;
			}
			break;

		case Token::KEY_DOUBLE:
			switch (fromType)
			{
				case Token::KEY_LONG:
					return !IsInRange<double>(value.mLong);
				case Token::KEY_ULONG:
					return !IsInRange<double>(value.mULong);
				default:
					break;
			}
			break;

		default:
			break;
	}
	return false;
}


bool IsLossyConversion(const TypeAndValue &fromTav, const TypeDescriptor &toType)
{
	const TypeDescriptor &fromType = fromTav.GetTypeDescriptor();
	if (fromType.IsNumericType() && toType.IsNumericType())
	{
		if (fromTav.IsValueDefined())
		{
			return IsLossyConversion(fromTav.GetValue(), fromType.GetPrimitiveType(), toType.GetPrimitiveType());
		}
		else
		{
			return IsLossyConversion(fromType.GetPrimitiveType(), toType.GetPrimitiveType());
		}
	}
	return false;
}


TypeDescriptor PromoteType(const TypeDescriptor &type)
{
	const Token::TokenType t = type.GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (type.IsPointerType())
	{
		result = type;
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


TypeDescriptor CombineOperandTypes(const TypeDescriptor &typeA, const TypeDescriptor &typeB)
{
 	const Token::TokenType a = typeA.GetPrimitiveType();
	const Token::TokenType b = typeB.GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (typeA.IsPointerType())
	{
		result = typeA;
		result.ConvertToPointerIntrinsic();
		result.ClearLValue();
	}
	else if (typeB.IsPointerType())
	{
		result = typeB;
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
		result = typeA;
	}

	return result;
}


Value CastValue(const Value &value, Token::TokenType fromType, Token::TokenType toType)
{
	Value resultValue;

	switch (toType)
	{
		case Token::KEY_BOOL:
			switch (fromType)
			{
				case Token::KEY_BOOL:
					resultValue.mBool = static_cast<bool>(value.mBool);
					break;
				default:
					break;
			}

		case Token::KEY_CHAR:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mInt = static_cast<int8_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mInt = static_cast<int8_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mInt = static_cast<int8_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mInt = static_cast<int8_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mInt = static_cast<int8_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mInt = static_cast<int8_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UCHAR:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mUInt = static_cast<uint8_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mUInt = static_cast<uint8_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mUInt = static_cast<uint8_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mUInt = static_cast<uint8_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mUInt = static_cast<uint8_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mUInt = static_cast<uint8_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_SHORT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mInt = static_cast<int16_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mInt = static_cast<int16_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mInt = static_cast<int16_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mInt = static_cast<int16_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mInt = static_cast<int16_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mInt = static_cast<int16_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_USHORT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mUInt = static_cast<uint16_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mUInt = static_cast<uint16_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mUInt = static_cast<uint16_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mUInt = static_cast<uint16_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mUInt = static_cast<uint16_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mUInt = static_cast<uint16_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_INT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mInt = static_cast<int32_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mInt = static_cast<int32_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mInt = static_cast<int32_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mInt = static_cast<int32_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mInt = static_cast<int32_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mInt = static_cast<int32_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mUInt = static_cast<uint32_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mUInt = static_cast<uint32_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mUInt = static_cast<uint32_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mUInt = static_cast<uint32_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mUInt = static_cast<uint32_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mUInt = static_cast<uint32_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_LONG:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mLong = static_cast<int64_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mLong = static_cast<int64_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mLong = static_cast<int64_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mLong = static_cast<int64_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mLong = static_cast<int64_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mLong = static_cast<int64_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_ULONG:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mULong = static_cast<uint64_t>(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mULong = static_cast<uint64_t>(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mULong = static_cast<uint64_t>(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mULong = static_cast<uint64_t>(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mULong = static_cast<uint64_t>(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mULong = static_cast<uint64_t>(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_FLOAT:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mFloat = float(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mFloat = float(value.mUInt);
					break;
				case Token::KEY_LONG:
					resultValue.mFloat = float(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mFloat = float(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mFloat = float(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
					resultValue.mFloat = float(value.mDouble);
					break;
				default:
					break;
			}
			break;

		case Token::KEY_DOUBLE:
			switch (fromType)
			{
				case Token::KEY_CHAR:
				case Token::KEY_SHORT:
				case Token::KEY_INT:
					resultValue.mDouble = double(value.mInt);
					break;
				case Token::KEY_UCHAR:
				case Token::KEY_USHORT:
				case Token::KEY_UINT:
					resultValue.mDouble = double(value.mUInt);
					break;
				case Token::KEY_LONG:
				case Token::CONST_LONG:
					resultValue.mDouble = double(value.mLong);
					break;
				case Token::KEY_ULONG:
					resultValue.mDouble = double(value.mULong);
					break;
				case Token::KEY_FLOAT:
					resultValue.mDouble = double(value.mFloat);
					break;
				case Token::KEY_DOUBLE:
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


Value CastValue(const Token &fromValue, Token::TokenType toType)
{
	// This function assumes that it is only called for tokens that are constant literals.
	return CastValue(fromValue.GetValue(), GetPrimitiveType(fromValue), toType);
}


Value CastValue(const TypeAndValue &fromTav, const TypeDescriptor &toType)
{
	// This function assumes that it is only called for valid casts on defined values.
	return CastValue(fromTav.GetValue(), fromTav.GetTypeDescriptor().GetPrimitiveType(), toType.GetPrimitiveType());
}


bool IsNegativeIntegerConstant(const Token &token)
{
	return
		SIGNED_INTEGER_CONSTANTS_TYPESET.Contains(token.GetTokenType()) &&
		(CastValue(token, Token::KEY_INT).mInt < 0);
}

}
