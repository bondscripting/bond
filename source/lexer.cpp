#include "bond/lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


namespace Bond
{

Lexer::~Lexer()
{
	delete [] mTokenBuffer;
	mTokenBuffer = NULL;
}


void Lexer::SetText(const char *text, int length)
{
	mStream.SetBuffer(text, length);
	mBufferIndex = 0;

	// Since tokens must be null terminated and since we could have as many tokens as there are
	// characters in the source text, then in the worst case the token buffer needs to be twice
	// as long as the source text.
	delete [] mTokenBuffer;
	mTokenBuffer = new char[2 * length];
	mBufferLength = 2 * length;
}


Token Lexer::NextToken()
{
	StreamPos startPos;
	StreamPos endPos;
	StreamPos errorPos;
	Value value;
	const char *tokenString = "";
	Token::TokenType type = Token::INVALID;
	Token::ErrorType error = Token::NO_ERROR;
	LexState state = STATE_SPACE;

	while (mStream.HasNext() && (state != STATE_DONE))
	{
		StreamPos pos = mStream.GetStreamPos();
		const char c = mStream.Next();

		switch (state)
		{
			case STATE_SPACE:
				startPos = pos;

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
					state = STATE_DONE;
					type = Token::OP_MULT;
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
					type = Token::OPAREN;
				}
				else if (c == ')')
				{
					state = STATE_DONE;
					type = Token::CPAREN;
				}
				else if (c == '{')
				{
					state = STATE_DONE;
					type = Token::OBRACE;
				}
				else if (c == '}')
				{
					state = STATE_DONE;
					type = Token::CBRACE;
				}
				else if (c == ';')
				{
					state = STATE_DONE;
					type = Token::SEMICOLON;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_E;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_DIV;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_PLUS;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_MINUS;
				}
				break;

			case STATE_LT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::OP_LTE;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_LT;
				}
				break;

			case STATE_GT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::OP_GTE;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_GT;
				}
				break;

			case STATE_EQUAL:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::OP_EQUAL;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::ASSIGN;
				}
				break;
				
			case STATE_NOT:
				if (c == '=')
				{
					state = STATE_DONE;
					type = Token::OP_NOT_EQUAL;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::OP_NOT;
				}
				break;

			case STATE_OR:
				if (c == '|')
				{
					state = STATE_DONE;
					type = Token::OP_OR;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
				}
				break;

			case STATE_AND:
				if (c == '&')
				{
					state = STATE_DONE;
					type = Token::OP_AND;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
				}
				break;

			case STATE_ZERO:
				if (c == '.')
				{
					state = STATE_FDIGITS;
				}
				else if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (c == 'u')
				{
					state = STATE_DONE;
					type = Token::VAL_UINT;
				}
				else if (c == 'x')
				{
					state = STATE_HEX;
				}
				else if (IsOctalChar(c))
				{
					state = STATE_OCTAL;
				}
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::VAL_INT;
				}
				break;

			case STATE_OCTAL:
				break;

			case STATE_HEX:
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
				else if (c == 'u')
				{
					state = STATE_DONE;
					type = Token::VAL_UINT;
				}
				else if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::VAL_INT;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::VAL_FLOAT;
				}
				break;

			case STATE_EDIGITS:
				if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::VAL_FLOAT;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::PERIOD;
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
					mStream.Unget();
					state = STATE_DONE;
					type = Token::IDENTIFIER;
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
					mStream.Unget(2);
					state = STATE_DONE;
					type = Token::IDENTIFIER;
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
					mStream.Unget(2);
					state = STATE_DONE;
					type = Token::VAL_FLOAT;
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
					mStream.Unget(3);
					state = STATE_DONE;
					type = Token::VAL_FLOAT;
				}
				break;

			case STATE_IDENTIFIER:
				if (!IsIdentifierChar(c))
				{
					// The last character is not part of the token; roll it back.
					mStream.Unget();
					state = STATE_DONE;
					type = Token::IDENTIFIER;
				}
				break;

			case STATE_DONE:
				break;
		}
	}

	endPos = mStream.GetStreamPos();

	if ((state == STATE_SPACE) || (state == STATE_LINE_COMMENT))
	{
		tokenString = "END";
		type = Token::END;
	}
	else if ((state == STATE_C_COMMENT) || (state == STATE_C_COMMENT_STAR))
	{
		error = Token::UNTERMINATED_COMMENT;
		errorPos = startPos;
	}
	else
	{
		tokenString = CreateTokenString(startPos.index, endPos.index - startPos.index);

		// Distinguish keywords from other identifiers.
		if (type == Token::IDENTIFIER)
		{
			if (strcmp(tokenString, "bool") == 0)
			{
				type = Token::KEY_BOOL;
			}
			else if (strcmp(tokenString, "char") == 0)
			{
				type = Token::KEY_CHAR;
			}
			else if (strcmp(tokenString, "float") == 0)
			{
				type = Token::KEY_FLOAT;
			}
			else if (strcmp(tokenString, "int") == 0)
			{
				type = Token::KEY_INT;
			}
			else if (strcmp(tokenString, "uint") == 0)
			{
				type = Token::KEY_UINT;
			}
			else if (strcmp(tokenString, "if") == 0)
			{
				type = Token::KEY_IF;
			}
			else if (strcmp(tokenString, "else") == 0)
			{
				type = Token::KEY_ELSE;
			}
			else if (strcmp(tokenString, "while") == 0)
			{
				type = Token::KEY_WHILE;
			}
			else if (strcmp(tokenString, "false") == 0)
			{
				type = Token::VAL_BOOL;
				value.mBool = false;
			}
			else if (strcmp(tokenString, "true") == 0)
			{
				type = Token::VAL_BOOL;
				value.mBool = true;
			}
		}
		else if (type == Token::VAL_INT)
		{
			sscanf(tokenString, BOND_INT_SCAN_FORMAT, &value.mInt);
		}
		else if (type == Token::VAL_UINT)
		{
			sscanf(tokenString, BOND_UINT_SCAN_FORMAT, &value.mUInt);
		}
		else if (type == Token::VAL_FLOAT)
		{
			sscanf(tokenString, BOND_FLOAT_SCAN_FORMAT, &value.mFloat);
		}
	}

	return Token(startPos, endPos, errorPos, value, tokenString, type, error);
}


const char *Lexer::CreateTokenString(int startIndex, int numChars)
{
	assert((mBufferIndex + numChars + 1) < mBufferLength);

	const char *tokenString = mTokenBuffer + mBufferIndex;
	memcpy(mTokenBuffer + mBufferIndex, mStream.GetBuffer() + startIndex, numChars);
	mBufferIndex += numChars;
	mTokenBuffer[mBufferIndex++] = '\0';

	return tokenString;
}


bool Lexer::IsIdentifierChar(char c)
{
	return  isalnum(c) || (c == '_');
}


bool Lexer::IsOctalChar(char c)
{
	return (c >= '0') && (c <= '8');
}


bool Lexer::IsHexChar(char c)
{
	return isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

}