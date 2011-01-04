#include "bond/parsenodes.h"

namespace Bond
{

const Token *TypeDescriptor::GetContextToken() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->GetContextToken();
	}
	if (mParent != 0)
	{
		return mParent->GetContextToken();
	}
	return 0;
}


Token::TokenType TypeDescriptor::GetPrimitiveType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->GetPrimitiveType();
	}
	return Token::INVALID;
}


bool TypeDescriptor::IsBooleanType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsBooleanType();
	}
	return false;
}


bool TypeDescriptor::IsIntegerType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsIntegerType();
	}
	return false;
}


bool TypeDescriptor::IsNumericType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsNumericType();
	}
	return false;
}


const Token *TypeSpecifier::GetContextToken() const
{
	if (mPrimitiveType != 0)
	{
		return mPrimitiveType;
	}
	if (mIdentifier != 0)
	{
		return mIdentifier->GetContextToken();
	}
	return 0;
}


Token::TokenType TypeSpecifier::GetPrimitiveType() const
{
	if (mPrimitiveType != 0)
	{
		return mPrimitiveType->GetTokenType();
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsBooleanType() const
{
	if (mPrimitiveType != 0)
	{
		return BOOLEAN_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != 0)
	{
		return INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != 0)
	{
		return NUMERIC_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


const Token *FunctionDefinition::GetContextToken() const
{
	return (mPrototype != 0) ? mPrototype->GetContextToken() : 0;
}


const Token *IdentifierExpression::GetContextToken() const
{
	return (mIdentifier != 0) ? mIdentifier->GetContextToken() : 0;
}


const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);

const TypeDescriptor CONST_BOOL_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&BOOL_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_CHAR_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&CHAR_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_INT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&INT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_UINT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&UINT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_FLOAT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&FLOAT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_STRING_TYPE_DESCRIPTOR(const_cast<TypeDescriptor *>(&CONST_CHAR_TYPE_DESCRIPTOR), true);

}
