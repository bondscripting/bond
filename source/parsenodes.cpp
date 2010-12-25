#include "bond/parsenodes.h"

namespace Bond
{

Token::TokenType TypeDescriptor::GetPrimitiveType() const
{
	if (mSpecifier != 0)
	{
		return mSpecifier->GetPrimitiveType();
	}
	return Token::INVALID;
}


bool TypeDescriptor::IsBooleanType() const
{
	if (mSpecifier != 0)
	{
		return mSpecifier->IsBooleanType();
	}
	return false;
}


bool TypeDescriptor::IsIntegerType() const
{
	if (mSpecifier != 0)
	{
		return mSpecifier->IsIntegerType();
	}
	return false;
}


bool TypeDescriptor::IsNumericType() const
{
	if (mSpecifier != 0)
	{
		return mSpecifier->IsNumericType();
	}
	return false;
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
		return TokenTypeSet::BOOLEAN_TYPE_SPECIFIERS.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != 0)
	{
		return TokenTypeSet::INTEGER_TYPE_SPECIFIERS.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != 0)
	{
		return TokenTypeSet::NUMERIC_TYPE_SPECIFIERS.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


static const Token BOOL_TOKEN("bool", Token::KEY_BOOL);
static const Token CHAR_TOKEN("char", Token::KEY_CHAR);
static const Token INT_TOKEN("int", Token::KEY_INT);
static const Token UINT_TOKEN("uint", Token::KEY_UINT);
static const Token FLOAT_TOKEN("float", Token::KEY_FLOAT);

static const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
static const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
static const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
static const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
static const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);

const TypeDescriptor CONST_BOOL_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&BOOL_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_CHAR_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&CHAR_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_INT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&INT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_UINT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&UINT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_FLOAT_TYPE_DESCRIPTOR(const_cast<TypeSpecifier *>(&FLOAT_TYPE_SPECIFIER), true);
const TypeDescriptor CONST_STRING_TYPE_DESCRIPTOR(const_cast<TypeDescriptor *>(&CONST_CHAR_TYPE_DESCRIPTOR), true);

}
