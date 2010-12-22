#include "bond/parseerror.h"
#include "bond/textwriter.h"
#include "bond/token.h"

namespace Bond
{

ParseError::Category ParseError::GetCategory() const
{
	return GetCategory(mType);
}


ParseError::Category ParseError::GetCategory(Type type)
{
	static const Category ERROR_CATEGORIES[] =
	{
#define BOND_PARSE_ERROR(category, type, format) category,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR
	};

	return ERROR_CATEGORIES[type];
}


const char *ParseError::GetFormat() const
{
	return GetFormat(mType);
}


const char *ParseError::GetFormat(Type type)
{
	static const char *const ERROR_FORMATS[] =
	{
#define BOND_PARSE_ERROR(category, type, format) format,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR
	};

	return ERROR_FORMATS[type];
}


void ParseError::Print(TextWriter &writer) const
{
	const Bond::StreamPos &pos = mContext->GetStartPos();
	writer.Write("(%d, %d): ", pos.line, pos.column);

	const char *format = GetFormat();
	switch (GetCategory())
	{
		case BASIC:
		{
			writer.Write(format);
		}
		break;

		case CONTEXT:
		{
			writer.Write(format, mContext->GetText());
		}
		break;

		case CONTEXT_ALT_LINE:
		{
			const Bond::StreamPos &altPos = mAltContext->GetStartPos();
			writer.Write(format, mContext->GetText(), altPos.line);
		}
		break;

		case EXPECTED_CONTEXT:
		{
			writer.Write(format, mExpected, mContext->GetText());
		}
		break;
	}

	/*
	switch (mType)
	{
		case ParseError::UNEXPECTED_TOKEN:
		{
			writer.Write("'%s' before '%s'.", mExpected, mContext->GetText());
		}
		break;

		case ParseError::DUPLICATE_SYMBOL:
		case ParseError::DUPLICATE_FUNCTION_DEFINITION:
		case ParseError::FUNCTION_PROTOTYPE_MISMATCH:
		{
			const Bond::StreamPos &previousPos = mAltContext->GetStartPos();
			writer.Write("'%s' previously defined on line '%d'.", mContext->GetText(), previousPos.line);
		}

		default:
		{
			writer.Write("near '%s'.", mContext->GetText());
		}
	}
	*/
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
