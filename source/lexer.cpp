#include "bond/compiler/charstream.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"
#include "bond/io/memoryinputstream.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/allocator.h"
#include "private/memory.h"
#include <cctype>
#include <cstdio>
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
		STATE_CHAR_END,            // Expect the end quote of a character literal..
		STATE_STRING,              // String literal.
		STATE_STRING_ESCAPE,       // Escape sequence within a string literal.
		STATE_STRING_OCTAL_ESCAPE, // Octal escape within a string literal.
		STATE_STRING_HEX_ESCAPE_X, // \x at the start of a hex escape within string literal.
		STATE_STRING_HEX_ESCAPE,   // Hex escape within a string literal.
		STATE_C_COMMENT,           // C style comment.
		STATE_C_COMMENT_STAR,      // Star within a C style comment.
		STATE_LINE_COMMENT,        // Single line C++ style comment.
		STATE_DONE                 // Done parsing the current token.
	};

	struct Resources
	{
		Resources() : numTokens(0), stringBufferLength(0) {}

		size_t numTokens;
		size_t stringBufferLength;
	};

	struct CharResult
	{
		CharResult(): error(CompilerError::NO_ERROR), end(nullptr), value(0) {}

		CompilerError::Type error;
		const char *end;
		char value;
	};

	Resources CalculateResources(CharStream &stream) const;

	void GenerateTokens(CharStream &stream);
	void GenerateToken(CharStream &stream, Token &token);
	CompilerError::Type ScanToken(CharStream &stream, Token &token) const;
	void ExtractToken(Token &token);

	void EvaluateToken(Token &token);
	void EvaluateKeywordOrIdentifierToken(Token &token) const;
	void EvaluateCharToken(Token &token);
	void EvaluateIntegerToken(Token &token) const;
	void EvaluateLongToken(Token &token) const;
	void EvaluateFloatToken(Token &token) const;
	void EvaluateDoubleToken(Token &token) const;
	void EvaluateStringToken(Token &token);
	CharResult EvaluateChar(const char *text) const;

	bool IsIdentifierChar(char c) const;
	bool IsOctalChar(char c) const;
	bool IsHexChar(char c) const;
	bool IsBadNumberChar(char c) const;
	bool IsExponentChar(char c) const;
	bool IsFloatSuffixChar(char c) const;
	bool IsLongSuffixChar(char c) const;
	bool IsUnsignedSuffixChar(char c) const;
	bool IsEscapeChar(char c) const;

	void PushError(CompilerError::Type type, Token &token);

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

	TokenCollection *tokenCollection = new (memHandle.get() + tokenCollectionStart) TokenCollection(mTokens, static_cast<int>(resources.numTokens));
	memHandle.release();

	return TokenCollectionHandle(mAllocator, tokenCollection);
}


