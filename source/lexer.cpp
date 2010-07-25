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
	StringAllocator allocator(mStringBuffer, resources.stringBufferLength);

	mTokens = new Token[resources.numTokens];
	mNumTokens = resources.numTokens;

	stream.Reset();
	GenerateTokens(stream, allocator);
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
					token.SetTokenType(Token::COLON);
					state = STATE_DONE;
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
				else if (c == '.')
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
					stream.Unget();
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
				else if (c == '.')
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
					stream.Unget();
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
					stream.Unget();
					token.SetTokenType(Token::OP_MULT);
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
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
					stream.Unget();
					token.SetTokenType(Token::OP_BIT_AND);
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
					stream.Unget();
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
					stream.Unget();
					token.SetTokenType(Token::OP_BIT_XOR);
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
					token.SetTokenType(Token::VAL_UINT);
					state = STATE_DONE;
				}
				else if (c == 'x')
				{
					state = STATE_HEX;
				}
				else if (IsOctalChar(c))
				{
					state = STATE_OCTAL;
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_INT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_OCTAL:
				if (c == 'u')
				{
					token.SetTokenType(Token::VAL_UINT);
					token.AddAnnotation(Token::OCTAL);
					state = STATE_DONE;
				}
				else if (IsOctalChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_OCTAL_INT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_INT);
					token.AddAnnotation(Token::OCTAL);
					state = STATE_DONE;
				}
				break;

			case STATE_HEX:
				if (c == 'u')
				{
					token.SetTokenType(Token::VAL_UINT);
					token.AddAnnotation(Token::HEX);
					state = STATE_DONE;
				}
				else if (IsHexChar(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_HEX_INT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_INT);
					token.AddAnnotation(Token::HEX);
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
				else if (c == 'u')
				{
					token.SetTokenType(Token::VAL_UINT);
					state = STATE_DONE;
				}
				else if (isdigit(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_INT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_INT);
					state = STATE_DONE;
				}
				break;

			case STATE_FDIGITS:
				if ((c == 'e') || (c == 'E'))
				{
					state = STATE_EXPONENT;
				}
				else if (isdigit(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_FLOAT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_FLOAT);
					state = STATE_DONE;
				}
				break;

			case STATE_EDIGITS:
				if (isdigit(c))
				{
					// Remain in this state.
				}
				else if (IsBadNumberChar(c))
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_FLOAT);
					token.SetErrorPos(pos);
					state = STATE_BAD_NUMBER;
				}
				else
				{
					stream.Unget();
					token.SetTokenType(Token::VAL_FLOAT);
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
					stream.Unget();
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
				else
				{
					// The last character and the previous 'e' are not part of the token.
					stream.Unget(2);
					token.SetTokenType(Token::VAL_FLOAT);
					state = STATE_DONE;
				}
				break;

			case STATE_EXPONENT_SIGN:
				if (isdigit(c))
				{
					state = STATE_EDIGITS;
				}
				else
				{
					// The last character, the previous '+' or '-' and the previous 'e' are not part of the token.
					stream.Unget(3);
					token.SetTokenType(Token::VAL_FLOAT);
					state = STATE_DONE;
				}
				break;

			case STATE_BAD_NUMBER:
				if (!IsBadNumberChar(c))
				{
					stream.Unget();
					state = STATE_DONE;
				}
				break;

			case STATE_IDENTIFIER:
				if (!IsIdentifierChar(c))
				{
					stream.Unget();
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
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::EMPTY_CHARACTER_CONSTANT);
					token.SetErrorPos(token.GetStartPos());
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
				else
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_ESCAPE);
					token.SetErrorPos(pos);
					state = STATE_BAD_CHAR;
				}
				break;

			case STATE_CHAR_END:
				if (c == '\'')
				{
					token.SetTokenType(Token::VAL_CHAR);
					state = STATE_DONE;
				}
				else
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::MULTI_CHARACTER_CONSTANT);
					token.SetErrorPos(token.GetStartPos());
					state = (c == '\\') ? STATE_BAD_CHAR_ESCAPE : STATE_BAD_CHAR;
				}
				break;

			case STATE_BAD_CHAR:
				if (c == '\\')
				{
					state = STATE_BAD_CHAR_ESCAPE;
				}
				else if (c == '\'')
				{
					state = STATE_DONE;
				}
				break;

			case STATE_BAD_CHAR_ESCAPE:
				state = STATE_BAD_CHAR;
				break;

			case STATE_STRING:
				if (c == '\\')
				{
					state = STATE_STRING_ESCAPE;
				}
				else if (c == '\"')
				{
					token.SetTokenType(Token::VAL_STRING);
					state = STATE_DONE;
				}
				break;

			case STATE_STRING_ESCAPE:
				if (IsEscapeChar(c))
				{
					state = STATE_STRING;
				}
				else
				{
					token.SetTokenType(Token::INVALID);
					token.SetErrorType(Token::INVALID_ESCAPE);
					token.SetErrorPos(pos);
					state = STATE_BAD_STRING;
				}
				break;

			case STATE_BAD_STRING:
				if (c == '\\')
				{
					state = STATE_BAD_STRING_ESCAPE;
				}
				else if (c == '\"')
				{
					state = STATE_DONE;
				}
				break;

			case STATE_BAD_STRING_ESCAPE:
				state = STATE_BAD_STRING;
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

	token.SetEndPos(stream.GetStreamPos());

	if ((state == STATE_SPACE) || (state == STATE_LINE_COMMENT))
	{
		token.SetTokenType(Token::END);
		token.SetEndPos(token.GetStartPos());
	}
	else if ((state == STATE_CHAR) || (state == STATE_CHAR_ESCAPE) || (state == STATE_CHAR_END) ||
	         (state == STATE_BAD_CHAR) || (state == STATE_BAD_CHAR_ESCAPE))
	{
		token.SetTokenType(Token::INVALID);
		token.SetErrorType(Token::UNTERMINATED_CHARACTER);
		token.SetErrorPos(token.GetStartPos());
	}
	else if ((state == STATE_STRING) || (state == STATE_STRING_ESCAPE) ||
	         (state == STATE_BAD_STRING) || (state == STATE_BAD_STRING_ESCAPE))
	{
		token.SetTokenType(Token::INVALID);
		token.SetErrorType(Token::UNTERMINATED_STRING);
		token.SetErrorPos(token.GetStartPos());
	}
	else if ((state == STATE_C_COMMENT) || (state == STATE_C_COMMENT_STAR))
	{
		token.SetTokenType(Token::INVALID);
		token.SetErrorType(Token::UNTERMINATED_COMMENT);
		token.SetErrorPos(token.GetStartPos());
	}
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
	switch (token.GetTokenType())
	{
		case Token::IDENTIFIER:
			EvaluateKeywordToken(token);
			break;

		case Token::VAL_CHAR:
			EvaluateCharToken(token);
			break;

		case Token::VAL_INT:
		case Token::VAL_UINT:
			EvaluateIntegerToken(token);
			break;

		case Token::VAL_FLOAT:
			EvaluateFloatToken(token);
			break;

		case Token::VAL_STRING:
			EvaluateStringToken(allocator, token);
			break;

		default:
			// Fall through.
			break;
	}
}


