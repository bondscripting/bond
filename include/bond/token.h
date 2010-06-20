#ifndef BOND_TOKEN_H
#define BOND_TOKEN_H

#include "bond/private/value.h"

namespace Bond
{

class Token
{
public:
	enum TypeId
	{
		// Keywords
		TYPEID_KEY_BOOL,
		TYPEID_KEY_FLOAT,
		TYPEID_KEY_IF,
		TYPEID_KEY_ELSE,
		TYPEID_KEY_WHILE,

		// Operators
		TYPEID_OP_PLUS,      // '+'
		TYPEID_OP_MINUS,     // '-'
		TYPEID_OP_MULT,      // '*'
		TYPEID_OP_DIV,       // '/'
		TYPEID_OP_LT,        // '<'
		TYPEID_OP_LTE,       // '<='
		TYPEID_OP_GT,        // '>'
		TYPEID_OP_GTE,       // '>='
		TYPEID_OP_EQUAL,     // '=='
		TYPEID_OP_NOT_EQUAL, // '!='
		TYPEID_OP_NOT,       // '!'
		TYPEID_OP_OR,        // '||'
		TYPEID_OP_AND,       // '&&'

		// Punctuation
		TYPEID_OPAREN,    // '('
		TYPEID_CPAREN,    // ')'
		TYPEID_OBRACE,    // '{'
		TYPEID_CBRACE,    // '}'
		TYPEID_PERIOD,    // '.'
		TYPEID_SEMICOLON, // ';'

		// Literal values.
		TYPEID_VAL_BOOL,
		TYPEID_VAL_INT,
		TYPEID_VAL_UINT,
		TYPEID_VAL_FLOAT,

		// Stuff
		TYPEID_ASSIGN,    // '='
		TYPEID_IDENTIFIER,

		// Special values
		TYPEID_INVALID,
		TYPEID_EOF
	};

	Token(Value value, TypeId type, const char *text, int line, int column, int index):
		mValue(value),
		mTypeId(type),
		mText(text),
		mLine(line),
		mColumn(column),
		mIndex(index)
	{
	}

	TypeId GetTypeId() const { return mTypeId; }
	const char *GetText() const { return mText; }
	float_t GetFloatValue() const { return mValue.mFloat; }
	int_t GetIntValue() const { return mValue.mInt; }
	uint_t GetUIntValue() const { return mValue.mUInt; }
	int GetLine() const { return mLine; }
	int GetColumn() const { return mColumn; }
	int GetIndex() const { return mIndex; }

private:
	Value mValue;
	TypeId mTypeId;
	const char *mText;
	int mLine;
	int mColumn;
	int mIndex;
};

}

#endif
