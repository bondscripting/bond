#include "bond/lexer.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>


namespace Bond
{

Lexer::~Lexer()
{
	delete [] mTokenBuffer;
	mTokenBuffer = NULL;
}


void Lexer::SetText(const char *scriptName, const char *text, int textLength)
{
	mScriptName = scriptName;
	mText = text;
	mTextLength = textLength;
	mTextIndex = 0;
	mLine = 1;
	mColumn = 1;

	// Since tokens must be null terminated and since we could have as many tokens as there are
	// characters in the source text, then in the worst case the token buffer needs to be twice
	// as long as the source text.
	if (mBufferLength < (2 * mTextLength))
	{
		delete [] mTokenBuffer;
		mTokenBuffer = new char[2 * mTextLength];
		mBufferLength = 2 * mTextLength;
	}
}


Token Lexer::NextToken()
{
	int startIndex = -1;
	int startLine = -1;
	int startColumn = -1;
	Token::TypeId type = Token::TYPEID_INVALID;
	LexState state = STATE_SPACE;

	while (HasMoreText() && (state != STATE_DONE))
	{
		const char c = GetNextTextChar();

		switch (state)
		{
			case STATE_SPACE:
				startIndex = mTextIndex - 1;
				startLine = mLine;
				startColumn = mColumn;

				if (c == '\n')
				{
					NextLine();
				}
				else if (c == '+')
				{
					state = STATE_PLUS;
				}
				else if (c == '-')
				{
					state = STATE_MINUS;
				}
				else if (c == '*')
				{
					state = STATE_DONE;
					type = Token::TYPEID_MULT;
				}
				else if (c == '/')
				{
					state = STATE_SLASH;
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
				else if (c == '|')
				{
					state = STATE_OR;
				}
				else if (c == '&')
				{
					state = STATE_AND;
				}
				else if (c == '.')
				{
					state = STATE_PERIOD;
				}
				else if (c == '(')
				{
					state = STATE_DONE;
					type = Token::TYPEID_OPAREN;
				}
				else if (c == ')')
				{
					state = STATE_DONE;
					type = Token::TYPEID_CPAREN;
				}
				else if (c == '{')
				{
					state = STATE_DONE;
					type = Token::TYPEID_OBRACE;
				}
				else if (c == '}')
				{
					state = STATE_DONE;
					type = Token::TYPEID_CBRACE;
				}
				else if (c == ';')
				{
					state = STATE_DONE;
					type = Token::TYPEID_SEMICOLON;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_E;
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

			case STATE_C_COMMENT:
				if (c == '*')
				{
					state = STATE_C_COMMENT_STAR;
				}
				else if (c == '\n')
				{
					NextLine();
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
					NextLine();
					state = STATE_SPACE;
				}
				break;

			case STATE_SLASH:
				if (c == '*')
				{
					state = STATE_C_COMMENT;
				}
				else if (c == '/')
				{
					state = STATE_LINE_COMMENT;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_DIV;
				}
				break;

			case STATE_PLUS:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_PLUS;
				}
				break;

			case STATE_MINUS:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_MINUS;
				}
				break;

			case STATE_LT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::TYPEID_LTE;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_LT;
				}
				break;

			case STATE_GT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::TYPEID_GTE;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_GT;
				}
				break;

			case STATE_EQUAL:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::TYPEID_EQUAL;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_ASSIGN;
				}
				break;
				