void Lexer::EvaluateKeywordToken(Token &token) const
{
	if (strcmp(token.GetText(), "bool") == 0)
	{
		token.SetTokenType(Token::KEY_BOOL);
	}
	else if (strcmp(token.GetText(), "break") == 0)
	{
		token.SetTokenType(Token::KEY_BREAK);
	}
	else if (strcmp(token.GetText(), "char") == 0)
	{
		token.SetTokenType(Token::KEY_CHAR);
	}
	else if (strcmp(token.GetText(), "case") == 0)
	{
		token.SetTokenType(Token::KEY_CASE);
	}
	else if (strcmp(token.GetText(), "const") == 0)
	{
		token.SetTokenType(Token::KEY_CONST);
	}
	else if (strcmp(token.GetText(), "continue") == 0)
	{
		token.SetTokenType(Token::KEY_CONTINUE);
	}
	else if (strcmp(token.GetText(), "default") == 0)
	{
		token.SetTokenType(Token::KEY_DEFAULT);
	}
	else if (strcmp(token.GetText(), "do") == 0)
	{
		token.SetTokenType(Token::KEY_DO);
	}
	else if (strcmp(token.GetText(), "else") == 0)
	{
		token.SetTokenType(Token::KEY_ELSE);
	}
	else if (strcmp(token.GetText(), "enum") == 0)
	{
		token.SetTokenType(Token::KEY_ENUM);
	}
	else if (strcmp(token.GetText(), "float") == 0)
	{
		token.SetTokenType(Token::KEY_FLOAT);
	}
	else if (strcmp(token.GetText(), "for") == 0)
	{
		token.SetTokenType(Token::KEY_FOR);
	}
	else if (strcmp(token.GetText(), "if") == 0)
	{
		token.SetTokenType(Token::KEY_IF);
	}
	else if (strcmp(token.GetText(), "int") == 0)
	{
		token.SetTokenType(Token::KEY_INT);
	}
	else if (strcmp(token.GetText(), "return") == 0)
	{
		token.SetTokenType(Token::KEY_RETURN);
	}
	else if (strcmp(token.GetText(), "sizeof") == 0)
	{
		token.SetTokenType(Token::KEY_SIZEOF);
	}
	else if (strcmp(token.GetText(), "switch") == 0)
	{
		token.SetTokenType(Token::KEY_SWITCH);
	}
	else if (strcmp(token.GetText(), "uint") == 0)
	{
		token.SetTokenType(Token::KEY_UINT);
	}
	else if (strcmp(token.GetText(), "void") == 0)
	{
		token.SetTokenType(Token::KEY_VOID);
	}
	else if (strcmp(token.GetText(), "while") == 0)
	{
		token.SetTokenType(Token::KEY_WHILE);
	}
	else if (strcmp(token.GetText(), "false") == 0)
	{
		token.SetTokenType(Token::VAL_BOOL);
		token.SetBoolValue(false);
	}
	else if (strcmp(token.GetText(), "true") == 0)
	{
		token.SetTokenType(Token::VAL_BOOL);
		token.SetBoolValue(true);
	}
}


