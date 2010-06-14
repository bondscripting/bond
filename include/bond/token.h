#ifndef BOND_TOKEN_H
#define BOND_TOKEN_H

namespace Bond
{

class Token
{
public:
	enum TypeId
	{
		// Keywords
		BOOL,
		FLOAT,
		FALSE,
		TRUE,
		IF,
		ELSE,
		WHILE,

		// Operators
		PLUS,      // '+'
		MINUS,     // '-'
		MULT,      // '*'
		DIV,       // '/'
		LT,        // '<'
		LTE,       // '<='
		GT,        // '>'
		GTE,       // '>='
		EQUAL,     // '=='
		NOT_EQUAL, // '!='
		NOT,       // '!'
		OR,        // '||'
		AND,       // '&&'

		// Punctuation
		OPAREN,    // '('
		CPAREN,    // ')'
		OBRACE,    // '{'
		CBRACE,    // '}'
		PERIOD,    // '.'
		SEMICOLON, // ';'

		// Stuff
		ASSIGN,    // '='
		//REMOTE,    // '@'
		IDENTIFIER,
		NUMBER,

		// Special values
		INVALID,
		EOF
	};

	Token(TypeId type, const char *text, /*float value,*/ int line, int column, int index):
		mTypeId(type),
		mText(text),
		//mValue(value),
		mLine(line),
		mColumn(column),
		mIndex(index)
	{
	}

	TypeId GetTypeId() const { return mTypeId; }
	const char *GetText() const { return mText; }
	//float GetValue() const { return mValue; }
	int GetLine() const { return mLine; }
	int GetColumn() const { return mColumn; }
	int GetIndex() const { return mIndex; }

private:
	TypeId mTypeId;
	const char *mText;
	//float mValue;
	int mLine;
	int mColumn;
	int mIndex;
};

}

#endif
