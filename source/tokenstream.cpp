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
	if (token != 0)
	{
		Advance();
		return token;
	}
	return 0;
}


const Token *TokenStream::NextIf(const TokenTypeSet &typeSet)
{
	const Token *token = PeekIf(typeSet);
	if (token != 0)
	{
		Advance();
		return token;
	}
	return 0;
}


const Token *TokenStream::PeekIf(Token::TokenType type) const
{
	const Token *token = Peek();
	if (token->GetTokenType() == type)
	{
		return token;
	}
	return 0;
}


const Token *TokenStream::PeekIf(const TokenTypeSet &typeSet) const
{
	for (int i = 0; i < typeSet.numTypes; ++i)
	{
		const Token *token = Peek();
		if (token->GetTokenType() == typeSet.types[i])
		{
			return token;
		}
	}
	return 0;
}


void TokenStream::SkipTo(Token::TokenType type)
{
	while (mIndex < (mLength - 1))
	{
		if (PeekIf(type) != 0)
		{
			break;
		}
	}
}


void TokenStream::SkipTo(const TokenTypeSet &typeSet)
{
	while (mIndex < (mLength - 1))
	{
		if (PeekIf(typeSet) != 0)
		{
			break;
		}
	}
}

}
