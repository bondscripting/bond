#include "bond/lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


namespace Bond
{

Lexer::~Lexer()
{
	Dispose();
}


void Lexer::Dispose()
{
	delete [] mStringBuffer;
	mStringBuffer = 0;
	delete [] mTokens;
	mTokens = 0;
	mNumTokens = 0;
}


void Lexer::Lex(const char *text, int length)
{
	Dispose();

	CharStream stream(text, length);
	Resources resources;

	CalculateResources(stream, resources);

	mStringBuffer = new char[resources.stringBufferLength];
	StringAllocator stringAllocator(mStringBuffer, resources.stringBufferLength);

	mTokens = new Token[resources.numTokens];
	mNumTokens = resources.numTokens;
}


void Lexer::CalculateResources(CharStream &stream, Resources &resources) const
{
	resources.numTokens = 0;
	resources.stringBufferLength = 0;

	while (true)
	{
		Token token;
		ScanToken(stream, token);

		int length = token.GetEndPos().index - token.GetStartPos().index + 1;
		if (token.GetTokenType() == Token::VAL_STRING)
		{
			length *=2;
		}

		resources.stringBufferLength += length;
		++resources.numTokens;

		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}
}


void Lexer::GenerateTokens(CharStream &stream, StringAllocator &allocator)
{
	int tokenIndex = 0;

	while (true)
	{
		assert(tokenIndex < mNumTokens);
		Token &token = mTokens[tokenIndex++];
		GenerateToken(stream, allocator, token);

		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}

	assert(tokenIndex == mNumTokens);
}


void Lexer::GenerateToken(CharStream &stream, StringAllocator &allocator, Token &token) const
{
	ScanToken(stream, token);
	ExtractToken(stream, allocator, token);
	EvaluateToken(allocator, token);
}


void Lexer::ScanToken(CharStream &stream, Token &token) const
{
	LexState state = STATE_SPACE;

	while (stream.HasNext() && (state != STATE_DONE))
	{
		StreamPos pos = stream.GetStreamPos();
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
					state = STATE_DONE;
					token.SetTokenType(Token::OP_MULT);
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
					token.SetTokenType(Token::OPAREN);
				}
				else if (c == ')')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::CPAREN);
				}
				else if (c == '{')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OBRACE);
				}
				else if (c == '}')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::CBRACE);
				}
				else if (c == ';')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::SEMICOLON);
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
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_DIV);
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
				else if (c == '0')
				{
					state = STATE_ZERO;
				}
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_PLUS);
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
				else if (c == '0')
				{
					state = STATE_ZERO;
				}
				else if (isdigit(c))
				{
					state = STATE_IDIGITS;
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_MINUS);
				}
				break;

			case STATE_LT:
				if (c == '=')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_LTE);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_LT);
				}
				break;

			case STATE_GT:
				if (c == '=')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_GTE);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_GT);
				}
				break;

			case STATE_EQUAL:
				if (c == '=')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_EQUAL);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::ASSIGN);
				}
				break;
				
			case STATE_NOT:
				if (c == '=')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_NOT_EQUAL);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::OP_NOT);
				}
				break;

			case STATE_OR:
				if (c == '|')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_OR);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
				}
				break;

			case STATE_AND:
				if (c == '&')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::OP_AND);
				}
				else
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
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
					token.SetTokenType(Token::VAL_UINT);
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
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_INT);
				}
				break;

			case STATE_OCTAL:
				if (c == 'u')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_UINT);
					token.AddAnnotation(Token::OCTAL);
				}
				else if (!IsOctalChar(c))
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_INT);
					token.AddAnnotation(Token::OCTAL);
				}

			case STATE_HEX:
				if (c == 'u')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_UINT);
					token.AddAnnotation(Token::HEX);
				}
				else if (!IsHexChar(c))
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_INT);
					token.AddAnnotation(Token::HEX);
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
				else if (c == 'u')
				{
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_UINT);
				}
				else if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_INT);
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
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_FLOAT);
				}
				break;

			case STATE_EDIGITS:
				if (!isdigit(c))
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_FLOAT);
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
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::PERIOD);
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
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::IDENTIFIER);
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
					stream.Unget(2);
					state = STATE_DONE;
					token.SetTokenType(Token::IDENTIFIER);
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
					stream.Unget(2);
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_FLOAT);
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
					stream.Unget(3);
					state = STATE_DONE;
					token.SetTokenType(Token::VAL_FLOAT);
				}
				break;

			case STATE_IDENTIFIER:
				if (!IsIdentifierChar(c))
				{
					// The last character is not part of the token; roll it back.
					stream.Unget();
					state = STATE_DONE;
					token.SetTokenType(Token::IDENTIFIER);
				}
				break;

			case STATE_DONE:
				break;
		}
	}

	token.SetEndPos(stream.GetStreamPos());

	if ((state == STATE_SPACE) || (state == STATE_LINE_COMMENT))
	{
		token.SetTokenType(Token::END);
	}
	else if ((state == STATE_C_COMMENT) || (state == STATE_C_COMMENT_STAR))
	{
		token.SetErrorType(Token::UNTERMINATED_COMMENT);
		token.SetErrorPos(token.GetStartPos());
	}

	/*
	else
	{
		tokenString = mStringAllocator.Alloc(stream.GetBuffer() + startPos.index, endPos.index - startPos.index);

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
	*/
}


