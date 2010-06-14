#ifndef BOND_LEXER_H
#define BOND_LEXER_H

#include "bond/token.h"
#include <string>

namespace Bond
{

class Lexer
{
public:
	Lexer():
		//mTokenStack(),
		mScriptName(""),
		mText(""),
		mTextLength(0),
		mIndex(0),
		mLine(1),
		mColumn(1),
		mAllowInvalidTokens(false)
	{
	}

	void SetText(const char *scriptName, const char *text, int mTextLength);
	void AllowInvalidTokens() { mAllowInvalidTokens = true; }
	void DisllowInvalidTokens() { mAllowInvalidTokens = false; }


private:

	//[sign]integral-digits[.[fractional-digits]][e[sign]exponential-digits]
	enum LexState
	{
		SPACE,          // Whitespace
		C_COMMENT,      // C style comment
		C_COMMENT_STAR, // Star within a C style comment
		LINE_COMMENT,   // Single line C++ style comment
		SLASH,          // '/'
		PLUS,           // '+'
		MINUS,          // '-'
		LT,             // '<'
		GT,             // '>'
		EQUAL,          // '='
		NOT,            // '!'
		OR,             // '|'
		AND,            // '&'
		IDIGITS,        // Integral digits
		FDIGITS,        // Fractional digits
		EDIGITS,        // Exponential digits
		PERIOD,         // '.'
		E,              // 'e' or 'E'
		E_SIGN,         // '+' or '-' following 'e' or 'E'
		EXPONENT,       // 'e' or 'E' known to be inside a number
		EXPONENT_SIGN,  // '+' or '-' known to be inside a number's exponent
		IDENTIFIER,     // Sequence of characters forming an identifier
		DONE            // Done parsing the current token
	};

	bool HasMoreCharacters() const;
	char NextCharacter();
	void NextLine();
	void RollBack(int numCharacters);
	bool IsIdentifierChar(char c);

	//Stack mTokenStack;
	const char *mScriptName;
	const char *mText;
	int mTextLength;
	int mIndex;
	int mLine;
	int mColumn;
	bool mAllowInvalidTokens;
};

}

#endif
