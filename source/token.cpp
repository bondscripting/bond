#include "bond/token.h"

namespace Bond
{

Token &Token::operator=(const Token &other)
{
	mStartPos = other.mStartPos;
	mEndPos = other.mEndPos;
	mErrorPos = other.mErrorPos;
	mValue = other.mValue;
	mText = other.mText;
	mTokenType = other.mTokenType;
	mErrorType = other.mErrorType;
	return *this;
}


void Token::SetStringValue(const char *buffer, int length)
{
	mValue.mString.buffer = buffer;
	mValue.mString.length = length;
}


const char *Token::GetTokenName() const
{
	return GetTokenName(mTokenType);
}


const char *Token::GetTokenName(TokenType type)
{
	static const char *const TOKEN_NAMES[] =
	{
#define BOND_TOKEN_ITEM(item) #item,
		BOND_TOKEN_LIST
#undef BOND_TOKEN_ITEM
	};

	return TOKEN_NAMES[type];
}


const char *Token::GetErrorName() const
{
	return GetErrorName(mErrorType);
}


const char *Token::GetErrorName(ErrorType type)
{
	static const char *const ERROR_NAMES[] =
	{
#define BOND_TOKEN_ERROR_ITEM(item) #item,
		BOND_TOKEN_ERROR_LIST
#undef BOND_TOKEN_ERROR_ITEM
	};

	return ERROR_NAMES[type];
}

}