void Lexer::ExtractToken(CharStream &stream, StringAllocator &allocator, Token &token) const
{
	const int startIndex = token.GetStartPos().index;
	const int length = token.GetEndPos().index - startIndex;
	const char *tokenString = allocator.Alloc(stream.GetBuffer() + startIndex, length);
	token.SetText(tokenString);
}


void Lexer::EvaluateToken(StringAllocator &allocator, Token &token) const
{
	Value value;

	switch (token.GetTokenType())
	{
		case Token::IDENTIFIER:
		{
			// We need to refine the type of identifiers to separate out the keywords.
			if (strcmp(token.GetText(), "bool") == 0)
			{
				token.SetTokenType(Token::KEY_BOOL);
			}
			else if (strcmp(token.GetText(), "char") == 0)
			{
				token.SetTokenType(Token::KEY_CHAR);
			}
			else if (strcmp(token.GetText(), "float") == 0)
			{
				token.SetTokenType(Token::KEY_FLOAT);
			}
			else if (strcmp(token.GetText(), "int") == 0)
			{
				token.SetTokenType(Token::KEY_INT);
			}
			else if (strcmp(token.GetText(), "uint") == 0)
			{
				token.SetTokenType(Token::KEY_UINT);
			}
			else if (strcmp(token.GetText(), "if") == 0)
			{
				token.SetTokenType(Token::KEY_IF);
			}
			else if (strcmp(token.GetText(), "else") == 0)
			{
				token.SetTokenType(Token::KEY_ELSE);
			}
			else if (strcmp(token.GetText(), "while") == 0)
			{
				token.SetTokenType(Token::KEY_WHILE);
			}
			else if (strcmp(token.GetText(), "false") == 0)
			{
				token.SetTokenType(Token::VAL_BOOL);
				value.mBool = false;
			}
			else if (strcmp(token.GetText(), "true") == 0)
			{
				token.SetTokenType(Token::VAL_BOOL);
				value.mBool = true;
			}
		}
		break;
		case Token::VAL_INT:
		case Token::VAL_UINT:
		{
			int_t sign = 1;
			uint_t uval;
			const char *text = token.GetText();
			if (text[0] == '-')
			{
				sign = -1;
				++text;
			}
			else if (text[0] == '+')
			{
				++text;
			}
			if (token.HasAnnotation(Token::OCTAL))
			{
				sscanf(token.GetText(), BOND_UOCTAL_SCAN_FORMAT, &uval);
			}
			else if (token.HasAnnotation(Token::HEX))
			{
				sscanf(token.GetText(), BOND_UHEX_SCAN_FORMAT, &uval);
			}
			else
			{
				sscanf(token.GetText(), BOND_UDECIMAL_SCAN_FORMAT, &uval);
			}
			if (token.GetTokenType() == Token::VAL_INT)
			{
				value.mInt = static_cast<int_t>(sign * uval);
			}
			else
			{
				value.mUInt = static_cast<uint_t>(sign * uval);
			}
		}
		break;
		case Token::VAL_FLOAT:
		{
			sscanf(token.GetText(), BOND_FLOAT_SCAN_FORMAT, &value.mFloat);
		}
		break;
		default:
		{
			// Fall through.
		}
		break;
	}
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
