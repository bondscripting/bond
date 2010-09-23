#include "bond/token.h"

namespace Bond
{

Token &Token::operator=(const Token &other)
{
	mStartPos = other.mStartPos;
	mEndPos = other.mEndPos;
	mErrorPos = other.mErrorPos;
	mValue = other.mValue;
	mText = other.mText;
	mTokenType = other.mTokenType;
	mErrorType = other.mErrorType;
	mAnnotations = other.mAnnotations;
	return *this;
}


void Token::SetStringValue(const char *buffer, int length)
{
	mValue.mString.buffer = buffer;
	mValue.mString.length = length;
}


const char *Token::GetTokenName() const
{
	return GetTokenName(mTokenType);
}


const char *Token::GetTokenName(TokenType type)
{
	static const char *const TOKEN_NAMES[] =
	{
#define BOND_TOKEN_ITEM(item) #item,
		BOND_TOKEN_LIST
#undef BOND_TOKEN_ITEM
	};

	return TOKEN_NAMES[type];
}


const char *Token::GetErrorName() const
{
	return GetErrorName(mErrorType);
}


const char *Token::GetErrorName(ErrorType type)
{
	static const char *const ERROR_NAMES[] =
	{
#define BOND_TOKEN_ERROR_ITEM(item) #item,
		BOND_TOKEN_ERROR_LIST
#undef BOND_TOKEN_ERROR_ITEM
	};

	return ERROR_NAMES[type];
}


static const Token::TokenType PRIMITIVE_TYPE_SPECIFIER_TYPES[] =
{
	Token::KEY_BOOL,
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_VOID,
};

TokenTypeSet TokenTypeSet::PRIMITIVE_TYPE_SPECIFIERS =
{
	PRIMITIVE_TYPE_SPECIFIER_TYPES,
	sizeof(PRIMITIVE_TYPE_SPECIFIER_TYPES) / sizeof(*PRIMITIVE_TYPE_SPECIFIER_TYPES),
	"PRIMITIVE_TYPE_SPECIFIER",
};


static const Token::TokenType TYPE_DESCRIPTOR_TYPES[] =
{
	Token::OP_MULT,
	Token::OBRACKET,
};

TokenTypeSet TokenTypeSet::TYPE_DESCRIPTORS =
{
	TYPE_DESCRIPTOR_TYPES,
	sizeof(TYPE_DESCRIPTOR_TYPES) / sizeof(*TYPE_DESCRIPTOR_TYPES),
	"TYPE_DESCRIPTOR",
};


static const Token::TokenType JUMP_OPERATOR_TYPES[] =
{
	Token::KEY_BREAK,
	Token::KEY_CONTINUE,
	Token::KEY_RETURN,
};

TokenTypeSet TokenTypeSet::JUMP_OPERATORS =
{
	JUMP_OPERATOR_TYPES,
	sizeof(JUMP_OPERATOR_TYPES) / sizeof(*JUMP_OPERATOR_TYPES),
	"JUMP_OPERATOR",
};


static const Token::TokenType ASSIGNMENT_OPERATOR_TYPES[] =
{
	Token::ASSIGN,
	Token::ASSIGN_LEFT,
	Token::ASSIGN_RIGHT,
	Token::ASSIGN_PLUS,
	Token::ASSIGN_MINUS,
	Token::ASSIGN_MULT,
	Token::ASSIGN_DIV,
	Token::ASSIGN_MOD,
	Token::ASSIGN_AND,
	Token::ASSIGN_OR,
	Token::ASSIGN_XOR,
};

TokenTypeSet TokenTypeSet::ASSIGNMENT_OPERATORS =
{
	ASSIGNMENT_OPERATOR_TYPES,
	sizeof(ASSIGNMENT_OPERATOR_TYPES) / sizeof(*ASSIGNMENT_OPERATOR_TYPES),
	"ASSIGNMENT_OPERATOR",
};


static const Token::TokenType EQUALITY_OPERATOR_TYPES[] =
{
	Token::OP_EQUAL,
	Token::OP_NOT_EQUAL,
};

TokenTypeSet TokenTypeSet::EQUALITY_OPERATORS =
{
	EQUALITY_OPERATOR_TYPES,
	sizeof(EQUALITY_OPERATOR_TYPES) / sizeof(*EQUALITY_OPERATOR_TYPES),
	"EQUALITY_OPERATOR",
};


static const Token::TokenType RELATIONAL_OPERATOR_TYPES[] =
{
	Token::OP_LT,
	Token::OP_LTE,
	Token::OP_GT,
	Token::OP_GTE,
};

TokenTypeSet TokenTypeSet::RELATIONAL_OPERATORS =
{
	RELATIONAL_OPERATOR_TYPES,
	sizeof(RELATIONAL_OPERATOR_TYPES) / sizeof(*RELATIONAL_OPERATOR_TYPES),
	"RELATIONAL_OPERATOR",
};


static const Token::TokenType SHIFT_OPERATOR_TYPES[] =
{
	Token::OP_LEFT,
	Token::OP_RIGHT,
};

TokenTypeSet TokenTypeSet::SHIFT_OPERATORS =
{
	SHIFT_OPERATOR_TYPES,
	sizeof(SHIFT_OPERATOR_TYPES) / sizeof(*SHIFT_OPERATOR_TYPES),
	"SHIFT_OPERATOR",
};


static const Token::TokenType ADDITIVE_OPERATOR_TYPES[] =
{
	Token::OP_PLUS,
	Token::OP_MINUS,
};

TokenTypeSet TokenTypeSet::ADDITIVE_OPERATORS =
{
	ADDITIVE_OPERATOR_TYPES,
	sizeof(ADDITIVE_OPERATOR_TYPES) / sizeof(*ADDITIVE_OPERATOR_TYPES),
	"ADDITIVE_OPERATOR",
};


static const Token::TokenType MULTIPLICATIVE_OPERATOR_TYPES[] =
{
	Token::OP_MULT,
	Token::OP_DIV,
	Token::OP_MOD,
};

TokenTypeSet TokenTypeSet::MULTIPLICATIVE_OPERATORS =
{
	MULTIPLICATIVE_OPERATOR_TYPES,
	sizeof(MULTIPLICATIVE_OPERATOR_TYPES) / sizeof(*MULTIPLICATIVE_OPERATOR_TYPES),
	"MULTIPLICATIVE_OPERATOR",
};


static const Token::TokenType UNARY_OPERATOR_TYPES[] =
{
	Token::OP_PLUS,
	Token::OP_MINUS,
	Token::OP_MULT,
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_NOT,
	Token::OP_BIT_AND,
	Token::OP_BIT_NOT,
};

TokenTypeSet TokenTypeSet::UNARY_OPERATORS =
{
	UNARY_OPERATOR_TYPES,
	sizeof(UNARY_OPERATOR_TYPES) / sizeof(*UNARY_OPERATOR_TYPES),
	"UNARY_OPERATOR",
};


static const Token::TokenType POSTFIX_OPERATOR_TYPES[] =
{
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_ARROW,
	Token::OPAREN,
	Token::OBRACKET,
	Token::PERIOD,
};

TokenTypeSet TokenTypeSet::POSTFIX_OPERATORS =
{
	POSTFIX_OPERATOR_TYPES,
	sizeof(POSTFIX_OPERATOR_TYPES) / sizeof(*POSTFIX_OPERATOR_TYPES),
	"POSTFIX_OPERATOR",
};


static const Token::TokenType CONSTANT_VALUE_TYPES[] =
{
	Token::CONST_BOOL,
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT,
	Token::CONST_FLOAT,
	Token::CONST_STRING,
};

TokenTypeSet TokenTypeSet::CONSTANT_VALUES =
{
	CONSTANT_VALUE_TYPES,
	sizeof(CONSTANT_VALUE_TYPES) / sizeof(*CONSTANT_VALUE_TYPES),
	"CONSTANT_VALUE",
};

}
