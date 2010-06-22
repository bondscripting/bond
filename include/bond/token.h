#ifndef BOND_TOKEN_H
#define BOND_TOKEN_H

#include "bond/private/value.h"

#define BOND_TOKEN_LIST                    \
	/* Keywords */												   \
	BOND_TOKEN_ITEM(KEY_BOOL)                \
	BOND_TOKEN_ITEM(KEY_FLOAT)               \
	BOND_TOKEN_ITEM(KEY_IF)                  \
	BOND_TOKEN_ITEM(KEY_ELSE)                \
	BOND_TOKEN_ITEM(KEY_WHILE)               \
                                           \
	/* Operators */                          \
	BOND_TOKEN_ITEM(OP_PLUS)      /* '+' */  \
	BOND_TOKEN_ITEM(OP_MINUS)     /* '-' */  \
	BOND_TOKEN_ITEM(OP_MULT)      /* '*' */  \
	BOND_TOKEN_ITEM(OP_DIV)       /* '/' */  \
	BOND_TOKEN_ITEM(OP_LT)        /* '<' */  \
	BOND_TOKEN_ITEM(OP_LTE)       /* '<=' */ \
	BOND_TOKEN_ITEM(OP_GT)        /* '>' */  \
	BOND_TOKEN_ITEM(OP_GTE)       /* '>=' */ \
	BOND_TOKEN_ITEM(OP_EQUAL)     /* '==' */ \
	BOND_TOKEN_ITEM(OP_NOT_EQUAL) /* '!=' */ \
	BOND_TOKEN_ITEM(OP_NOT)       /* '!' */  \
	BOND_TOKEN_ITEM(OP_OR)        /* '||' */ \
	BOND_TOKEN_ITEM(OP_AND)       /* '&&' */ \
                                           \
	/* Punctuation */                        \
	BOND_TOKEN_ITEM(OPAREN)       /* '(' */  \
	BOND_TOKEN_ITEM(CPAREN)       /* ')' */  \
	BOND_TOKEN_ITEM(OBRACE)       /* '{' */  \
	BOND_TOKEN_ITEM(CBRACE)       /* '}' */  \
	BOND_TOKEN_ITEM(PERIOD)       /* '.' */  \
	BOND_TOKEN_ITEM(SEMICOLON)    /* ';' */  \
                                           \
	/* Literal values */                     \
	BOND_TOKEN_ITEM(VAL_BOOL)                \
	BOND_TOKEN_ITEM(VAL_INT)                 \
	BOND_TOKEN_ITEM(VAL_UINT)                \
	BOND_TOKEN_ITEM(VAL_FLOAT)               \
                                           \
	/* Stuff */                              \
	BOND_TOKEN_ITEM(ASSIGN)       /* '=' */  \
	BOND_TOKEN_ITEM(IDENTIFIER)              \
                                           \
	/* Special values */                     \
	BOND_TOKEN_ITEM(INVALID)                 \
	BOND_TOKEN_ITEM(END)                     \


namespace Bond
{

class Token
{
public:
	/*
	enum TokenType
	{
		// Keywords
		KEY_BOOL,
		KEY_FLOAT,
		KEY_IF,
		KEY_ELSE,
		KEY_WHILE,

		// Operators
		OP_PLUS,      // '+'
		OP_MINUS,     // '-'
		OP_MULT,      // '*'
		OP_DIV,       // '/'
		OP_LT,        // '<'
		OP_LTE,       // '<='
		OP_GT,        // '>'
		OP_GTE,       // '>='
		OP_EQUAL,     // '=='
		OP_NOT_EQUAL, // '!='
		OP_NOT,       // '!'
		OP_OR,        // '||'
		OP_AND,       // '&&'

		// Punctuation
		OPAREN,       // '('
		CPAREN,       // ')'
		OBRACE,       // '{'
		CBRACE,       // '}'
		PERIOD,       // '.'
		SEMICOLON,    // ';'

		// Literal values.
		VAL_BOOL,
		VAL_INT,
		VAL_UINT,
		VAL_FLOAT,

		// Stuff
		ASSIGN,       // '='
		IDENTIFIER,

		// Special values
		INVALID,
		END,
	};
	*/

	enum TokenType
	{
#define BOND_TOKEN_ITEM(item) item,
		BOND_TOKEN_LIST
#undef BOND_TOKEN_ITEM
	};

	Token(Value value, TokenType type, const char *text, int line, int column, int index):
		mValue(value),
		mTokenType(type),
		mText(text),
		mLine(line),
		mColumn(column),
		mIndex(index)
	{
	}

	TokenType GetTokenType() const { return mTokenType; }
	const char *GetTokenName() const;
	const char *GetText() const { return mText; }
	float_t GetFloatValue() const { return mValue.mFloat; }
	int_t GetIntValue() const { return mValue.mInt; }
	uint_t GetUIntValue() const { return mValue.mUInt; }
	int GetLine() const { return mLine; }
	int GetColumn() const { return mColumn; }
	int GetIndex() const { return mIndex; }

	static const char *GetTokenName(TokenType type);

private:
	Value mValue;
	TokenType mTokenType;
	const char *mText;
	int mLine;
	int mColumn;
	int mIndex;
};

}

#endif
