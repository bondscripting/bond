#include "bond/token.h"

namespace Bond
{

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

}
