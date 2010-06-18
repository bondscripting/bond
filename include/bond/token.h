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
		TYPEID_BOOL,
		TYPEID_FLOAT,
		TYPEID_FALSE,
		TYPEID_TRUE,
		TYPEID_IF,
		TYPEID_ELSE,
		TYPEID_WHILE,

		// Operators
		TYPEID_PLUS,      // '+'
		TYPEID_MINUS,     // '-'
		TYPEID_MULT,      // '*'
		TYPEID_DIV,       // '/'
		TYPEID_LT,        // '<'
		TYPEID_LTE,       // '<='
		TYPEID_GT,        // '>'
		TYPEID_GTE,       // '>='
		TYPEID_EQUAL,     // '=='
		TYPEID_NOT_EQUAL, // '!='
		TYPEID_NOT,       // '!'
		TYPEID_OR,        // '||'
		TYPEID_AND,       // '&&'

		// Punctuation
		TYPEID_OPAREN,    // '('
		TYPEID_CPAREN,    // ')'
		TYPEID_OBRACE,    // '{'
		TYPEID_CBRACE,    // '}'
		TYPEID_PERIOD,    // '.'
		TYPEID_SEMICOLON, // ';'

		// Stuff
		TYPEID_ASSIGN,    // '='
		TYPEID_IDENTIFIER,
		TYPEID_NUMBER,

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
