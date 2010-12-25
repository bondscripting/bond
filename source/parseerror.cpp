#include "bond/parseerror.h"
#include "bond/bufferedtextwriter.h"
#include "bond/prettyprinter.h"
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

		case CONTEXT_LINE:
		{
			const Bond::StreamPos &altPos = GetTokenArg()->GetStartPos();
			writer.Write(format, mContext->GetText(), altPos.line);
		}
		break;

		case STR_CONTEXT:
		{
			writer.Write(format, GetStringArg(), mContext->GetText());
		}
		break;

		case CONTEXT_NODE:
		{
			const int BUFFER_SIZE = 1024;
			char nodeBuffer[BUFFER_SIZE];
			BufferedTextWriter nodeWriter(nodeBuffer, BUFFER_SIZE);
			PrettyPrinter nodePrinter(nodeWriter);
			nodePrinter.Print(GetParseNodeArg());
			writer.Write(format, mContext->GetText(), nodeBuffer);
		}
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


void ParseErrorBuffer::PushError(ParseError::Type type, const Token *context, const char *arg)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = ParseError(type, context, arg);
		++mNumErrors;
	}
}


void ParseErrorBuffer::PushError(ParseError::Type type, const Token *context, const Token *arg)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = ParseError(type, context, arg);
		++mNumErrors;
	}
}


void ParseErrorBuffer::PushError(ParseError::Type type, const Token *context, const ParseNode *arg)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = ParseError(type, context, arg);
		++mNumErrors;
	}
}

}
