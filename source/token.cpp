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


bool TokenTypeSet::Contains(Token::TokenType type) const
{
	for (int i = 0; i < mNumTypes; ++i)
	{
		if (mTypes[i] == type)
		{
			return true;
		}
	}
	return false;
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

const TokenTypeSet TokenTypeSet::PRIMITIVE_TYPE_SPECIFIERS(
	PRIMITIVE_TYPE_SPECIFIER_TYPES,
	sizeof(PRIMITIVE_TYPE_SPECIFIER_TYPES) / sizeof(*PRIMITIVE_TYPE_SPECIFIER_TYPES),
	"PRIMITIVE_TYPE_SPECIFIER");


static const Token::TokenType BOOLEAN_TYPE_SPECIFIER_TYPES[] =
{
	Token::KEY_BOOL,
};

const TokenTypeSet TokenTypeSet::BOOLEAN_TYPE_SPECIFIERS(
	BOOLEAN_TYPE_SPECIFIER_TYPES,
	sizeof(BOOLEAN_TYPE_SPECIFIER_TYPES) / sizeof(*BOOLEAN_TYPE_SPECIFIER_TYPES),
	"BOOLEAN_TYPE_SPECIFIER");


static const Token::TokenType INTEGER_TYPE_SPECIFIER_TYPES[] =
{
	Token::KEY_CHAR,
	Token::KEY_INT,
	Token::KEY_UINT,
};

const TokenTypeSet TokenTypeSet::INTEGER_TYPE_SPECIFIERS(
	INTEGER_TYPE_SPECIFIER_TYPES,
	sizeof(INTEGER_TYPE_SPECIFIER_TYPES) / sizeof(*INTEGER_TYPE_SPECIFIER_TYPES),
	"INTEGER_TYPE_SPECIFIER");


static const Token::TokenType NUMERIC_TYPE_SPECIFIER_TYPES[] =
{
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT,
};

const TokenTypeSet TokenTypeSet::NUMERIC_TYPE_SPECIFIERS(
	NUMERIC_TYPE_SPECIFIER_TYPES,
	sizeof(NUMERIC_TYPE_SPECIFIER_TYPES) / sizeof(*NUMERIC_TYPE_SPECIFIER_TYPES),
	"NUMERIC_TYPE_SPECIFIER");


static const Token::TokenType TYPE_DESCRIPTOR_TYPES[] =
{
	Token::OP_MULT,
	Token::OBRACKET,
};

const TokenTypeSet TokenTypeSet::TYPE_DESCRIPTORS(
	TYPE_DESCRIPTOR_TYPES,
	sizeof(TYPE_DESCRIPTOR_TYPES) / sizeof(*TYPE_DESCRIPTOR_TYPES),
	"TYPE_DESCRIPTOR");


static const Token::TokenType JUMP_OPERATOR_TYPES[] =
{
	Token::KEY_BREAK,
	Token::KEY_CONTINUE,
	Token::KEY_RETURN,
};

const TokenTypeSet TokenTypeSet::JUMP_OPERATORS(
	JUMP_OPERATOR_TYPES,
	sizeof(JUMP_OPERATOR_TYPES) / sizeof(*JUMP_OPERATOR_TYPES),
	"JUMP_OPERATOR");


static const Token::TokenType SWITCH_LABEL_TYPES[] =
{
	Token::KEY_CASE,
	Token::KEY_DEFAULT,
};

const TokenTypeSet TokenTypeSet::SWITCH_LABELS(
	SWITCH_LABEL_TYPES,
	sizeof(SWITCH_LABEL_TYPES) / sizeof(*SWITCH_LABEL_TYPES),
	"SWITCH_LABEL");


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

const TokenTypeSet TokenTypeSet::ASSIGNMENT_OPERATORS(
	ASSIGNMENT_OPERATOR_TYPES,
	sizeof(ASSIGNMENT_OPERATOR_TYPES) / sizeof(*ASSIGNMENT_OPERATOR_TYPES),
	"ASSIGNMENT_OPERATOR");


static const Token::TokenType EQUALITY_OPERATOR_TYPES[] =
{
	Token::OP_EQUAL,
	Token::OP_NOT_EQUAL,
};

const TokenTypeSet TokenTypeSet::EQUALITY_OPERATORS(
	EQUALITY_OPERATOR_TYPES,
	sizeof(EQUALITY_OPERATOR_TYPES) / sizeof(*EQUALITY_OPERATOR_TYPES),
	"EQUALITY_OPERATOR");


static const Token::TokenType RELATIONAL_OPERATOR_TYPES[] =
{
	Token::OP_LT,
	Token::OP_LTE,
	Token::OP_GT,
	Token::OP_GTE,
};

const TokenTypeSet TokenTypeSet::RELATIONAL_OPERATORS(
	RELATIONAL_OPERATOR_TYPES,
	sizeof(RELATIONAL_OPERATOR_TYPES) / sizeof(*RELATIONAL_OPERATOR_TYPES),
	"RELATIONAL_OPERATOR");


static const Token::TokenType SHIFT_OPERATOR_TYPES[] =
{
	Token::OP_LEFT,
	Token::OP_RIGHT,
};

const TokenTypeSet TokenTypeSet::SHIFT_OPERATORS(
	SHIFT_OPERATOR_TYPES,
	sizeof(SHIFT_OPERATOR_TYPES) / sizeof(*SHIFT_OPERATOR_TYPES),
	"SHIFT_OPERATOR");


static const Token::TokenType ADDITIVE_OPERATOR_TYPES[] =
{
	Token::OP_PLUS,
	Token::OP_MINUS,
};

const TokenTypeSet TokenTypeSet::ADDITIVE_OPERATORS(
	ADDITIVE_OPERATOR_TYPES,
	sizeof(ADDITIVE_OPERATOR_TYPES) / sizeof(*ADDITIVE_OPERATOR_TYPES),
	"ADDITIVE_OPERATOR");


static const Token::TokenType MULTIPLICATIVE_OPERATOR_TYPES[] =
{
	Token::OP_MULT,
	Token::OP_DIV,
	Token::OP_MOD,
};

const TokenTypeSet TokenTypeSet::MULTIPLICATIVE_OPERATORS(
	MULTIPLICATIVE_OPERATOR_TYPES,
	sizeof(MULTIPLICATIVE_OPERATOR_TYPES) / sizeof(*MULTIPLICATIVE_OPERATOR_TYPES),
	"MULTIPLICATIVE_OPERATOR");


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

const TokenTypeSet TokenTypeSet::UNARY_OPERATORS(
	UNARY_OPERATOR_TYPES,
	sizeof(UNARY_OPERATOR_TYPES) / sizeof(*UNARY_OPERATOR_TYPES),
	"UNARY_OPERATOR");


static const Token::TokenType POSTFIX_OPERATOR_TYPES[] =
{
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_ARROW,
	Token::OPAREN,
	Token::OBRACKET,
	Token::PERIOD,
};

const TokenTypeSet TokenTypeSet::POSTFIX_OPERATORS(
	POSTFIX_OPERATOR_TYPES,
	sizeof(POSTFIX_OPERATOR_TYPES) / sizeof(*POSTFIX_OPERATOR_TYPES),
	"POSTFIX_OPERATOR");


static const Token::TokenType CONSTANT_VALUE_TYPES[] =
{
	Token::CONST_BOOL,
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT,
	Token::CONST_FLOAT,
	Token::CONST_STRING,
};

const TokenTypeSet TokenTypeSet::CONSTANT_VALUES(
	CONSTANT_VALUE_TYPES,
	sizeof(CONSTANT_VALUE_TYPES) / sizeof(*CONSTANT_VALUE_TYPES),
	"CONSTANT_VALUE");


static const Token::TokenType BLOCK_DELIMITER_TYPES[] =
{
	Token::CBRACE,
	Token::END,
};

const TokenTypeSet TokenTypeSet::BLOCK_DELIMITERS(
	BLOCK_DELIMITER_TYPES,
	sizeof(BLOCK_DELIMITER_TYPES) / sizeof(*BLOCK_DELIMITER_TYPES),
	"BLOCK_DELIMITER");


static const Token::TokenType DECLARATION_DELIMITER_TYPES[] =
{
	Token::KEY_ENUM,
	Token::KEY_NAMESPACE,
	Token::KEY_STRUCT,
	Token::CBRACE,
	Token::SEMICOLON,
};

const TokenTypeSet TokenTypeSet::DECLARATION_DELIMITERS(
	DECLARATION_DELIMITER_TYPES,
	sizeof(DECLARATION_DELIMITER_TYPES) / sizeof(*DECLARATION_DELIMITER_TYPES),
	"DECLARATION_DELIMITER");


static const Token::TokenType ENUM_DELIMITER_TYPES[] =
{
	Token::CBRACE,
	Token::SEMICOLON,
	Token::END,
};

const TokenTypeSet TokenTypeSet::ENUM_DELIMITERS(
	ENUM_DELIMITER_TYPES,
	sizeof(ENUM_DELIMITER_TYPES) / sizeof(*ENUM_DELIMITER_TYPES),
	"ENUM_DELIMITER");


static const Token::TokenType ENUMERATOR_DELIMITER_TYPES[] =
{
	Token::CBRACE,
	Token::SEMICOLON,
	Token::COMMA,
};

const TokenTypeSet TokenTypeSet::ENUMERATOR_DELIMITERS(
	ENUMERATOR_DELIMITER_TYPES,
	sizeof(ENUMERATOR_DELIMITER_TYPES) / sizeof(*ENUMERATOR_DELIMITER_TYPES),
	"ENUMERATOR_DELIMITER");


static const Token::TokenType STRUCT_MEMBER_DELIMITER_TYPES[] =
{
	Token::CBRACE,
	Token::SEMICOLON,
};

const TokenTypeSet TokenTypeSet::STRUCT_MEMBER_DELIMITERS(
	STRUCT_MEMBER_DELIMITER_TYPES,
	sizeof(STRUCT_MEMBER_DELIMITER_TYPES) / sizeof(*STRUCT_MEMBER_DELIMITER_TYPES),
	"STRUCT_MEMBER_DELIMITER");


static const Token::TokenType INITIALIZER_DELIMITER_TYPES[] =
{
	Token::OBRACE,
	Token::CBRACE,
	Token::COMMA,
};

const TokenTypeSet TokenTypeSet::INITIALIZER_DELIMITERS(
	INITIALIZER_DELIMITER_TYPES,
	sizeof(INITIALIZER_DELIMITER_TYPES) / sizeof(*INITIALIZER_DELIMITER_TYPES),
	"INITIALIZER_DELIMITER");


static const Token::TokenType SWITCH_SECTION_DELIMITER_TYPES[] =
{
	Token::KEY_CASE,
	Token::KEY_DEFAULT,
	Token::CBRACE,
	Token::END,
};

const TokenTypeSet TokenTypeSet::SWITCH_SECTION_DELIMITERS(
	SWITCH_SECTION_DELIMITER_TYPES,
	sizeof(SWITCH_SECTION_DELIMITER_TYPES) / sizeof(*SWITCH_SECTION_DELIMITER_TYPES),
	"SWITCH_SECTION_DELIMITER");


static const Token::TokenType STATEMENT_DELIMITER_TYPES[] =
{
	Token::KEY_BREAK,
	Token::KEY_CASE,
	Token::KEY_CONTINUE,
	Token::KEY_DEFAULT,
	Token::KEY_DO,
	Token::KEY_IF,
	Token::KEY_FOR,
	Token::KEY_RETURN,
	Token::KEY_SWITCH,
	Token::KEY_WHILE,
	Token::OBRACE,
	Token::CBRACE,
	Token::SEMICOLON,
};

const TokenTypeSet TokenTypeSet::STATEMENT_DELIMITERS(
	STATEMENT_DELIMITER_TYPES,
	sizeof(STATEMENT_DELIMITER_TYPES) / sizeof(*STATEMENT_DELIMITER_TYPES),
	"STATEMENT_DELIMITER");


static const Token::TokenType LABEL_DELIMITER_TYPES[] =
{
	Token::KEY_BREAK,
	Token::KEY_CASE,
	Token::KEY_CONTINUE,
	Token::KEY_DEFAULT,
	Token::KEY_DO,
	Token::KEY_IF,
	Token::KEY_FOR,
	Token::KEY_RETURN,
	Token::KEY_SWITCH,
	Token::KEY_WHILE,
	Token::OBRACE,
	Token::CBRACE,
	Token::SEMICOLON,
	Token::COLON,
};

const TokenTypeSet TokenTypeSet::LABEL_DELIMITERS(
	LABEL_DELIMITER_TYPES,
	sizeof(LABEL_DELIMITER_TYPES) / sizeof(*LABEL_DELIMITER_TYPES),
	"LABEL_DELIMITER");

}
