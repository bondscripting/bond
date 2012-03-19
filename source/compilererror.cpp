#include "bond/compilererror.h"
#include "bond/textwriter.h"
#include "bond/prettyprinter.h"
#include "bond/token.h"

namespace Bond
{

const char *CompilerError::GetFormat() const
{
	return GetFormat(mType);
}


const char *CompilerError::GetFormat(Type type)
{
	static const char *const ERROR_FORMATS[] =
	{
#define BOND_COMPILER_ERROR(type, format) format,
		BOND_COMPILER_ERROR_LIST
#undef BOND_COMPILER_ERROR
	};

	return ERROR_FORMATS[type];
}


const char *CompilerError::GetErrorName() const
{
	return GetErrorName(mType);
}


const char *CompilerError::GetErrorName(Type type)
{
	static const char *const ERROR_NAMES[] =
	{
#define BOND_COMPILER_ERROR(type, format) #type,
		BOND_COMPILER_ERROR_LIST
#undef BOND_COMPILER_ERROR
	};

	return ERROR_NAMES[type];
}


void CompilerError::Print(TextWriter &writer) const
{
	const char *format = GetFormat();

	if (mContext != NULL)
	{
		const Bond::StreamPos &pos = mContext->GetStartPos();
		writer.Write("(%d, %d): ", pos.line, pos.column);
	}

	enum State
	{
		STATE_NORMAL,
		STATE_PERCENT,
	};
	State state = STATE_NORMAL;

	const void *arg = NULL;

	while (*format != '\0')
	{
		if (state == STATE_NORMAL)
		{
			if (*format == '%')
			{
				state = STATE_PERCENT;
				arg = mArg0;
			}
			else
			{
				writer.Write("%c", *format);
			}
		}
		else
		{
			switch (*format)
			{
				case '0':
				{
					arg = mArg0;
				}
				break;

				case '1':
				{
					arg = mArg1;
				}
				break;

				case 'c':
				{
					writer.Write("%s", mContext->GetText());
					state = STATE_NORMAL;
				}
				break;

				case 'l':
				{
					const Token *token = static_cast<const Token *>(arg);
					const Bond::StreamPos &argPos = token->GetStartPos();
					writer.Write("%d", argPos.line);
					state = STATE_NORMAL;
				}
				break;

				case 'n':
				{
					const ParseNode *node = static_cast<const ParseNode *>(arg);
					PrettyPrinter printer(writer, true);
					printer.Print(node);
					state = STATE_NORMAL;
				}
				break;

				case 't':
				{
					const Token *token = static_cast<const Token *>(arg);
					writer.Write("%s", token->GetText());
					state = STATE_NORMAL;
				}
				break;

				case 's':
				{
					const char *str = static_cast<const char *>(arg);
					state = STATE_NORMAL;
					writer.Write("%s", str);
					state = STATE_NORMAL;
				}
				break;

				default:
				{
					writer.Write("%%%c", *format);
					state = STATE_NORMAL;
				}
				break;
			}
		}

		++format;
	}
}


CompilerErrorBuffer::CompilerErrorBuffer()
{
	Reset();
}


void CompilerErrorBuffer::Reset()
{
	mNumErrors = 0;
	for (int i = 0; i < MAX_ERRORS; ++i)
	{
		mErrors[i] = CompilerError();
	}
}


void CompilerErrorBuffer::PushError(CompilerError::Type type, const Token *context, const void *arg0, const void *arg1)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = CompilerError(type, context, arg0, arg1);
		++mNumErrors;
	}
}


void CompilerErrorBuffer::CopyFrom(const CompilerErrorBuffer &other)
{
	for (int i = 0; i < other.mNumErrors; ++i)
	{
		if (mNumErrors < MAX_ERRORS)
		{
			mErrors[mNumErrors] = other.mErrors[i];
			++mNumErrors;
		}
	}
}

}