void Lexer::EvaluateCharToken(Token &token) const
{
	const char value = EvaluateChar(token.GetText() + 1).value;
	token.SetCharValue(value);
}


void Lexer::EvaluateFloatToken(Token &token) const
{
	float_t value;
	sscanf(token.GetText(), BOND_FLOAT_SCAN_FORMAT, &value);
	token.SetFloatValue(value);
}


void Lexer::EvaluateIntegerToken(Token &token) const
{
	int_t sign = 1;
	int_t value;
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
		sscanf(text, BOND_UOCTAL_SCAN_FORMAT, &value);
	}
	else if (token.HasAnnotation(Token::HEX))
	{
		sscanf(text, BOND_UHEX_SCAN_FORMAT, &value);
	}
	else
	{
		sscanf(text, BOND_UDECIMAL_SCAN_FORMAT, &value);
	}

	if (token.GetTokenType() == Token::VAL_INT)
	{
		token.SetIntValue(sign * value);
	}
	else
	{
		token.SetUIntValue(static_cast<uint_t>(sign * value));
	}
}


void Lexer::EvaluateStringToken(StringAllocator &allocator, Token &token) const
{
	// Allocate enough space for the string with the quotes stripped off. The string
	// could actually be shorter once escape sequences are expanded.
	const int allocLength = token.GetEndPos().index - token.GetStartPos().index - 2;
	char *buffer = allocator.Alloc(allocLength);
	char *dest = buffer;
	const char *source = token.GetText() + 1;
	const char *end = source + allocLength;
	int usedLength = 0;

	while (source < end)
	{
		CharResult result = EvaluateChar(source);
		source = result.end;
		*dest = result.value;
		++dest;
		++usedLength;
	}

	*dest = '\0';
	token.SetStringValue(buffer, usedLength);
}


Lexer::CharResult Lexer::EvaluateChar(const char *text) const
{
	CharResult result;
	if (text[0] == '\\')
	{
		switch (text[1])
		{
			case '0': result.value = '\0'; break;
			case 'a': result.value = '\a'; break;
			case 'b': result.value = '\b'; break;
			case 'e': result.value = '\e'; break;
			case 'f': result.value = '\f'; break;
			case 'n': result.value = '\n'; break;
			case 'r': result.value = '\r'; break;
			case 't': result.value = '\t'; break;
			case 'v': result.value = '\v'; break;
			case '\'': result.value = '\''; break;
			case '\"': result.value = '\"'; break;
			case '\\': result.value = '\\'; break;
			default: result.value = text[1]; break;
		}
		result.end = text + 2;
	}
	else
	{
		result.end = text + 1;
		result.value = text[0];
	}

	return result;
}


bool Lexer::IsIdentifierChar(char c) const
{
	return  isalnum(c) || (c == '_');
}


bool Lexer::IsOctalChar(char c) const
{
	return (c >= '0') && (c <= '7');
}


bool Lexer::IsHexChar(char c) const
{
	return isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}


bool Lexer::IsBadNumberChar(char c) const
{
	return IsIdentifierChar(c) || (c == '.');
}


bool Lexer::IsEscapeChar(char c) const
{
	// TODO: Ensure exhaustive list.
	return (c == '0') || (c == 'a') || (c == 'b') || (c == 'e') || (c == 'f') || (c == 'n') ||
		(c == 'r') || (c == 't') || (c == 'v') || (c == '\'') || (c == '\"') || (c == '\\');
}

}
