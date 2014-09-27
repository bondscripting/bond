#include "bond/compiler/tokenstream.h"

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
	if (token != nullptr)
	{
		Advance();
		return token;
	}
	return nullptr;
}


const Token *TokenStream::NextIf(const TokenTypeSet &typeSet)
{
	const Token *token = PeekIf(typeSet);
	if (token != nullptr)
	{
		Advance();
		return token;
	}
	return nullptr;
}


const Token *TokenStream::PeekIf(Token::TokenType type) const
{
	const Token *token = Peek();
	if (token->GetTokenType() == type)
	{
		return token;
	}
	return nullptr;
}


const Token *TokenStream::PeekIf(const TokenTypeSet &typeSet) const
{
	const Token *token = Peek();
	if (typeSet.Contains(token->GetTokenType()))
	{
		return token;
	}
	return nullptr;
}


void TokenStream::SkipTo(Token::TokenType type)
{
	while (mIndex < (mLength - 1))
	{
		if (PeekIf(type) != nullptr)
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
		if (PeekIf(typeSet) != nullptr)
		{
			break;
		}
		Advance();
	}
}

}
