#include "bond/parseerror.h"
#include "bond/textwriter.h"
#include "bond/token.h"

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


void ParseError::Print(TextWriter &writer) const
{
	const Bond::StreamPos &pos = mContext->GetStartPos();

	writer.Write("(%d, %d): %s ", pos.line, pos.column, GetDescription());

	if (mType == Bond::ParseError::UNEXPECTED_TOKEN)
	{
		writer.Write("'%s' before '%s'.", mExpected, mContext->GetTokenName());
	}
	else
	{
		writer.Write("near '%s'.", mContext->GetTokenName());
	}
}

}
