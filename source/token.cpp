#include "bond/compiler/token.h"

namespace Bond
{

void Token::SetStringValue(const StringView &str)
{
	mValue.mString.buffer = str.data();
	mValue.mString.length = str.length();
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
	for (auto &element: mBitField)
	{
		element = 0;
	}

	Add(t0);
	Add(t1);
	Add(t2);
	Add(t3);
	Add(t4);
	Add(t5);
	Add(t6);
	Add(t7);
	Add(t8);
	Add(t9);
	Add(t10);
	Add(t11);
	Add(t12);
	Add(t13);
	Add(t14);
	Add(t15);
}


const Token VOID_TOKEN("void", Token::KEY_VOID);
const Token BOOL_TOKEN("bool", Token::KEY_BOOL);
const Token CHAR_TOKEN("char", Token::KEY_CHAR);
const Token UCHAR_TOKEN("uchar", Token::KEY_UCHAR);
const Token INT_TOKEN("int", Token::KEY_INT);
const Token UINT_TOKEN("uint", Token::KEY_UINT);
const Token LONG_TOKEN("long", Token::KEY_LONG);
const Token ULONG_TOKEN("ulong", Token::KEY_ULONG);
const Token FLOAT_TOKEN("float", Token::KEY_FLOAT);
const Token DOUBLE_TOKEN("double", Token::KEY_DOUBLE);


const TokenTypeSet PRIMITIVE_TYPE_SPECIFIERS_TYPESET(
	"PRIMITIVE_TYPE_SPECIFIER",
	Token::KEY_BOOL,
	Token::KEY_CHAR,
	Token::KEY_UCHAR,
	Token::KEY_SHORT,
	Token::KEY_USHORT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_LONG,
	Token::KEY_ULONG,
	Token::KEY_FLOAT,
	Token::KEY_DOUBLE,
	Token::KEY_VOID
);


const TokenTypeSet BOOLEAN_TYPE_SPECIFIERS_TYPESET(
	"BOOLEAN_TYPE_SPECIFIER",
	Token::KEY_BOOL
);


const TokenTypeSet CHAR_TYPE_SPECIFIERS_TYPESET(
	"CHAR_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_UCHAR
);


const TokenTypeSet INTEGER_TYPE_SPECIFIERS_TYPESET(
	"INTEGER_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_UCHAR,
	Token::KEY_SHORT,
	Token::KEY_USHORT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_LONG,
	Token::KEY_ULONG
);


const TokenTypeSet SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET(
	"SIGNED_INTEGER_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_SHORT,
	Token::KEY_INT,
	Token::KEY_LONG
);


const TokenTypeSet UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET(
	"UNSIGNED_INTEGER_TYPE_SPECIFIER",
	Token::KEY_UCHAR,
	Token::KEY_USHORT,
	Token::KEY_UINT,
	Token::KEY_ULONG
);


const TokenTypeSet LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET(
	"LEAST32_TYPE_SPECIFIER",
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_LONG,
	Token::KEY_ULONG
);


const TokenTypeSet MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET(
	"MOST32_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_UCHAR,
	Token::KEY_SHORT,
	Token::KEY_USHORT,
	Token::KEY_INT,
	Token::KEY_UINT
);


const TokenTypeSet LONG_INTEGER_TYPE_SPECIFIERS_TYPESET(
	"LONG_TYPE_SPECIFIER",
	Token::KEY_LONG,
	Token::KEY_ULONG
);


const TokenTypeSet NUMERIC_TYPE_SPECIFIERS_TYPESET(
	"NUMERIC_TYPE_SPECIFIER",
	Token::KEY_CHAR,
	Token::KEY_UCHAR,
	Token::KEY_SHORT,
	Token::KEY_USHORT,
	Token::KEY_INT,
	Token::KEY_UINT,
	Token::KEY_LONG,
	Token::KEY_ULONG,
	Token::KEY_FLOAT,
	Token::KEY_DOUBLE
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


const TokenTypeSet MOST32_INTEGER_CONSTANTS_TYPESET(
	"MOST32_INTEGER_CONSTANT",
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT
);


const TokenTypeSet SIGNED_INTEGER_CONSTANTS_TYPESET(
	"SIGNED_INTEGER_CONSTANT",
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_LONG
);


const TokenTypeSet JUMP_KEYWORDS_TYPESET(
	"JUMP_KEYWORDS",
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


const TokenTypeSet PROPERTYOF_OPERATORS_TYPESET(
	"PROPERTYOF_OPERATOR",
	Token::KEY_ALIGNOF,
	Token::KEY_SIZEOF
);


const TokenTypeSet CONSTANT_LITERALS_TYPESET(
	"CONSTANT_LITERAL",
	Token::CONST_BOOL,
	Token::CONST_CHAR,
	Token::CONST_INT,
	Token::CONST_UINT,
	Token::CONST_LONG,
	Token::CONST_ULONG,
	Token::CONST_FLOAT,
	Token::CONST_DOUBLE,
	Token::CONST_STRING,
	Token::CONST_NULL
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
	Token::KEY_NATIVE,
	Token::KEY_STRUCT,
	Token::CBRACE,
	Token::SEMICOLON,
	Token::END
);


const TokenTypeSet INCLUDE_AND_DECLARATION_DELIMITERS_TYPESET(
	"INCLUDE_AND_DECLARATION_DELIMITER",
	Token::KEY_ENUM,
	Token::KEY_INCLUDE,
	Token::KEY_NAMESPACE,
	Token::KEY_NATIVE,
	Token::KEY_STRUCT,
	Token::CBRACE,
	Token::SEMICOLON,
	Token::END
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
