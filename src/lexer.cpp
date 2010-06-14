#include "bond/lexer.h"
#include <ctype.h>

namespace Bond
{

void Lexer::SetText(const char *scriptName, const char *text, int textLength)
{
	//mTokenStack.Clear();
	mScriptName = scriptName;
	mText = text;
	mTextLength = textLength;
	mIndex = 0;
	mLine = 1;
	mColumn = 1;
}


bool Lexer::HasMoreCharacters() const
{
	// Use <= instead of < since we're artificially introducing a space at the end to ensure
	// that the last token is properly parsed.
	return mIndex <= mTextLength;
}


char Lexer::NextCharacter()
{
	// Aritificially introduce a space as the last character to ensure that the last token
	// is properly parsed.
	char c = (mIndex >= mTextLength) ? ' ' : mText[mIndex];
	++mIndex;
	return c;
}

void Lexer::NextLine()
{
	++mLine;
	mColumn = 0;						
}


void Lexer::RollBack(int numCharacters)
{
	mIndex -= numCharacters;
	mColumn -= numCharacters;
}


bool Lexer::IsIdentifierChar(char c)
{
	return  isalnum(c) || (c == '_');
}

}
