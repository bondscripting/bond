#include "bond/tokenstream.h"

namespace Bond
{

void TokenStream::SetBuffer(const Token *buffer, int length)
{
	mBuffer = buffer;
	mLength = length;
	mIndex = 0;
}


const Token *TokenStream::Next()
{
	const Token *token = Peek();
	Advance();
	return token;
}


const Token *TokenStream::TestNext(Token::TokenType type)
{
	const Token *token = Peek();
	if (token->GetTokenType() == type)
	{
		Advance();
		return token;
	}
	return 0;
}


void TokenStream::SkipTo(Token::TokenType type)
{
	while (mIndex < (mLength - 1))
	{
		if (TestPeek(type))
		{
			break;
		}
	}
}


void TokenStream::SkipTo(const Token::TokenType *types, int numTypes)
{
	while (mIndex < (mLength - 1))
	{
		if (TestPeek(types, numTypes))
		{
			break;
		}
	}
}


bool TokenStream::TestPeek(const Token::TokenType *types, int numTypes) const
{
	for (int i = 0; i < numTypes; ++i)
	{
		if (TestPeek(types[i]))
		{
			return true;
		}
	}
	return false;
}

}
