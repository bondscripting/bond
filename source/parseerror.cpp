#include "bond/parseerror.h"

namespace Bond
{

const char *ParseError::GetDescription() const
{
	return GetDescription(mType);
}


const char *ParseError::GetDescription(Type type)
{
	static const char *const ERROR_DESCRIPTIONS[] =
	{
#define BOND_PARSE_ERROR_ITEM(item, description) description,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR_ITEM
	};

	return ERROR_DESCRIPTIONS[type];
}

}
