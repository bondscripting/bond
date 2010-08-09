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
	if (HasNext())
	{
		++mIndex;
	}
	return token;
}

}
