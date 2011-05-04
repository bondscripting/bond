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


const Token BOOL_TOKEN("bool", Token::KEY_BOOL);
const Token CHAR_TOKEN("char", Token::KEY_CHAR);
const Token INT_TOKEN("int", Token::KEY_INT);
const Token UINT_TOKEN("uint", Token::KEY_UINT);
const Token FLOAT_TOKEN("float", Token::KEY_FLOAT);


const TokenTypeSet PRIMITIVE_TYPE_SPECIFIERS_TYPESET(
	"PRIMITIVE_TYPE_SPECIFIER",
	Token::KEY_BOOL,
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_VOID
);


const TokenTypeSet BOOLEAN_TYPE_SPECIFIERS_TYPESET(
	"BOOLEAN_TYPE_SPECIFIER",
	Token::KEY_BOOL
);


const TokenTypeSet INTEGER_TYPE_SPECIFIERS_TYPESET(
	"INTEGER_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_INT,
	Token::KEY_UINT
);


const TokenTypeSet NUMERIC_TYPE_SPECIFIERS_TYPESET(
	"NUMERIC_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_FLOAT,
	Token::KEY_INT,
	Token::KEY_UINT
);


const TokenTypeSet VOID_TYPE_SPECIFIERS_TYPESET(
	"VOID_TYPE_SPECIFIER",
	Token::KEY_VOID
);


const TokenTypeSet TYPE_DESCRIPTORS_TYPESET(
	"TYPE_DESCRIPTOR",
	Token::OP_STAR,
	Token::OBRACKET
);


const TokenTypeSet JUMP_OPERATORS_TYPESET(
	"JUMP_OPERATOR",
	Token::KEY_BREAK,
	Token::KEY_CONTINUE,
	Token::KEY_RETURN
);


const TokenTypeSet SWITCH_LABELS_TYPESET(
	"SWITCH_LABEL",
	Token::KEY_CASE,
	Token::KEY_DEFAULT
);


const TokenTypeSet ASSIGNMENT_OPERATORS_TYPESET(
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


const TokenTypeSet EQUALITY_OPERATORS_TYPESET(
	"EQUALITY_OPERATOR",
	Token::OP_EQUAL,
	Token::OP_NOT_EQUAL
);


const TokenTypeSet RELATIONAL_OPERATORS_TYPESET(
	"RELATIONAL_OPERATOR",
	Token::OP_LT,
	Token::OP_LTE,
	Token::OP_GT,
	Token::OP_GTE
);


const TokenTypeSet SHIFT_OPERATORS_TYPESET(
	"SHIFT_OPERATOR",
	Token::OP_LEFT,
	Token::OP_RIGHT
);


const TokenTypeSet ADDITIVE_OPERATORS_TYPESET(
	"ADDITIVE_OPERATOR",
	Token::OP_PLUS,
	Token::OP_MINUS
);


const TokenTypeSet MULTIPLICATIVE_OPERATORS_TYPESET(
	"MULTIPLICATIVE_OPERATOR",
	Token::OP_STAR,
	Token::OP_DIV,
	Token::OP_MOD
);


const TokenTypeSet UNARY_OPERATORS_TYPESET(
	"UNARY_OPERATOR",
	Token::OP_PLUS,
	Token::OP_MINUS,
	Token::OP_STAR,
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_NOT,
	Token::OP_AMP,
	Token::OP_BIT_NOT
);


const TokenTypeSet POSTFIX_OPERATORS_TYPESET(
	"POSTFIX_OPERATOR",
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_ARROW,
	Token::OPAREN,
	Token::OBRACKET,
	Token::PERIOD
);


const TokenTypeSet CONSTANT_VALUES_TYPESET(
	"CONSTANT_VALUE",
	Token::CONST_BOOL,
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT,
	Token::CONST_FLOAT,
	Token::CONST_STRING
);


const TokenTypeSet BLOCK_DELIMITERS_TYPESET(
	"BLOCK_DELIMITER",
	Token::CBRACE,
	Token::END
);


const TokenTypeSet DECLARATION_DELIMITERS_TYPESET(
	"DECLARATION_DELIMITER",
	Token::KEY_ENUM,
	Token::KEY_NAMESPACE,
	Token::KEY_STRUCT,
	Token::CBRACE,
	Token::SEMICOLON
);


const TokenTypeSet ENUM_DELIMITERS_TYPESET(
	"ENUM_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON,
	Token::END
);


const TokenTypeSet ENUMERATOR_DELIMITERS_TYPESET(
	"ENUMERATOR_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON,
	Token::COMMA
);


const TokenTypeSet STRUCT_MEMBER_DELIMITERS_TYPESET(
	"STRUCT_MEMBER_DELIMITER",
	Token::CBRACE,
	Token::SEMICOLON
);


const TokenTypeSet INITIALIZER_DELIMITERS_TYPESET(
	"INITIALIZER_DELIMITER",
	Token::OBRACE,
	Token::CBRACE,
	Token::COMMA
);


const TokenTypeSet SWITCH_SECTION_DELIMITERS_TYPESET(
	"SWITCH_SECTION_DELIMITER",
	Token::KEY_CASE,
	Token::KEY_DEFAULT,
	Token::CBRACE,
	Token::END
);


const TokenTypeSet STATEMENT_DELIMITERS_TYPESET(
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


const TokenTypeSet LABEL_DELIMITERS_TYPESET(
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
