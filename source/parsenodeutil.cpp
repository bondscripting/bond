#include "bond/parsenodeutil.h"
#include "bond/symboltable.h"

namespace Bond
{

bu32_t GetLength(const ListParseNode *list)
{
	bu32_t length = 0;
	const ListParseNode *current = list;
	while (current != 0)
	{
		++length;
		current = current->GetNextNode();
	}
	return length;
}

	/*
bool AreMatchingTypes(const TypeSpecifier *typeA, const TypeSpecifier *typeB)
{
	// This function can only be used once type specifiers have been resolved.
	if ((typeA != 0) && (typeB != 0))
	{
		const Token *primitiveTypeA = typeA->GetPrimitiveTypeToken();
		const Token *primitiveTypeB = typeB->GetPrimitiveTypeToken();

		if ((primitiveTypeA != 0) && (primitiveTypeA != 0))
		{
			return primitiveTypeA->GetTokenType() == primitiveTypeB->GetTokenType();
		}
		else if (!((primitiveTypeA == 0) && (primitiveTypeB == 0)))
		{
			return false;
		}

		return typeA->GetDefinition() == typeB->GetDefinition();
	}

	return ((typeA == 0) && (typeB == 0));
}


bool AreMatchingTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
	// This function can only be used once type specifiers have been resolved.
	while ((typeA != 0) && (typeB != 0))
	{
		if (typeA->GetVariant() != typeB->GetVariant())
		{
			return false;
		}
		if (typeA->IsConst() != typeB->IsConst())
		{
			return false;
		}
		if (!AreMatchingTypes(typeA->GetTypeSpecifier(), typeB->GetTypeSpecifier()))
		{
			return false;
		}
		// TODO: Test length.

		typeA = typeA->GetParent();
		typeB = typeB->GetParent();
	}

	return (typeA == 0) && (typeB == 0);
}
	*/

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


TypeDescriptor CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
 	const Token::TokenType a = typeA->GetPrimitiveType();
	const Token::TokenType b = typeB->GetPrimitiveType();
	TypeDescriptor result = TypeDescriptor::GetIntType();

	if (typeA->IsPointerType())
	{
		result = *typeA;
		result.ConvertToPointerIntrinsic();
	}

	else if (typeB->IsPointerType())
	{
		result = *typeB;
		result.ConvertToPointerIntrinsic();
	}

	else if ((a == Token::KEY_FLOAT) || (b == Token::KEY_FLOAT))
	{
		result = TypeDescriptor::GetFloatType();
	}
	else if ((a == Token::KEY_INT) || (b == Token::KEY_INT) ||
	         (a == Token::KEY_CHAR) || (b == Token::KEY_CHAR))
	{
		result = TypeDescriptor::GetIntType();
	}
	else if ((a == Token::KEY_UINT) || (b == Token::KEY_UINT))
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
					resultValue.mChar = static_cast<char>(value.mChar);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mChar = static_cast<char>(value.mFloat);
					break;
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mChar = static_cast<char>(value.mInt);
					break;
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mChar = static_cast<char>(value.mUInt);
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
					resultValue.mFloat = static_cast<bf32_t>(value.mChar);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mFloat = static_cast<bf32_t>(value.mFloat);
					break;
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mFloat = static_cast<bf32_t>(value.mInt);
					break;
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mFloat = static_cast<bf32_t>(value.mUInt);
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
					resultValue.mInt = static_cast<bi32_t>(value.mChar);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mInt = static_cast<bi32_t>(value.mFloat);
					break;
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mInt = static_cast<bi32_t>(value.mInt);
					break;
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mInt = static_cast<bi32_t>(value.mUInt);
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
					resultValue.mUInt = static_cast<bu32_t>(value.mChar);
					break;
				case Token::KEY_FLOAT:
				case Token::CONST_FLOAT:
					resultValue.mUInt = static_cast<bu32_t>(value.mFloat);
					break;
				case Token::KEY_INT:
				case Token::CONST_INT:
					resultValue.mUInt = static_cast<bu32_t>(value.mInt);
					break;
				case Token::KEY_UINT:
				case Token::CONST_UINT:
					resultValue.mUInt = static_cast<bu32_t>(value.mUInt);
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
		case Token::KEY_CHAR:
			resultValue.mChar = -value.GetCharValue();
			break;
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
			case Token::KEY_CHAR:
				resultValue.mChar = ~value.GetCharValue();
				break;
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
