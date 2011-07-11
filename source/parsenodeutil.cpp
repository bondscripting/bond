#include "bond/parsenodeutil.h"
#include "bond/symboltable.h"

namespace Bond
{

bu32_t GetLength(const ListParseNode *head)
{
	bu32_t length = 0;
	const ListParseNode *current = head;
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
	TypeDescriptor result = INT_TYPE_DESCRIPTOR;

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
		result = FLOAT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_INT) || (b == Token::KEY_INT))
	{
		result = INT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_UINT) || (b == Token::KEY_UINT))
	{
		result = UINT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_CHAR) || (b == Token::KEY_CHAR))
	{
		result = CHAR_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_BOOL) || (b == Token::KEY_BOOL))
	{
		result = BOOL_TYPE_DESCRIPTOR;
	}

	return result;
}


Value CastValue(const TypeAndValue &value, const TypeDescriptor *type)
{
	// This function assumes that it is only called for valid casts on defined values.
	Token::TokenType sourceType = value.GetTypeDescriptor()->GetPrimitiveType();
	Value resultValue;

	switch (type->GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			switch (sourceType)
			{
				case Token::KEY_BOOL:
					resultValue.mBool = static_cast<bool>(value.GetBoolValue());
					break;
				default:
					break;
			}

		case Token::KEY_CHAR:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
					resultValue.mChar = static_cast<char>(value.GetCharValue());
					break;
				case Token::KEY_FLOAT:
					resultValue.mChar = static_cast<char>(value.GetFloatValue());
					break;
				case Token::KEY_INT:
					resultValue.mChar = static_cast<char>(value.GetIntValue());
					break;
				case Token::KEY_UINT:
					resultValue.mChar = static_cast<char>(value.GetUIntValue());
					break;
				default:
					break;
			}
			break;

		case Token::KEY_FLOAT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
					resultValue.mFloat = static_cast<bf32_t>(value.GetCharValue());
					break;
				case Token::KEY_FLOAT:
					resultValue.mFloat = static_cast<bf32_t>(value.GetFloatValue());
					break;
				case Token::KEY_INT:
					resultValue.mFloat = static_cast<bf32_t>(value.GetIntValue());
					break;
				case Token::KEY_UINT:
					resultValue.mFloat = static_cast<bf32_t>(value.GetUIntValue());
					break;
				default:
					break;
			}
			break;

		case Token::KEY_INT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
					resultValue.mInt = static_cast<bi32_t>(value.GetCharValue());
					break;
				case Token::KEY_FLOAT:
					resultValue.mInt = static_cast<bi32_t>(value.GetFloatValue());
					break;
				case Token::KEY_INT:
					resultValue.mInt = static_cast<bi32_t>(value.GetIntValue());
					break;
				case Token::KEY_UINT:
					resultValue.mInt = static_cast<bi32_t>(value.GetUIntValue());
					break;
				default:
					break;
			}
			break;

		case Token::KEY_UINT:
			switch (sourceType)
			{
				case Token::KEY_CHAR:
					resultValue.mUInt = static_cast<bu32_t>(value.GetCharValue());
					break;
				case Token::KEY_FLOAT:
					resultValue.mUInt = static_cast<bu32_t>(value.GetFloatValue());
					break;
				case Token::KEY_INT:
					resultValue.mUInt = static_cast<bu32_t>(value.GetIntValue());
					break;
				case Token::KEY_UINT:
					resultValue.mUInt = static_cast<bu32_t>(value.GetUIntValue());
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