			case STATE_NOT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::TYPEID_NOT_EQUAL;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_NOT;
				}
				break;

			case STATE_OR:
				if (c == '|')
				{
					state = STATE_DONE;
					type = Token::TYPEID_OR;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
				}
				break;

			case STATE_AND:
				if (c == '&')
				{
					state = STATE_DONE;
					type = Token::TYPEID_AND;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
				}
				break;

			case STATE_IDIGITS:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_NUMBER;
				}
				break;

			case STATE_FDIGITS:
				if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_NUMBER;
				}
				break;

			case STATE_EDIGITS:
				if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_NUMBER;
				}
				break;

			case STATE_PERIOD:
				if (isdigit(c))
				{
					state = STATE_EDIGITS;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_PERIOD;
				}
				break;

			case STATE_E:
				if (isdigit(c))
				{
					state = STATE_EDIGITS;
				}
				else if ((c == '+') || (c == '-'))
				{
					state = STATE_E_SIGN;
				}
				else if (IsIdentifierChar(c))
				{
					state = STATE_IDENTIFIER;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_IDENTIFIER;
				}
				break;

			case STATE_E_SIGN:
				if (isdigit(c))
				{
					state = STATE_EDIGITS;
				}
				else
				{
					// The last character and the previous '+' or '-' are not part of the token;
					// roll them back.
					UngetTextChars(2);
					state = STATE_DONE;
					type = Token::TYPEID_IDENTIFIER;
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
				else
				{
					// The last character and the previous 'e' are not part of the token; roll them back.
					UngetTextChars(2);
					state = STATE_DONE;
					type = Token::TYPEID_NUMBER;
				}
				break;

			case STATE_EXPONENT_SIGN:
				if (isdigit(c))
				{
					state = STATE_EDIGITS;
				}
				else
				{
					// The last character, the previous '+' or '-' and the previous 'e' are not
					// part of the token; roll them back.
					UngetTextChars(3);
					state = STATE_DONE;
					type = Token::TYPEID_NUMBER;
				}
				break;

			case STATE_IDENTIFIER:
				if (!IsIdentifierChar(c))
				{
					// The last character is not part of the token; roll it back.
					UngetTextChars(1);
					state = STATE_DONE;
					type = Token::TYPEID_IDENTIFIER;
				}
				break;

			case STATE_DONE:
				break;
		}
	}

	if ((state == STATE_SPACE) || (state == STATE_LINE_COMMENT))
	{
		return Token(Value(), Token::TYPEID_EOF, "EOF", mLine, mColumn, mTextIndex);
	}

	if (((state == STATE_C_COMMENT) || (state == STATE_C_COMMENT_STAR)) &&
	    !mAllowInvalidTokens)
	{
		//throw new CompilerException(String.Format("{0}({1},{2}): Unterminated comment.", mScriptName, startLine, startColumn));
	}

	const char *text = NULL; //mText.Substring(startIndex, mTextIndex - startIndex);
	//float value = 0.0f;

	// Distinguish keywords from other identifiers.
	if (type == Token::TYPEID_IDENTIFIER)
	{
		if (strcmp(text, "bool") == 0)
		{
			type = Token::TYPEID_BOOL;
		}
		else if (strcmp(text, "float") == 0)
		{
			type = Token::TYPEID_FLOAT;
		}
		else if (strcmp(text, "false") == 0)
		{
			type = Token::TYPEID_FALSE;
		}
		else if (strcmp(text, "true") == 0)
		{
			type = Token::TYPEID_TRUE;
		}
		else if (strcmp(text, "if") == 0)
		{
			type = Token::TYPEID_IF;
		}
		else if (strcmp(text, "else") == 0)
		{
			type = Token::TYPEID_ELSE;
		}
		else if (strcmp(text, "while") == 0)
		{
			type = Token::TYPEID_WHILE;
		}
	}
	else if (type == Token::TYPEID_NUMBER)
	{
		//Single.TryParse(text, out value);			
	}
	else if (type == Token::TYPEID_INVALID)
	{
		if (!mAllowInvalidTokens)
		{
			//throw new CompilerException(String.Format("{0}({1},{2}): Invalid token '{2}'.", mScriptName, startLine, startColumn, text));
		}
	}

	return Token(Value(), type, text, startLine, startColumn, startIndex);

	//return Token(Token::TYPEID_EOF, "", 0, 0, 0);
}


bool Lexer::HasMoreText() const
{
	// Use <= instead of < since we're artificially introducing a space at the end to ensure
	// that the last token is properly parsed.
	return mTextIndex <= mTextLength;
}


char Lexer::GetNextTextChar()
{
	// Aritificially introduce a space as the last character to ensure that the last token
	// is properly parsed.
	char c = (mTextIndex >= mTextLength) ? ' ' : mText[mTextIndex];
	++mTextIndex;
	++mColumn;
	return c;
}


void Lexer::UngetTextChars(int numChars)
{
	mTextIndex -= numChars;
	mColumn -= numChars;
}


void Lexer::PushTokenChar(char c)
{
	assert(mBufferIndex < mBufferLength);
	mTokenBuffer[mBufferIndex++] = c;
}


const char *Lexer::TerminateToken()
{
	assert(mBufferIndex < mBufferLength);
	mTokenBuffer[mBufferIndex++] = '\0';
	const char *token = mCurrentToken;
	mCurrentToken = mTokenBuffer + mBufferIndex;
	return token;
}


void Lexer::NextLine()
{
	++mLine;
	mColumn = 0;						
}


bool Lexer::IsIdentifierChar(char c)
{
	return  isalnum(c) || (c == '_');
}

}