LexerCore::Resources LexerCore::CalculateResources(CharStream &stream) const
{
	Resources resources;

	Token token;
	for (;;)
	{
		ScanToken(stream, token);

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

	while (!done)
	{
		Token &token = mTokens[tokenIndex++];
		GenerateToken(stream, token);
		done = token.GetTokenType() == Bond::Token::END;
	}
}


void LexerCore::GenerateToken(CharStream &stream, Token &token)
{
	const CompilerError::Type error = ScanToken(stream, token);
	token.SetFileName(mFileName);
	PushError(error, token);
	ExtractToken(token);
	EvaluateToken(token);
}


CompilerError::Type LexerCore::ScanToken(CharStream &stream, Token &token) const
{
	CompilerError::Type error = CompilerError::NO_ERROR;
	token = Token();
	LexState state = STATE_SPACE;

	stream.Sync();
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
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else if (isalpha(c) || (c == '_'))
				{
					state = STATE_IDENTIFIER;
				}
				else if (!isspace(c))
				{
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
				else if (isdigit(c))
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
				else if (isdigit(c))
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
				if (isdigit(c))
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
				if (isdigit(c))
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
				if (isdigit(c))
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
				if (isdigit(c))
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
				else
				{
					state = STATE_CHAR_END;
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

			case STATE_C_COMMENT:
				if (c == '*')
				{
					state = STATE_C_COMMENT_STAR;
				}
				break;

			case STATE_C_COMMENT_STAR:
				if (c == '/')
				{
					state = STATE_SPACE;
				}
				else if (c != '*')
				{
					state = STATE_C_COMMENT;
				}
				break;

			case STATE_LINE_COMMENT:
				if (c == '\n')
				{
					state = STATE_SPACE;
				}
				break;

			case STATE_DONE:
				break;
		}
	}

	token.SetEndIndex(stream.GetStreamPos().index);

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
	KEYWORD_ITEM(CONST_BOOL,    "true")		  \
	KEYWORD_ITEM(KEY_UCHAR,     "uchar")     \
	KEYWORD_ITEM(KEY_UINT,      "uint")      \
	KEYWORD_ITEM(KEY_ULONG,     "ulong")     \
	KEYWORD_ITEM(KEY_USHORT,    "ushort")    \
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
	token.SetIntValue(result.value);
	PushError(result.error, token);
}


void LexerCore::EvaluateFloatToken(Token &token) const
{
	float value;
	sscanf(token.GetRawText(), "%f", &value);
	token.SetFloatValue(value);
}


void LexerCore::EvaluateIntegerToken(Token &token) const
{
	uint32_t value;
	const char *text = token.GetRawText();
	if (token.HasAnnotation(Token::OCTAL))
	{
		sscanf(text, "%" BOND_SCNo32, &value);
	}
	else if (token.HasAnnotation(Token::HEX))
	{
		sscanf(text, "%" BOND_SCNx32, &value);
	}
	else
	{
		sscanf(text, "%" BOND_SCNu32, &value);
	}

	if (token.GetTokenType() == Token::CONST_INT)
	{
		token.SetIntValue(int32_t(value));
	}
	else
	{
		token.SetUIntValue(value);
	}
}


void LexerCore::EvaluateLongToken(Token &token) const
{
	uint64_t value;
	const char *text = token.GetRawText();
	if (token.HasAnnotation(Token::OCTAL))
	{
		sscanf(text, "%" BOND_SCNo64, &value);
	}
	else if (token.HasAnnotation(Token::HEX))
	{
		sscanf(text, "%" BOND_SCNx64, &value);
	}
	else
	{
		sscanf(text, "%" BOND_SCNu64, &value);
	}

	if (token.GetTokenType() == Token::CONST_LONG)
	{
		token.SetLongValue(int64_t(value));
	}
	else
	{
		token.SetULongValue(value);
	}
}


void LexerCore::EvaluateDoubleToken(Token &token) const
{
	double value;
	sscanf(token.GetRawText(), "%lf", &value);
	token.SetDoubleValue(value);
}


void LexerCore::EvaluateStringToken(Token &token)
{
	// Allocate enough space for the string with the quotes stripped off. The string
	// could actually be shorter once escape sequences are expanded.
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
		*dest = result.value;
		++dest;
		++usedLength;
		error = (error == CompilerError::NO_ERROR) ? result.error : error;
	}

	*dest = '\0';
	token.SetStringValue(StringView(buffer, usedLength));
	PushError(error, token);
}


LexerCore::CharResult LexerCore::EvaluateChar(const char *text) const
{
	CharResult result;
	if (text[0] == '\\')
	{
		result.end = text + 2;
		switch (text[1])
		{
			case 'a': result.value = '\a'; break;
			case 'b': result.value = '\b'; break;
			case 'f': result.value = '\f'; break;
			case 'n': result.value = '\n'; break;
			case 'r': result.value = '\r'; break;
			case 't': result.value = '\t'; break;
			case 'v': result.value = '\v'; break;
			case '\'': result.value = '\''; break;
			case '\"': result.value = '\"'; break;
			case '\\': result.value = '\\'; break;
			case '\?': result.value = '\?'; break;

			case 'x':
			{
				uint32_t value;
				int length;
				sscanf(text + 2, "%" BOND_SCNx32 "%n", &value, &length);
				result.value = char(value);
				result.end = text + 2 + length;
				if (!IsInRange<uint8_t>(value))
				{
					result.error = CompilerError::HEX_ESCAPE_RANGE;
				}
			}
			break;

			default:
				if (IsOctalChar(text[1]))
				{
					uint32_t value;
					int length;
					sscanf(text + 1, "%" BOND_SCNo32 "%n", &value, &length);
					result.value = char(value);
					result.end = text + 1 + length;
					if (!IsInRange<uint8_t>(value))
					{
						result.error = CompilerError::OCTAL_ESCAPE_RANGE;
					}
				}
				else
				{
					result.value = text[1];
				}
				break;
		}
	}
	else
	{
		result.end = text + 1;
		result.value = text[0];
	}

	return result;
}


bool LexerCore::IsIdentifierChar(char c) const
{
	return  isalnum(c) || (c == '_');
}


bool LexerCore::IsOctalChar(char c) const
{
	return (c >= '0') && (c <= '7');
}


bool LexerCore::IsHexChar(char c) const
{
	return isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
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


void LexerCore::PushError(CompilerError::Type type, Token &token)
{
	if (type != CompilerError::NO_ERROR)
	{
		token.SetTokenType(Token::INVALID);
		mErrorBuffer.PushError(type, &token);
	}
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
