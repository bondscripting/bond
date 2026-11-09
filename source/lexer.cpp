#include "bond/compiler/charstream.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"
#include "bond/io/memoryinputstream.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/allocator.h"
#include "private/memory.h"
#include "private/utf8.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Bond
{

class LexerCore
{
public:
	LexerCore(
			Allocator &allocator,
			CompilerErrorBuffer &errorBuffer,
			const char *fileName,
			InputStream &stream):
		mAllocator(allocator),
		mErrorBuffer(errorBuffer),
		mFileName(fileName),
		mStream(stream),
		mTokens(nullptr),
		mStringBuffer(nullptr)
	{}

	LexerCore(const LexerCore &other) = delete;
	LexerCore &operator=(const LexerCore &other) = delete;

	TokenCollectionHandle Lex();

private:
	//integral-digits[.[fractional-digits]][e[sign]exponential-digits]
	enum LexState
	{
		STATE_SPACE,               // Whitespace.
		STATE_PLUS,                // '+'
		STATE_MINUS,               // '-'
		STATE_STAR,                // '*'
		STATE_SLASH,               // '/'
		STATE_PERCENT,             // '%'
		STATE_LT,                  // '<'
		STATE_GT,                  // '>'
		STATE_LEFT,                // '<<'
		STATE_RIGHT,               // '>>'
		STATE_EQUAL,               // '='
		STATE_NOT,                 // '!'
		STATE_AND,                 // '&'
		STATE_OR,                  // '|'
		STATE_XOR,                 // '^'
		STATE_COLON,               // ':'
		STATE_ZERO,                // '0'
		STATE_OCTAL,               // Octal integer.
		STATE_HEX,                 // Hex integer.
		STATE_IDIGITS,             // Integral digits.
		STATE_FDIGITS,             // Fractional digits.
		STATE_EDIGITS,             // Exponential digits.
		STATE_PERIOD,              // '.'
		STATE_EXPONENT,            // 'e' or 'E' known to be inside a number.
		STATE_EXPONENT_SIGN,       // '+' or '-' known to be inside a number's exponent.
		STATE_FLOAT,               // Complete float literal.
		STATE_UNSIGNED,            // Complete unsigned literal.
		STATE_LONG,                // Complete long literal.
		STATE_ULONG,               // Complete unsigned long literal.
		STATE_BAD_NUMBER,          // Consume the remaining characters of an incorrectly formatted number.
		STATE_IDENTIFIER,          // Sequence of characters forming an identifier.
		STATE_CHAR,                // Character literal.
		STATE_CHAR_ESCAPE,         // Escape sequence within a character literal.
		STATE_CHAR_OCTAL_ESCAPE,   // Octal escape within a character literal.
		STATE_CHAR_HEX_ESCAPE_X,   // \x at the start of a hex escape within a character literal.
		STATE_CHAR_HEX_ESCAPE,     // Hex escape within a character literal.
		STATE_CHAR_UNICODE_ESCAPE, // \u or \U escape within a character literal.
		STATE_CHAR_UTF8,           // Multi-byte UTF-8 character within a character literal.
		STATE_CHAR_END,            // Expect the end quote of a character literal..
		STATE_STRING,              // String literal.
		STATE_STRING_ESCAPE,       // Escape sequence within a string literal.
		STATE_STRING_OCTAL_ESCAPE, // Octal escape within a string literal.
		STATE_STRING_HEX_ESCAPE_X, // \x at the start of a hex escape within string literal.
		STATE_STRING_HEX_ESCAPE,   // Hex escape within a string literal.
		STATE_STRING_UNICODE_ESCAPE, // \u or \U escape within a string literal.
		STATE_STRING_UTF8,         // Multi-byte UTF-8 character within a string literal.
		STATE_C_COMMENT,           // C style comment.
		STATE_C_COMMENT_STAR,      // Star within a C style comment.
		STATE_C_COMMENT_UTF8,      // Multi-byte UTF-8 character within a C style comment.
		STATE_LINE_COMMENT,        // Single line C++ style comment.
		STATE_LINE_COMMENT_UTF8,   // Multi-byte UTF-8 character within a single line comment.
		STATE_INVALID_CHAR,        // Consume the continuation bytes of an invalid multi-byte character.
		STATE_DONE                 // Done parsing the current token.
	};

	struct Resources
	{
		Resources() : numTokens(0), stringBufferLength(0) {}

		size_t numTokens;
		size_t stringBufferLength;
	};

	// The bytes that a character or escape sequence evaluates to. A \u or \U escape evaluates to
	// the UTF-8 encoding of its code point; anything else evaluates to a single byte.
	struct CharResult
	{
		CharResult(): error(CompilerError::NO_ERROR), end(nullptr), codePoint(0), length(1), bytes{0, 0, 0, 0} {}

		CompilerError::Type error;
		const char *end;
		uint32_t codePoint;
		size_t length;
		char bytes[4];
	};

	const char *SkipByteOrderMark();
	Resources CalculateResources(CharStream &stream) const;

	void GenerateTokens(CharStream &stream);
	void GenerateToken(CharStream &stream, Token &token, LexState &resumeState);
	CompilerError::Type ScanToken(CharStream &stream, Token &token, LexState &resumeState) const;
	void ExtractToken(Token &token);

	void EvaluateToken(Token &token);
	void EvaluateKeywordOrIdentifierToken(Token &token) const;
	void EvaluateCharToken(Token &token);
	void EvaluateIntegerToken(Token &token);
	void EvaluateLongToken(Token &token);
	void EvaluateFloatToken(Token &token) const;
	void EvaluateDoubleToken(Token &token) const;
	void EvaluateStringToken(Token &token);
	CharResult EvaluateChar(const char *text) const;

	// These classify ASCII only, independent of the locale. Unlike the <cctype> functions, they are
	// safe to call with negative chars, i.e. bytes of multi-byte UTF-8 characters.
	bool IsDigitChar(char c) const;
	bool IsAlphaChar(char c) const;
	bool IsSpaceChar(char c) const;
	bool IsIdentifierChar(char c) const;
	bool IsOctalChar(char c) const;
	bool IsHexChar(char c) const;
	bool IsBadNumberChar(char c) const;
	bool IsExponentChar(char c) const;
	bool IsFloatSuffixChar(char c) const;
	bool IsLongSuffixChar(char c) const;
	bool IsUnsignedSuffixChar(char c) const;
	bool IsEscapeChar(char c) const;
	uint32_t GetHexValue(char c) const;

	void PushError(CompilerError::Type type, Token &token);
	void PushInvalidCharacterError(Token &token);
	void PushNonAsciiCharacterError(Token &token, uint32_t codePoint, size_t length);

	char *AllocString(size_t length);
	char *AllocString(const char *content, size_t length);

	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	const char *mFileName;
	InputStream &mStream;
	Token *mTokens;
	char *mStringBuffer;
};


TokenCollectionHandle Lexer::Lex(const char *fileName, const char *text, size_t length)
{
	MemoryInputStream stream(text, Stream::pos_t(length));
	LexerCore lexer(mAllocator, mErrorBuffer, fileName, stream);
	return lexer.Lex();
}


TokenCollectionHandle Lexer::Lex(const char *fileName, InputStream &stream)
{
	LexerCore lexer(mAllocator, mErrorBuffer, fileName, stream);
	return lexer.Lex();
}


TokenCollectionHandle LexerCore::Lex()
{
	// Must run before the CharStream is constructed, since it captures the stream's start position.
	const char *unsupportedEncoding = SkipByteOrderMark();

	CharStream stream(mStream);
	Resources resources = CalculateResources(stream);

	const size_t fileNameLength = strlen(mFileName);
	resources.stringBufferLength += fileNameLength + 1;

	size_t memSize = 0;
	const size_t tokenCollectionStart = TallyMemoryRequirements<TokenCollection>(memSize, 1);
	const size_t tokensStart = TallyMemoryRequirements<Token>(memSize, resources.numTokens);
	const size_t stringBufferStart = TallyMemoryRequirements<Token>(memSize, resources.stringBufferLength);

	auto memHandle = mAllocator.AllocOwned<char>(memSize);
	mTokens = reinterpret_cast<Token *>(memHandle.get() + tokensStart);
	mStringBuffer = memHandle.get() + stringBufferStart;
	mFileName = AllocString(mFileName, fileNameLength);

	stream.Reset();
	GenerateTokens(stream);

	if (unsupportedEncoding != nullptr)
	{
		// The stream was skipped to its end, so the only token is END. Report the error against it
		// for its file name, but leave its type intact so the parser still sees the end of input.
		mErrorBuffer.PushError(CompilerError::UNSUPPORTED_ENCODING, mTokens, unsupportedEncoding);
	}

	TokenCollection *tokenCollection = new (memHandle.get() + tokenCollectionStart) TokenCollection(mTokens, static_cast<int>(resources.numTokens));
	memHandle.release();

	return TokenCollectionHandle(mAllocator, tokenCollection);
}


const char *LexerCore::SkipByteOrderMark()
{
	const Stream::pos_t startPos = mStream.GetPosition();
	uint8_t bytes[4] = {0x01, 0x01, 0x01, 0x01};
	const size_t numBytes = mStream.Read(bytes, sizeof(bytes));

	const char *unsupportedEncoding = nullptr;
	size_t skipLength = 0;

	// UTF-32LE must be tested before UTF-16LE since its byte order mark starts with UTF-16LE's.
	if ((numBytes >= 3) && (bytes[0] == 0xEF) && (bytes[1] == 0xBB) && (bytes[2] == 0xBF))
	{
		skipLength = 3;
	}
	else if ((numBytes >= 4) && (bytes[0] == 0xFF) && (bytes[1] == 0xFE) && (bytes[2] == 0x00) && (bytes[3] == 0x00))
	{
		unsupportedEncoding = "UTF-32LE";
	}
	else if ((numBytes >= 4) && (bytes[0] == 0x00) && (bytes[1] == 0x00) && (bytes[2] == 0xFE) && (bytes[3] == 0xFF))
	{
		unsupportedEncoding = "UTF-32BE";
	}
	else if ((numBytes >= 2) && (bytes[0] == 0xFF) && (bytes[1] == 0xFE))
	{
		unsupportedEncoding = "UTF-16LE";
	}
	else if ((numBytes >= 2) && (bytes[0] == 0xFE) && (bytes[1] == 0xFF))
	{
		unsupportedEncoding = "UTF-16BE";
	}

	if (unsupportedEncoding != nullptr)
	{
		// Lexing the content would produce a flood of meaningless errors, so skip all of it.
		mStream.SetPositionFromEnd(0);
	}
	else
	{
		mStream.SetPosition(startPos + Stream::pos_t(skipLength));
	}

	return unsupportedEncoding;
}


LexerCore::Resources LexerCore::CalculateResources(CharStream &stream) const
{
	Resources resources;

	Token token;
	LexState resumeState = STATE_SPACE;
	for (;;)
	{
		ScanToken(stream, token, resumeState);

		size_t length = token.GetEndIndex() - token.GetStartPos().index + 1;
		if (token.GetTokenType() == Token::CONST_STRING)
		{
			// Account for the string value. Subtract 2 because the quotes are stripped.
			// Could actually be shorter once escape sequences are expanded.
			length += length - 2;
		}

		resources.stringBufferLength += length;
		++resources.numTokens;

		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}

	return resources;
}


void LexerCore::GenerateTokens(CharStream &stream)
{
	size_t tokenIndex = 0;
	bool done = false;
	LexState resumeState = STATE_SPACE;

	while (!done)
	{
		Token &token = mTokens[tokenIndex++];
		GenerateToken(stream, token, resumeState);
		done = token.GetTokenType() == Bond::Token::END;
	}
}


void LexerCore::GenerateToken(CharStream &stream, Token &token, LexState &resumeState)
{
	const CompilerError::Type error = ScanToken(stream, token, resumeState);
	token.SetFileName(mFileName);
	ExtractToken(token);

	// The token's text must be extracted before an invalid character can be decoded.
	if (error == CompilerError::INVALID_CHARACTER)
	{
		PushInvalidCharacterError(token);
	}
	else if (error == CompilerError::NON_ASCII_CHARACTER_CONSTANT)
	{
		// Decode the character following the opening quote, which the scanner has validated.
		const StringView text = token.GetText();
		const Private::Utf8Decoded decoded = Private::DecodeUtf8(text.data() + 1, text.size() - 1);
		PushNonAsciiCharacterError(token, decoded.codePoint, decoded.length);
	}
	else
	{
		PushError(error, token);
	}

	EvaluateToken(token);
}


CompilerError::Type LexerCore::ScanToken(CharStream &stream, Token &token, LexState &resumeState) const
{
	CompilerError::Type error = CompilerError::NO_ERROR;
	token = Token();

	// Scanning resumes within a comment after an invalid UTF-8 sequence inside it was returned as a
	// token of its own.
	LexState state = resumeState;
	resumeState = STATE_SPACE;

	Private::Utf8Validator utf8Validator;
	StreamPos utf8SequenceStart;
	int unicodeDigitsRemaining = 0;

	stream.Sync();
	token.SetStartPos(stream.GetStreamPos());
	while (stream.HasNext() && (state != STATE_DONE))
	{
		const StreamPos pos = stream.GetStreamPos();
		const char c = stream.Next();

		switch (state)
		{
			case STATE_SPACE:
				token.SetStartPos(pos);

				if (c == '+')
				{
					state = STATE_PLUS;
				}
				else if (c == '-')
				{
					state = STATE_MINUS;
				}
				else if (c == '*')
				{
					state = STATE_STAR;
				}
				else if (c == '/')
				{
					state = STATE_SLASH;
				}
				else if (c == '%')
				{
					state = STATE_PERCENT;
				}
				else if (c == '<')
				{
					state = STATE_LT;
				}
				else if (c == '>')
				{
					state = STATE_GT;
				}
				else if (c == '=')
				{
					state = STATE_EQUAL;
				}
				else if (c == '!')
				{
					state = STATE_NOT;
				}
				else if (c == '&')
				{
					state = STATE_AND;
				}
				else if (c == '|')
				{
					state = STATE_OR;
				}
				else if (c == '^')
				{
					state = STATE_XOR;
				}
				else if (c == '~')
				{
					token.SetTokenType(Token::OP_BIT_NOT);
					state = STATE_DONE;
				}
				else if (c == '?')
				{
					token.SetTokenType(Token::OP_TERNARY);
					state = STATE_DONE;
				}
				else if (c == '.')
				{
					state = STATE_PERIOD;
				}
				else if (c == '(')
				{
					token.SetTokenType(Token::OPAREN);
					state = STATE_DONE;
				}
				else if (c == ')')
				{
					token.SetTokenType(Token::CPAREN);
					state = STATE_DONE;
				}
				else if (c == '{')
				{
					token.SetTokenType(Token::OBRACE);
					state = STATE_DONE;
				}
				else if (c == '}')
				{
					token.SetTokenType(Token::CBRACE);
					state = STATE_DONE;
				}
				else if (c == '[')
				{
					token.SetTokenType(Token::OBRACKET);
					state = STATE_DONE;
				}
				else if (c == ']')
				{
					token.SetTokenType(Token::CBRACKET);
					state = STATE_DONE;
				}
				else if (c == ';')
				{
					token.SetTokenType(Token::SEMICOLON);
					state = STATE_DONE;
				}
				else if (c == ':')
				{
					state = STATE_COLON;
				}
				else if (c == ',')
				{
					token.SetTokenType(Token::COMMA);
					state = STATE_DONE;
				}
				else if (c == '\'')
				{
					state = STATE_CHAR;
				}
				else if (c == '\"')
				{
					state = STATE_STRING;
				}
				else if (c == '0')
				{
					state = STATE_ZERO;
				}
				else if (IsDigitChar(c))
				{
					state = STATE_IDIGITS;
				}
				else if (IsAlphaChar(c) || (c == '_'))
				{
					state = STATE_IDENTIFIER;
				}
				else if (uint8_t(c) >= 0x80)
				{
					// A non-ASCII character. Consume all of its bytes so it is reported only once.
					error = CompilerError::INVALID_CHARACTER;
					state = STATE_INVALID_CHAR;
				}
				else if (!IsSpaceChar(c))
				{
					error = CompilerError::INVALID_CHARACTER;
					state = STATE_DONE;
				}
				break;

			case STATE_PLUS:
				if (c == '+')
				{
					token.SetTokenType(Token::OP_INC);
					state = STATE_DONE;
				}
				else if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_PLUS);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_PLUS);
					state = STATE_DONE;
				}
				break;

			case STATE_MINUS:
				if (c == '-')
				{
					token.SetTokenType(Token::OP_DEC);
					state = STATE_DONE;
				}
				else if (c == '>')
				{
					token.SetTokenType(Token::OP_ARROW);
					state = STATE_DONE;
				}
				else if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_MINUS);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_MINUS);
					state = STATE_DONE;
				}
				break;

			case STATE_STAR:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_MULT);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_STAR);
					state = STATE_DONE;
				}
				break;

			case STATE_SLASH:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_DIV);
					state = STATE_DONE;
				}
				else if (c == '*')
				{
					state = STATE_C_COMMENT;
				}
				else if (c == '/')
				{
					state = STATE_LINE_COMMENT;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_DIV);
					state = STATE_DONE;
				}
				break;

			case STATE_PERCENT:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_MOD);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_MOD);
					state = STATE_DONE;
				}
				break;

			case STATE_LT:
				if (c == '=')
				{
					token.SetTokenType(Token::OP_LTE);
					state = STATE_DONE;
				}
				else if (c == '<')
				{
					state = STATE_LEFT;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_LT);
					state = STATE_DONE;
				}
				break;

			case STATE_GT:
				if (c == '=')
				{
					token.SetTokenType(Token::OP_GTE);
					state = STATE_DONE;
				}
				else if (c == '>')
				{
					state = STATE_RIGHT;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_GT);
					state = STATE_DONE;
				}
				break;

			case STATE_LEFT:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_LEFT);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_LEFT);
					state = STATE_DONE;
				}
				break;

			case STATE_RIGHT:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_RIGHT);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_RIGHT);
					state = STATE_DONE;
				}
				break;

			case STATE_EQUAL:
				if (c == '=')
				{
					token.SetTokenType(Token::OP_EQUAL);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::ASSIGN);
					state = STATE_DONE;
				}
				break;

			case STATE_NOT:
				if (c == '=')
				{
					token.SetTokenType(Token::OP_NOT_EQUAL);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_NOT);
					state = STATE_DONE;
				}
				break;

			case STATE_AND:
				if (c == '&')
				{
					token.SetTokenType(Token::OP_AND);
					state = STATE_DONE;
				}
				else if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_AND);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_AMP);
					state = STATE_DONE;
				}
				break;

			case STATE_OR:
				if (c == '|')
				{
					token.SetTokenType(Token::OP_OR);
					state = STATE_DONE;
				}
				else if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_OR);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_BIT_OR);
					state = STATE_DONE;
				}
				break;

			case STATE_XOR:
				if (c == '=')
				{
					token.SetTokenType(Token::ASSIGN_XOR);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::OP_BIT_XOR);
					state = STATE_DONE;
				}
				break;

			case STATE_COLON:
				if (c == ':')
				{
					token.SetTokenType(Token::SCOPE);
					state = STATE_DONE;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::COLON);
					state = STATE_DONE;
				}
				break;

			case STATE_ZERO:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if (IsExponentChar(c))
				{
					state = STATE_EXPONENT;
				}
				else if (IsFloatSuffixChar(c))
				{
					state = STATE_FLOAT;
				}
				else if (IsLongSuffixChar(c))
				{
					state = STATE_LONG;
				}
				else if (IsUnsignedSuffixChar(c))
				{
					state = STATE_UNSIGNED;
				}
				else if (c == 'x')
				{
					token.AddAnnotation(Token::HEX);
					state = STATE_HEX;
				}
				else if (IsOctalChar(c))
				{
					token.AddAnnotation(Token::OCTAL);
					state = STATE_OCTAL;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_OCTAL:
				if (IsLongSuffixChar(c))
				{
					state = STATE_LONG;
				}
				else if (IsUnsignedSuffixChar(c))
				{
					state = STATE_UNSIGNED;
				}
				else if (IsOctalChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_OCTAL_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_HEX:
				if (IsLongSuffixChar(c))
				{
					state = STATE_LONG;
				}
				else if (IsUnsignedSuffixChar(c))
				{
					state = STATE_UNSIGNED;
				}
				else if (IsHexChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_HEX_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_IDIGITS:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if (IsExponentChar(c))
				{
					state = STATE_EXPONENT;
				}
				else if (IsFloatSuffixChar(c))
				{
					state = STATE_FLOAT;
				}
				else if (IsLongSuffixChar(c))
				{
					state = STATE_LONG;
				}
				else if (IsUnsignedSuffixChar(c))
				{
					state = STATE_UNSIGNED;
				}
				else if (IsDigitChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_FDIGITS:
				if (IsExponentChar(c))
				{
					state = STATE_EXPONENT;
				}
				else if (IsFloatSuffixChar(c))
				{
					state = STATE_FLOAT;
				}
				else if (IsDigitChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_FLOAT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_DOUBLE);
					state = STATE_DONE;
				}
				break;

			case STATE_EDIGITS:
				if (IsDigitChar(c))
				{
					// Remain in this state.
				}
				else if (IsFloatSuffixChar(c))
				{
					state = STATE_FLOAT;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_FLOAT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_DOUBLE);
					state = STATE_DONE;
				}
				break;

			case STATE_PERIOD:
				if (IsDigitChar(c))
				{
					state = STATE_FDIGITS;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::PERIOD);
					state = STATE_DONE;
				}
				break;

			case STATE_EXPONENT:
				if (IsDigitChar(c))
				{
					state = STATE_EDIGITS;
				}
				else if ((c == '+') || (c == '-'))
				{
					state = STATE_EXPONENT_SIGN;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_FLOAT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					error = CompilerError::INVALID_FLOAT;
					state = STATE_DONE;
				}
				break;

			case STATE_EXPONENT_SIGN:
				if (IsDigitChar(c))
				{
					state = STATE_EDIGITS;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_FLOAT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					error = CompilerError::INVALID_FLOAT;
					state = STATE_DONE;
				}
				break;

			case STATE_FLOAT:
				if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_FLOAT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_FLOAT);
					state = STATE_DONE;
				}
				break;

			case STATE_UNSIGNED:
				if (IsLongSuffixChar(c))
				{
					state = STATE_ULONG;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_UINT);
					state = STATE_DONE;
				}
				break;

			case STATE_LONG:
				if (IsUnsignedSuffixChar(c))
				{
					state = STATE_ULONG;
				}
				else if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_LONG);
					state = STATE_DONE;
				}
				break;

			case STATE_ULONG:
				if (IsBadNumberChar(c))
				{
					error = CompilerError::INVALID_INT;
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Reset(pos);
					token.SetTokenType(Token::CONST_ULONG);
					state = STATE_DONE;
				}
				break;

			case STATE_BAD_NUMBER:
				if (!IsBadNumberChar(c))
				{
					stream.Reset(pos);
					state = STATE_DONE;
				}
				break;

			case STATE_IDENTIFIER:
				if (!IsIdentifierChar(c))
				{
					stream.Reset(pos);
					token.SetTokenType(Token::IDENTIFIER);
					state = STATE_DONE;
				}
				break;

			case STATE_CHAR:
				if (c == '\\')
				{
					state = STATE_CHAR_ESCAPE;
				}
				else if (c == '\'')
				{
					error = CompilerError::EMPTY_CHARACTER_CONSTANT;
					state = STATE_DONE;
				}
				else if (uint8_t(c) >= 0x80)
				{
					if (utf8Validator.Start(c) == Private::Utf8Validator::INVALID)
					{
						error = CompilerError::INVALID_UTF8;
						state = STATE_CHAR_END;
					}
					else
					{
						state = STATE_CHAR_UTF8;
					}
				}
				else
				{
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_UTF8:
			{
				const Private::Utf8Validator::Result result = utf8Validator.Feed(c);
				if (result == Private::Utf8Validator::COMPLETE)
				{
					// A char holds a single byte, so it cannot hold a multi-byte character.
					error = CompilerError::NON_ASCII_CHARACTER_CONSTANT;
					state = STATE_CHAR_END;
				}
				else if (result == Private::Utf8Validator::INVALID)
				{
					stream.Reset(pos);
					error = CompilerError::INVALID_UTF8;
					state = STATE_CHAR_END;
				}
			}
			break;

			case STATE_CHAR_ESCAPE:
				if (IsEscapeChar(c))
				{
					state = STATE_CHAR_END;
				}
				else if (IsOctalChar(c))
				{
					state = STATE_CHAR_OCTAL_ESCAPE;
				}
				else if (c == 'x')
				{
					state = STATE_CHAR_HEX_ESCAPE_X;
				}
				else if ((c == 'u') || (c == 'U'))
				{
					unicodeDigitsRemaining = (c == 'u') ? 4 : 8;
					state = STATE_CHAR_UNICODE_ESCAPE;
				}
				else
				{
					error = CompilerError::INVALID_ESCAPE;
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_OCTAL_ESCAPE:
				if (!IsOctalChar(c))
				{
					stream.Reset(pos);
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_HEX_ESCAPE_X:
				if (IsHexChar(c))
				{
					state = STATE_CHAR_HEX_ESCAPE;
				}
				else
				{
					stream.Reset(pos);
					error = CompilerError::MISSING_HEX_ESCAPE;
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_HEX_ESCAPE:
				if (!IsHexChar(c))
				{
					stream.Reset(pos);
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_UNICODE_ESCAPE:
				if (!IsHexChar(c))
				{
					stream.Reset(pos);
					error = CompilerError::INVALID_UNICODE_ESCAPE;
					state = STATE_CHAR_END;
				}
				else if (--unicodeDigitsRemaining == 0)
				{
					state = STATE_CHAR_END;
				}
				break;

			case STATE_CHAR_END:
				if (c == '\'')
				{
					token.SetTokenType(Token::CONST_CHAR);
					state = STATE_DONE;
				}
				else if (c == '\\')
				{
					error = CompilerError::MULTI_CHARACTER_CONSTANT;
					state = STATE_CHAR_ESCAPE;
				}
				else
				{
					error = CompilerError::MULTI_CHARACTER_CONSTANT;
				}
				break;

			case STATE_STRING:
				if (c == '\\')
				{
					state = STATE_STRING_ESCAPE;
				}
				else if (c == '\"')
				{
					token.SetTokenType(Token::CONST_STRING);
					state = STATE_DONE;
				}
				else if (uint8_t(c) >= 0x80)
				{
					if (utf8Validator.Start(c) == Private::Utf8Validator::INVALID)
					{
						error = CompilerError::INVALID_UTF8;
					}
					else
					{
						state = STATE_STRING_UTF8;
					}
				}
				break;

			case STATE_STRING_UTF8:
			{
				const Private::Utf8Validator::Result result = utf8Validator.Feed(c);
				if (result == Private::Utf8Validator::COMPLETE)
				{
					state = STATE_STRING;
				}
				else if (result == Private::Utf8Validator::INVALID)
				{
					// The byte that ended the sequence is not part of it, so scan it again.
					stream.Reset(pos);
					error = CompilerError::INVALID_UTF8;
					state = STATE_STRING;
				}
			}
			break;

			case STATE_STRING_ESCAPE:
				if (IsEscapeChar(c))
				{
					state = STATE_STRING;
				}
				else if (IsOctalChar(c))
				{
					state = STATE_STRING_OCTAL_ESCAPE;
				}
				else if (c == 'x')
				{
					state = STATE_STRING_HEX_ESCAPE_X;
				}
				else if ((c == 'u') || (c == 'U'))
				{
					unicodeDigitsRemaining = (c == 'u') ? 4 : 8;
					state = STATE_STRING_UNICODE_ESCAPE;
				}
				else
				{
					error = CompilerError::INVALID_ESCAPE;
					state = STATE_STRING;
				}
				break;

			case STATE_STRING_OCTAL_ESCAPE:
				if (!IsOctalChar(c))
				{
					stream.Reset(pos);
					state = STATE_STRING;
				}
				break;

			case STATE_STRING_HEX_ESCAPE_X:
				if (IsHexChar(c))
				{
					state = STATE_STRING_HEX_ESCAPE;
				}
				else
				{
					stream.Reset(pos);
					error = CompilerError::MISSING_HEX_ESCAPE;
					state = STATE_STRING;
				}
				break;

			case STATE_STRING_HEX_ESCAPE:
				if (!IsHexChar(c))
				{
					stream.Reset(pos);
					state = STATE_STRING;
				}
				break;

			case STATE_STRING_UNICODE_ESCAPE:
				if (!IsHexChar(c))
				{
					stream.Reset(pos);
					error = CompilerError::INVALID_UNICODE_ESCAPE;
					state = STATE_STRING;
				}
				else if (--unicodeDigitsRemaining == 0)
				{
					state = STATE_STRING;
				}
				break;

			case STATE_C_COMMENT:
			case STATE_LINE_COMMENT:
				if ((state == STATE_C_COMMENT) && (c == '*'))
				{
					state = STATE_C_COMMENT_STAR;
				}
				else if ((state == STATE_LINE_COMMENT) && (c == '\n'))
				{
					state = STATE_SPACE;
				}
				else if (uint8_t(c) >= 0x80)
				{
					utf8SequenceStart = pos;
					if (utf8Validator.Start(c) == Private::Utf8Validator::INVALID)
					{
						// Comments aren't tokens, so return the invalid byte as a token of its own to
						// report it, and resume the comment when scanning the next token.
						token.SetStartPos(pos);
						error = CompilerError::INVALID_UTF8;
						resumeState = state;
						state = STATE_DONE;
					}
					else
					{
						state = (state == STATE_C_COMMENT) ? STATE_C_COMMENT_UTF8 : STATE_LINE_COMMENT_UTF8;
					}
				}
				break;

			case STATE_C_COMMENT_UTF8:
			case STATE_LINE_COMMENT_UTF8:
			{
				const LexState commentState = (state == STATE_C_COMMENT_UTF8) ? STATE_C_COMMENT : STATE_LINE_COMMENT;
				const Private::Utf8Validator::Result result = utf8Validator.Feed(c);
				if (result == Private::Utf8Validator::COMPLETE)
				{
					state = commentState;
				}
				else if (result == Private::Utf8Validator::INVALID)
				{
					// The byte that ended the sequence is not part of it, so scan it again.
					stream.Reset(pos);
					token.SetStartPos(utf8SequenceStart);
					error = CompilerError::INVALID_UTF8;
					resumeState = commentState;
					state = STATE_DONE;
				}
			}
			break;

			case STATE_C_COMMENT_STAR:
				if (c == '/')
				{
					state = STATE_SPACE;
				}
				else if (c != '*')
				{
					// Scan the character again, since it may start a multi-byte character.
					stream.Reset(pos);
					state = STATE_C_COMMENT;
				}
				break;

			case STATE_INVALID_CHAR:
				if (!Private::IsUtf8ContinuationByte(c))
				{
					stream.Reset(pos);
					state = STATE_DONE;
				}
				break;

			case STATE_DONE:
				break;
		}
	}

	token.SetEndIndex(stream.GetStreamPos().index);

	// The artificial space at the end of the stream completes or invalidates any UTF-8 sequence or
	// Unicode escape, so scanning can't end in one of those states.
	if ((state == STATE_SPACE) || (state == STATE_LINE_COMMENT))
	{
		token.SetTokenType(Token::END);
		token.SetStartPos(stream.GetStreamPos());
	}
	else if ((state == STATE_CHAR) || (state == STATE_CHAR_ESCAPE) || (state == STATE_CHAR_END))
	{
		error = CompilerError::UNTERMINATED_CHARACTER;
	}
	else if ((state == STATE_STRING) || (state == STATE_STRING_ESCAPE))
	{
		error = CompilerError::UNTERMINATED_STRING;
	}
	else if ((state == STATE_C_COMMENT) || (state == STATE_C_COMMENT_STAR))
	{
		error = CompilerError::UNTERMINATED_COMMENT;
	}
	return error;
}


void LexerCore::ExtractToken(Token &token)
{
	if (token.GetTokenType() == Token::END)
	{
		token.SetText("EOF");
	}
	else
	{
		const size_t startIndex = token.GetStartPos().index;
		const size_t length = token.GetEndIndex() - startIndex;
		char *tokenString = AllocString(length);
		mStream.SetPosition(Stream::pos_t(startIndex));
		mStream.Read(tokenString, length);
		tokenString[length] = '\0';
		token.SetText(tokenString, length);
	}
}


void LexerCore::EvaluateToken(Token &token)
{
	switch (token.GetTokenType())
	{
		case Token::IDENTIFIER:
			EvaluateKeywordOrIdentifierToken(token);
			break;

		case Token::CONST_CHAR:
			EvaluateCharToken(token);
			break;

		case Token::CONST_INT:
		case Token::CONST_UINT:
			EvaluateIntegerToken(token);
			break;

		case Token::CONST_LONG:
		case Token::CONST_ULONG:
			EvaluateLongToken(token);
			break;

		case Token::CONST_FLOAT:
			EvaluateFloatToken(token);
			break;

		case Token::CONST_DOUBLE:
			EvaluateDoubleToken(token);
			break;

		case Token::CONST_STRING:
			EvaluateStringToken(token);
			break;

		default:
			// Fall through.
			break;
	}
}


void LexerCore::EvaluateKeywordOrIdentifierToken(Token &token) const
{
#define KEYWORD_LIST \
	KEYWORD_ITEM(KEY_ALIGNOF,   "alignof")   \
	KEYWORD_ITEM(KEY_BOOL,      "bool")      \
	KEYWORD_ITEM(KEY_BREAK,     "break")     \
	KEYWORD_ITEM(KEY_CASE,      "case")      \
	KEYWORD_ITEM(KEY_CAST,      "cast")      \
	KEYWORD_ITEM(KEY_CHAR,      "char")      \
	KEYWORD_ITEM(KEY_CONST,     "const")     \
	KEYWORD_ITEM(KEY_CONTINUE,  "continue")  \
	KEYWORD_ITEM(KEY_DEFAULT,   "default")   \
	KEYWORD_ITEM(KEY_DO,        "do")        \
	KEYWORD_ITEM(KEY_DOUBLE,    "double")    \
	KEYWORD_ITEM(KEY_ELSE,      "else")      \
	KEYWORD_ITEM(KEY_ENUM,      "enum")      \
	KEYWORD_ITEM(CONST_BOOL,    "false")     \
	KEYWORD_ITEM(KEY_FLOAT,     "float")     \
	KEYWORD_ITEM(KEY_FOR,       "for")       \
	KEYWORD_ITEM(KEY_IF,        "if")        \
	KEYWORD_ITEM(KEY_INCLUDE,   "include")   \
	KEYWORD_ITEM(KEY_INT,       "int")       \
	KEYWORD_ITEM(KEY_LONG,      "long")      \
	KEYWORD_ITEM(KEY_NAMESPACE, "namespace") \
	KEYWORD_ITEM(KEY_NATIVE,    "native")    \
	KEYWORD_ITEM(CONST_NULL,    "null")      \
	KEYWORD_ITEM(KEY_RETURN,    "return")    \
	KEYWORD_ITEM(KEY_SHORT,     "short")     \
	KEYWORD_ITEM(KEY_SIZEOF,    "sizeof")    \
	KEYWORD_ITEM(KEY_STRUCT,    "struct")    \
	KEYWORD_ITEM(KEY_SWITCH,    "switch")    \
	KEYWORD_ITEM(KEY_THIS,      "this")      \
	KEYWORD_ITEM(CONST_BOOL,    "true")      \
	KEYWORD_ITEM(KEY_UCHAR,     "uchar")     \
	KEYWORD_ITEM(KEY_UINT,      "uint")      \
	KEYWORD_ITEM(KEY_ULONG,     "ulong")     \
	KEYWORD_ITEM(KEY_USHORT,    "ushort")    \
	KEYWORD_ITEM(KEY_USING,     "using")     \
	KEYWORD_ITEM(KEY_VOID,      "void")      \
	KEYWORD_ITEM(KEY_WHILE,     "while")     \

	static const StringView KEYWORD_NAMES[] =
	{
#define KEYWORD_ITEM(type, name) StringView(name),
		KEYWORD_LIST
#undef KEYWORD_ITEM
	};

	static const Token::TokenType KEYWORD_TYPES[] =
	{
#define KEYWORD_ITEM(type, name) Token::type,
		KEYWORD_LIST
#undef KEYWORD_ITEM
	};

	const size_t numKeywords = sizeof(KEYWORD_NAMES) / sizeof(*KEYWORD_NAMES);
	const StringView *lastKeyword = KEYWORD_NAMES + numKeywords;
	const StringView *keyword = lower_bound(KEYWORD_NAMES, lastKeyword, token.GetText());

	if ((keyword != lastKeyword) && (*keyword == token.GetText()))
	{
		const Token::TokenType type = KEYWORD_TYPES[keyword - KEYWORD_NAMES];
		token.SetTokenType(type);
		if (type == Token::CONST_BOOL)
		{
			token.SetBoolValue(token.GetRawText()[0] == 't');
		}
	}
}


void LexerCore::EvaluateCharToken(Token &token)
{
	const CharResult result = EvaluateChar(token.GetRawText() + 1);
	token.SetIntValue(result.bytes[0]);

	if ((result.error == CompilerError::NO_ERROR) && (result.length > 1))
	{
		// A \u or \U escape for a character beyond ASCII.
		PushNonAsciiCharacterError(token, result.codePoint, result.length);
	}
	else
	{
		PushError(result.error, token);
	}
}


void LexerCore::EvaluateFloatToken(Token &token) const
{
	float value = strtof(token.GetRawText(), nullptr);
	token.SetFloatValue(value);
}


void LexerCore::EvaluateIntegerToken(Token &token)
{
	const char *text = token.GetRawText();
	const int base = token.HasAnnotation(Token::OCTAL) ? 8 : token.HasAnnotation(Token::HEX) ? 16 : 10;
	errno = 0;
	const unsigned long long rawValue = strtoull(text, nullptr, base);
	const bool outOfRange = (errno == ERANGE) || (rawValue > 0xFFFFFFFFull);
	const uint32_t value = uint32_t(rawValue);

	if (token.GetTokenType() == Token::CONST_INT)
	{
		token.SetIntValue(int32_t(value));
	}
	else
	{
		token.SetUIntValue(value);
	}

	if (outOfRange)
	{
		PushError(CompilerError::INT_OUT_OF_RANGE, token);
	}
}


void LexerCore::EvaluateLongToken(Token &token)
{
	const char *text = token.GetRawText();
	const int base = token.HasAnnotation(Token::OCTAL) ? 8 : token.HasAnnotation(Token::HEX) ? 16 : 10;
	errno = 0;
	const uint64_t value = uint64_t(strtoull(text, nullptr, base));
	const bool outOfRange = (errno == ERANGE);

	if (token.GetTokenType() == Token::CONST_LONG)
	{
		token.SetLongValue(int64_t(value));
	}
	else
	{
		token.SetULongValue(value);
	}

	if (outOfRange)
	{
		PushError(CompilerError::INT_OUT_OF_RANGE, token);
	}
}


void LexerCore::EvaluateDoubleToken(Token &token) const
{
	double value = strtod(token.GetRawText(), nullptr);
	token.SetDoubleValue(value);
}


void LexerCore::EvaluateStringToken(Token &token)
{
	// Allocate enough space for the string with the quotes stripped off. The string
	// could actually be shorter once escape sequences are expanded, since no escape
	// sequence is shorter than the bytes it evaluates to.
	const size_t allocLength = token.GetEndIndex() - token.GetStartPos().index - 2;
	char *buffer = AllocString(allocLength);
	char *dest = buffer;
	const char *source = token.GetRawText() + 1;
	const char *end = source + allocLength;
	size_t usedLength = 0;
	CompilerError::Type error = CompilerError::NO_ERROR;

	while (source < end)
	{
		CharResult result = EvaluateChar(source);
		source = result.end;
		memcpy(dest, result.bytes, result.length);
		dest += result.length;
		usedLength += result.length;
		error = (error == CompilerError::NO_ERROR) ? result.error : error;
	}

	*dest = '\0';
	token.SetStringValue(StringView(buffer, usedLength));
	PushError(error, token);
}


LexerCore::CharResult LexerCore::EvaluateChar(const char *text) const
{
	CharResult result;
	char &value = result.bytes[0];
	if (text[0] == '\\')
	{
		result.end = text + 2;
		switch (text[1])
		{
			case 'a': value = '\a'; break;
			case 'b': value = '\b'; break;
			case 'f': value = '\f'; break;
			case 'n': value = '\n'; break;
			case 'r': value = '\r'; break;
			case 't': value = '\t'; break;
			case 'v': value = '\v'; break;
			case '\'': value = '\''; break;
			case '\"': value = '\"'; break;
			case '\\': value = '\\'; break;
			case '\?': value = '\?'; break;

			case 'x':
			{
				char *end;
				const unsigned long long hexValue = strtoull(text + 2, &end, 16);
				value = char(hexValue);
				result.end = end;
				if (!IsInRange<uint8_t>(hexValue))
				{
					result.error = CompilerError::HEX_ESCAPE_RANGE;
				}
			}
			break;

			case 'u':
			case 'U':
			{
				// The scanner has verified that the exact number of hex digits is present. Parse
				// them by hand, since strtoull would consume any hex digits that follow.
				const size_t numDigits = (text[1] == 'u') ? 4 : 8;
				const char *digits = text + 2;
				uint32_t codePoint = 0;
				for (size_t i = 0; i < numDigits; ++i)
				{
					// Can't overflow, since 8 hex digits fit in 32 bits.
					codePoint = (codePoint << 4) | GetHexValue(digits[i]);
				}
				result.end = digits + numDigits;
				result.codePoint = codePoint;
				result.length = Private::EncodeUtf8(codePoint, result.bytes);
				if (result.length == 0)
				{
					result.error = CompilerError::UNICODE_ESCAPE_RANGE;
				}
			}
			break;

			default:
				if (IsOctalChar(text[1]))
				{
					char *end;
					const unsigned long long octalValue = strtoull(text + 1, &end, 8);
					value = char(octalValue);
					result.end = end;
					if (!IsInRange<uint8_t>(octalValue))
					{
						result.error = CompilerError::OCTAL_ESCAPE_RANGE;
					}
				}
				else
				{
					value = text[1];
				}
				break;
		}
	}
	else
	{
		result.end = text + 1;
		value = text[0];
	}

	if (result.length == 1)
	{
		result.codePoint = uint8_t(value);
	}

	return result;
}


bool LexerCore::IsDigitChar(char c) const
{
	return (c >= '0') && (c <= '9');
}


bool LexerCore::IsAlphaChar(char c) const
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}


bool LexerCore::IsSpaceChar(char c) const
{
	return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\v') || (c == '\f') || (c == '\r');
}


bool LexerCore::IsIdentifierChar(char c) const
{
	return IsAlphaChar(c) || IsDigitChar(c) || (c == '_');
}


bool LexerCore::IsOctalChar(char c) const
{
	return (c >= '0') && (c <= '7');
}


bool LexerCore::IsHexChar(char c) const
{
	return IsDigitChar(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}


bool LexerCore::IsBadNumberChar(char c) const
{
	return IsIdentifierChar(c) || (c == '.');
}


bool LexerCore::IsExponentChar(char c) const
{
	return (c == 'e') || (c == 'E');
}


bool LexerCore::IsFloatSuffixChar(char c) const
{
	return (c == 'f') || (c == 'F');
}


bool LexerCore::IsLongSuffixChar(char c) const
{
	return (c == 'l') || (c == 'L');
}


bool LexerCore::IsUnsignedSuffixChar(char c) const
{
	return (c == 'u') || (c == 'U');
}


bool LexerCore::IsEscapeChar(char c) const
{
	return (c == 'a') || (c == 'b') || (c == 'f') || (c == 'n') || (c == 'r') ||
		(c == 't') || (c == 'v') || (c == '\'') || (c == '\"') || (c == '\\') || (c == '\?');
}


uint32_t LexerCore::GetHexValue(char c) const
{
	if ((c >= 'a') && (c <= 'f'))
	{
		return uint32_t(c - 'a' + 10);
	}
	if ((c >= 'A') && (c <= 'F'))
	{
		return uint32_t(c - 'A' + 10);
	}
	return uint32_t(c - '0');
}


void LexerCore::PushError(CompilerError::Type type, Token &token)
{
	if (type != CompilerError::NO_ERROR)
	{
		token.SetTokenType(Token::INVALID);
		mErrorBuffer.PushError(type, &token);
	}
}


void LexerCore::PushInvalidCharacterError(Token &token)
{
	// Decode the token as a single UTF-8 character. The scanner has already grouped the lead byte
	// with any continuation bytes that follow it, so the token must be exactly one well-formed sequence.
	const StringView text = token.GetText();
	const Private::Utf8Decoded decoded = Private::DecodeUtf8(text.data(), text.size());
	const bool valid = decoded.valid && (decoded.length == text.size());

	token.SetTokenType(Token::INVALID);
	if (valid)
	{
		mErrorBuffer.PushErrorInt(CompilerError::INVALID_CHARACTER, &token, intptr_t(decoded.codePoint));
	}
	else
	{
		mErrorBuffer.PushError(CompilerError::INVALID_UTF8, &token);
	}
}


void LexerCore::PushNonAsciiCharacterError(Token &token, uint32_t codePoint, size_t length)
{
	token.SetTokenType(Token::INVALID);
	mErrorBuffer.PushErrorInt(CompilerError::NON_ASCII_CHARACTER_CONSTANT, &token, intptr_t(codePoint), intptr_t(length));
}


char *LexerCore::AllocString(size_t length)
{
	char *buffer = mStringBuffer;
	mStringBuffer += length + 1;
	return buffer;
}


char *LexerCore::AllocString(const char *content, size_t length)
{
	char *buffer = AllocString(length);
	memcpy(buffer, content, length);
	buffer[length] = '\0';
	return buffer;
}

}
