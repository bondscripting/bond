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


TokenTypeSet::TokenTypeSet(
		const char *const typeName,
		Token::TokenType t0,
		Token::TokenType t1,
		Token::TokenType t2,
		Token::TokenType t3,
		Token::TokenType t4,
		Token::TokenType t5,
		Token::TokenType t6,
		Token::TokenType t7,
		Token::TokenType t8,
		Token::TokenType t9,
		Token::TokenType t10,
		Token::TokenType t11,
		Token::TokenType t12,
		Token::TokenType t13,
		Token::TokenType t14,
		Token::TokenType t15):
	mTypeName(typeName)
{
	Set(t0);
	Set(t1);
	Set(t2);
	Set(t3);
	Set(t4);
	Set(t5);
	Set(t6);
	Set(t7);
	Set(t8);
	Set(t9);
	Set(t10);
	Set(t11);
	Set(t12);
	Set(t13);
	Set(t14);
	Set(t15);
}


const TokenTypeSet TokenTypeSet::PRIMITIVE_TYPE_SPECIFIERS(
	"PRIMITIVE_TYPE_SPECIFIER",
	Token::KEY_BOOL,
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_VOID
);


const TokenTypeSet TokenTypeSet::BOOLEAN_TYPE_SPECIFIERS(
	"BOOLEAN_TYPE_SPECIFIER",
	Token::KEY_BOOL
);


const TokenTypeSet TokenTypeSet::INTEGER_TYPE_SPECIFIERS(
	"INTEGER_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_INT,
	Token::KEY_UINT
);


static const Token::TokenType NUMERIC_TYPE_SPECIFIER_TYPES[] =
{
};

const TokenTypeSet TokenTypeSet::NUMERIC_TYPE_SPECIFIERS(
	"NUMERIC_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT
);


const TokenTypeSet TokenTypeSet::TYPE_DESCRIPTORS(
	"TYPE_DESCRIPTOR",
	Token::OP_MULT,
	Token::OBRACKET
);


const TokenTypeSet TokenTypeSet::JUMP_OPERATORS(
	"JUMP_OPERATOR",
	Token::KEY_BREAK,
	Token::KEY_CONTINUE,
	Token::KEY_RETURN
);


const TokenTypeSet TokenTypeSet::SWITCH_LABELS(
	"SWITCH_LABEL",
	Token::KEY_CASE,
	Token::KEY_DEFAULT
);


const TokenTypeSet TokenTypeSet::ASSIGNMENT_OPERATORS(
	"ASSIGNMENT_OPERATOR",
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
	Token::ASSIGN_XOR
);


const TokenTypeSet TokenTypeSet::EQUALITY_OPERATORS(
	"EQUALITY_OPERATOR",
	Token::OP_EQUAL,
	Token::OP_NOT_EQUAL
);


const TokenTypeSet TokenTypeSet::RELATIONAL_OPERATORS(
	"RELATIONAL_OPERATOR",
	Token::OP_LT,
	Token::OP_LTE,
	Token::OP_GT,
	Token::OP_GTE
);


const TokenTypeSet TokenTypeSet::SHIFT_OPERATORS(
	"SHIFT_OPERATOR",
	Token::OP_LEFT,
	Token::OP_RIGHT
);


const TokenTypeSet TokenTypeSet::ADDITIVE_OPERATORS(
	"ADDITIVE_OPERATOR",
	Token::OP_PLUS,
	Token::OP_MINUS
);


const TokenTypeSet TokenTypeSet::MULTIPLICATIVE_OPERATORS(
	"MULTIPLICATIVE_OPERATOR",
	Token::OP_MULT,
	Token::OP_DIV,
	Token::OP_MOD
);


const TokenTypeSet TokenTypeSet::UNARY_OPERATORS(
	"UNARY_OPERATOR",
	Token::OP_PLUS,
	Token::OP_MINUS,
	Token::OP_MULT,
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_NOT,
	Token::OP_BIT_AND,
	Token::OP_BIT_NOT
);


const TokenTypeSet TokenTypeSet::POSTFIX_OPERATORS(
	"POSTFIX_OPERATOR",
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_ARROW,
	Token::OPAREN,
	Token::OBRACKET,
	Token::PERIOD
);


const TokenTypeSet TokenTypeSet::CONSTANT_VALUES(
	"CONSTANT_VALUE",
	Token::CONST_BOOL,
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT,
	Token::CONST_FLOAT,
	Token::CONST_STRING
);


const TokenTypeSet TokenTypeSet::BLOCK_DELIMITERS(
	"BLOCK_DELIMITER",
	Token::CBRACE,
	Token::END
);


const TokenTypeSet TokenTypeSet::DECLARATION_DELIMITERS(
	"DECLARATION_DELIMITER",
	Token::KEY_ENUM,
	Token::KEY_NAMESPACE,
	Token::KEY_STRUCT,
	Token::CBRACE,
	Token::SEMICOLON
);


const TokenTypeSet TokenTypeSet::ENUM_DELIMITERS(
	"ENUM_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON,
	Token::END
);


const TokenTypeSet TokenTypeSet::ENUMERATOR_DELIMITERS(
	"ENUMERATOR_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON,
	Token::COMMA
);


const TokenTypeSet TokenTypeSet::STRUCT_MEMBER_DELIMITERS(
	"STRUCT_MEMBER_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON
);


const TokenTypeSet TokenTypeSet::INITIALIZER_DELIMITERS(
	"INITIALIZER_DELIMITER",
	Token::OBRACE,
	Token::CBRACE,
	Token::COMMA
);


const TokenTypeSet TokenTypeSet::SWITCH_SECTION_DELIMITERS(
	"SWITCH_SECTION_DELIMITER",
	Token::KEY_CASE,
	Token::KEY_DEFAULT,
	Token::CBRACE,
	Token::END
);


const TokenTypeSet TokenTypeSet::STATEMENT_DELIMITERS(
	"STATEMENT_DELIMITER",
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
	Token::SEMICOLON
);


const TokenTypeSet TokenTypeSet::LABEL_DELIMITERS(
	"LABEL_DELIMITER",
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
	Token::COLON
);

}
