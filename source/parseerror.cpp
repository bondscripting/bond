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

	if (mType == ParseError::UNEXPECTED_TOKEN)
	{
		writer.Write("'%s' before '%s'.", mExpected, mContext->GetText());
	}
	else if (mType == ParseError::HASH_COLLISION)
	{
		const Bond::StreamPos &previousPos = mAltContext->GetStartPos();
		writer.Write("'%s' hash code collided with '%s' on line '%d'.", mContext->GetText(), mAltContext->GetText(), previousPos.line);
	}
	else if (mType == ParseError::DUPLICATE_SYMBOL)
	{
		const Bond::StreamPos &previousPos = mAltContext->GetStartPos();
		writer.Write("'%s' previously defined on line '%d'.", mContext->GetText(), previousPos.line);
	}
	else
	{
		writer.Write("near '%s'.", mContext->GetText());
	}
}


ParseErrorBuffer::ParseErrorBuffer()
{
	Reset();
}


void ParseErrorBuffer::Reset()
{
	mNumErrors = 0;
	for (int i = 0; i < MAX_ERRORS; ++i)
	{
		mErrors[i] = ParseError();
	}
}


void ParseErrorBuffer::PushError(ParseError::Type type, const Token *context, const char *expected)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = ParseError(type, context, expected);
		++mNumErrors;
	}
}


void ParseErrorBuffer::PushError(ParseError::Type type, const Token *context, const Token *altContext)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = ParseError(type, context, altContext);
		++mNumErrors;
	}
}

}
