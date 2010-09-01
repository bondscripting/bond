#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/allocator.h"
#include "bond/charstream.h"
#include "bond/stringallocator.h"
#include "bond/tokenstream.h"

namespace Bond
{

class Lexer
{
public:
	Lexer(Allocator &allocator):
		mAllocator(allocator),
		mStringBuffer(0),
		mTokens(0),
		mNumTokens(0)
	{
	}

	~Lexer();
	void Dispose();

	void Lex(const char *text, int length);
	TokenStream GetTokenStream() const { return TokenStream(mTokens, mNumTokens); }

private:
	//integral-digits[.[fractional-digits]][e[sign]exponential-digits]
	enum LexState
	{
		STATE_SPACE,             // Whitespace.
		STATE_PLUS,              // '+'
		STATE_MINUS,             // '-'
		STATE_STAR,              // '*'
		STATE_SLASH,             // '/'
		STATE_PERCENT,           // '%'
		STATE_LT,                // '<'	
		STATE_GT,                // '>'
		STATE_LEFT,              // '<<'
		STATE_RIGHT,             // '>>'
		STATE_EQUAL,             // '='
		STATE_NOT,               // '!'
		STATE_AND,               // '&'
		STATE_OR,                // '|'
		STATE_XOR,               // '^'
		STATE_COLON,             // ':'
		STATE_ZERO,              // '0'
		STATE_OCTAL,             // Octal integer.
		STATE_HEX,               // Hex integer.
		STATE_IDIGITS,           // Integral digits.
		STATE_FDIGITS,           // Fractional digits.
		STATE_EDIGITS,           // Exponential digits.
		STATE_PERIOD,            // '.'
		STATE_EXPONENT,          // 'e' or 'E' known to be inside a number.
		STATE_EXPONENT_SIGN,     // '+' or '-' known to be inside a number's exponent.
		STATE_BAD_NUMBER,        // Consume the remaining characters of an incorrectly formatted number.
		STATE_IDENTIFIER,        // Sequence of characters forming an identifier.
		STATE_CHAR,              // Character literal.
		STATE_CHAR_ESCAPE,       // Escape sequence within a character literal.
		STATE_CHAR_END,          // Expect the end quote of a character literal..
		STATE_BAD_CHAR,          // Consume the remaining characters of an incorrectly formated character literal.
		STATE_BAD_CHAR_ESCAPE,   // Consume the remaining characters of an incorrectly formated character literal.
		STATE_STRING,            // String literal.
		STATE_STRING_ESCAPE,     // Escape sequence within a string literal.
		STATE_BAD_STRING,        // Consume the remaining characters of an incorrectly formatted literal.
		STATE_BAD_STRING_ESCAPE, // Consume the remaining characters of an incorrectly formatted literal.
		STATE_C_COMMENT,         // C style comment.
		STATE_C_COMMENT_STAR,    // Star within a C style comment.
		STATE_LINE_COMMENT,      // Single line C++ style comment.
		STATE_DONE               // Done parsing the current token.
	};

	struct Resources
	{
		int numTokens;
		int stringBufferLength;
	};

	struct CharResult
	{
		const char *end;
		char value;
	};

	// Copying disallowed.
	Lexer(const Lexer &other);
	Lexer &operator=(const Lexer &other);

	void CalculateResources(CharStream &stream, Resources &resources) const;

	void GenerateTokens(CharStream &stream, StringAllocator &allocator);
	void GenerateToken(CharStream &stream, StringAllocator &allocator, Token &token) const;
	void ScanToken(CharStream &stream, Token &token) const;
	void ExtractToken(CharStream &stream, StringAllocator &allocator, Token &token) const;

	void EvaluateToken(StringAllocator &allocator, Token &token) const;
	void EvaluateKeywordToken(Token &token) const;
	void EvaluateCharToken(Token &token) const;
	void EvaluateFloatToken(Token &token) const;
	void EvaluateIntegerToken(Token &token) const;
	void EvaluateStringToken(StringAllocator &allocator, Token &token) const;
	CharResult EvaluateChar(const char *text) const;

	bool IsIdentifierChar(char c) const;
	bool IsOctalChar(char c) const;
	bool IsHexChar(char c) const;
	bool IsBadNumberChar(char c) const;
	bool IsUnsignedSuffixChar(char c) const;
	bool IsEscapeChar(char c) const;

	Allocator &mAllocator;
	char *mStringBuffer;
	Token *mTokens;
	int mNumTokens;
};

}

#endif
