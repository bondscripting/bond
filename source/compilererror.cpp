#include "bond/compiler/compilererror.h"
#include "bond/compiler/token.h"
#include "bond/io/outputstream.h"
#include "bond/tools/prettyprinter.h"

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


void CompilerError::Print(OutputStream &stream) const
{
	const char *format = GetFormat();

	if (mContext != nullptr)
	{
		const char *fileName = mContext->GetFileName();
		const Bond::StreamPos &pos = mContext->GetStartPos();
		stream.Print("%s (%d, %d): ", fileName, pos.line, pos.column);
	}

	enum State
	{
		STATE_NORMAL,
		STATE_PERCENT,
	};
	State state = STATE_NORMAL;

	intptr_t arg = 0;

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
				stream.Print("%c", *format);
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
					mContext->GetText().PrintTo(stream);
					state = STATE_NORMAL;
				}
				break;

				case 'd':
				{
					stream.Print("%" BOND_PRId32, int32_t(arg));
					state = STATE_NORMAL;
				}
				break;

				case 'f':
				{
					const Token *token = reinterpret_cast<const Token *>(arg);
					const char *fileName = token->GetFileName();
					stream.Print("%s", fileName);
					state = STATE_NORMAL;
				}
				break;

				case 'l':
				{
					const Token *token = reinterpret_cast<const Token *>(arg);
					const Bond::StreamPos &argPos = token->GetStartPos();
					stream.Print("%" BOND_PRIu32, uint32_t(argPos.line));
					state = STATE_NORMAL;
				}
				break;

				case 'n':
				{
					const ParseNode *node = reinterpret_cast<const ParseNode *>(arg);
					PrettyPrinter printer;
					printer.Print(node, stream, PrettyPrinter::VERBOSITY_NORMAL, PrettyPrinter::CONSTANT_FOLDING_ON);
					state = STATE_NORMAL;
				}
				break;

				case 's':
				{
					const char *str = reinterpret_cast<const char *>(arg);
					stream.Print("%s", str);
					state = STATE_NORMAL;
				}
				break;

				case 't':
				{
					const Token *token = reinterpret_cast<const Token *>(arg);
					token->GetText().PrintTo(stream);
					state = STATE_NORMAL;
				}
				break;

				default:
				{
					stream.Print("%%%c", *format);
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
	for (size_t i = 0; i < MAX_ERRORS; ++i)
	{
		mErrors[i] = CompilerError();
	}
}


void CompilerErrorBuffer::PushError(CompilerError::Type type, const Token *context, const void *arg0, const void *arg1)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = CompilerError(type, context, reinterpret_cast<intptr_t>(arg0), reinterpret_cast<intptr_t>(arg1));
		++mNumErrors;
	}
}


void CompilerErrorBuffer::PushErrorInt(CompilerError::Type type, const Token *context, intptr_t arg0, intptr_t arg1)
{
	if (mNumErrors < MAX_ERRORS)
	{
		mErrors[mNumErrors] = CompilerError(type, context, arg0, arg1);
		++mNumErrors;
	}
}


void CompilerErrorBuffer::CopyFrom(const CompilerErrorBuffer &other)
{
	for (size_t i = 0; i < other.mNumErrors; ++i)
	{
		if (mNumErrors < MAX_ERRORS)
		{
			mErrors[mNumErrors] = other.mErrors[i];
			++mNumErrors;
		}
	}
}


void CompilerErrorBuffer::Print(OutputStream &stream) const
{
	for (size_t i = 0; i < mNumErrors; ++i)
	{
		const Bond::CompilerError &error = mErrors[i];
		error.Print(stream);
		stream.Print("\n");
	}
}

}
