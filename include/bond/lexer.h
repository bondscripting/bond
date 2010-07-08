#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/charstream.h"
#include "bond/stringallocator.h"
#include "bond/token.h"

namespace Bond
{

class Lexer
{
public:
	Lexer():
		mStringBuffer(0),
		mTokens(0),
		mNumTokens(0)
	{
	}

	~Lexer();

	void Lex(const char *text, int length);

	//Token NextToken();

private:

	//[sign]integral-digits[.[fractional-digits]][e[sign]exponential-digits]
	enum LexState
	{
		STATE_SPACE,          // Whitespace
		STATE_C_COMMENT,      // C style comment
		STATE_C_COMMENT_STAR, // Star within a C style comment
		STATE_LINE_COMMENT,   // Single line C++ style comment
		STATE_SLASH,          // '/'
		STATE_PLUS,           // '+'
		STATE_MINUS,          // '-'
		STATE_LT,             // '<'	
		STATE_GT,             // '>'
		STATE_EQUAL,          // '='
		STATE_NOT,            // '!'
		STATE_OR,             // '|'
		STATE_AND,            // '&'
		STATE_ZERO,           // '0'
		STATE_OCTAL,          // Octal integer
		STATE_HEX,            // Hex integer
		STATE_IDIGITS,        // Integral digits
		STATE_FDIGITS,        // Fractional digits
		STATE_EDIGITS,        // Exponential digits
		STATE_PERIOD,         // '.'
		STATE_E,              // 'e' or 'E'
		STATE_E_SIGN,         // '+' or '-' following 'e' or 'E'
		STATE_EXPONENT,       // 'e' or 'E' known to be inside a number
		STATE_EXPONENT_SIGN,  // '+' or '-' known to be inside a number's exponent
		STATE_IDENTIFIER,     // Sequence of characters forming an identifier
		STATE_DONE            // Done parsing the current token
	};

	struct Resources
	{
		int numTokens;
		int stringBufferLength;
	};

	void Dispose();
	void CalculateResources(CharStream &stream, Resources &resources) const;
	void ScanToken(CharStream &stream, Token &token) const;

	static bool IsIdentifierChar(char c);
	static bool IsOctalChar(char c);
	static bool IsHexChar(char c);

	char *mStringBuffer;
	Token *mTokens;
	int mNumTokens;
};

}

#endif
