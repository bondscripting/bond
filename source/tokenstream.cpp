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


const Token *TokenStream::NextIf(Token::TokenType type)
{
	const Token *token = PeekIf(type);
	if (token != NULL)
	{
		Advance();
		return token;
	}
	return NULL;
}


const Token *TokenStream::NextIf(const TokenTypeSet &typeSet)
{
	const Token *token = PeekIf(typeSet);
	if (token != NULL)
	{
		Advance();
		return token;
	}
	return NULL;
}


const Token *TokenStream::PeekIf(Token::TokenType type) const
{
	const Token *token = Peek();
	if (token->GetTokenType() == type)
	{
		return token;
	}
	return NULL;
}


const Token *TokenStream::PeekIf(const TokenTypeSet &typeSet) const
{
	const Token *token = Peek();
	if (typeSet.Contains(token->GetTokenType()))
	{
		return token;
	}
	return NULL;
}


void TokenStream::SkipTo(Token::TokenType type)
{
	while (mIndex < (mLength - 1))
	{
		if (PeekIf(type) != NULL)
		{
			break;
		}
		Advance();
	}
}


void TokenStream::SkipTo(const TokenTypeSet &typeSet)
{
	while (mIndex < (mLength - 1))
	{
		if (PeekIf(typeSet) != NULL)
		{
			break;
		}
		Advance();
	}
}

}
