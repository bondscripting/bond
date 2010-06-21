
#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/token.h"
#include <stdlib.h>

namespace Bond
{

class Lexer
{
public:
	Lexer():
		mScriptName(""),
		mText(""),
		mTokenBuffer(NULL),
		mTextLength(0),
		mTextIndex(0),
		mLine(1),
		mColumn(1)
	{
	}

	~Lexer();

	void SetText(const char *scriptName, const char *text, int mTextLength);

	Token NextToken();

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

	// Functions for manipulating the input text buffer.
	bool HasMoreText() const;
	char GetNextTextChar();
	void UngetTextChars(int numChars);
	void NextLine();

	// Functions for manipulating the token buffer.
	//void PushTokenChar(char c);
	//const char *TerminateToken();
	const char *CreateTokenString(int startIndex, int numChars);

	static bool IsIdentifierChar(char c);

	const char *mScriptName;
	const char *mText;
	char *mTokenBuffer;
	int mTextLength;
	int mTextIndex;
	int mBufferLength;
	int mBufferIndex;
	int mLine;
	int mColumn;
};

}

#endif
