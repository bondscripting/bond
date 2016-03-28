#ifndef BOND_COMPILER_TOKEN_H
#define BOND_COMPILER_TOKEN_H

#include "bond/compiler/streampos.h"
#include "bond/types/simplestring.h"
#include "bond/types/value.h"

#define BOND_TOKEN_LIST \
	/* Keywords */												    \
	BOND_TOKEN_ITEM(KEY_BOOL)                 \
	BOND_TOKEN_ITEM(KEY_BREAK)                \
	BOND_TOKEN_ITEM(KEY_CASE)                 \
	BOND_TOKEN_ITEM(KEY_CAST)                 \
	BOND_TOKEN_ITEM(KEY_CHAR)                 \
	BOND_TOKEN_ITEM(KEY_CONST)                \
	BOND_TOKEN_ITEM(KEY_CONTINUE)             \
	BOND_TOKEN_ITEM(KEY_DEFAULT)              \
	BOND_TOKEN_ITEM(KEY_DO)                   \
	BOND_TOKEN_ITEM(KEY_DOUBLE)               \
	BOND_TOKEN_ITEM(KEY_ELSE)                 \
	BOND_TOKEN_ITEM(KEY_ENUM)                 \
	BOND_TOKEN_ITEM(KEY_FLOAT)                \
	BOND_TOKEN_ITEM(KEY_FOR)                  \
	BOND_TOKEN_ITEM(KEY_IF)                   \
	BOND_TOKEN_ITEM(KEY_INCLUDE)              \
	BOND_TOKEN_ITEM(KEY_INT)                  \
	BOND_TOKEN_ITEM(KEY_LONG)                 \
	BOND_TOKEN_ITEM(KEY_NAMESPACE)            \
	BOND_TOKEN_ITEM(KEY_NATIVE)               \
	BOND_TOKEN_ITEM(KEY_RETURN)               \
	BOND_TOKEN_ITEM(KEY_SHORT)                \
	BOND_TOKEN_ITEM(KEY_SIZEOF)               \
	BOND_TOKEN_ITEM(KEY_STRUCT)               \
	BOND_TOKEN_ITEM(KEY_SWITCH)               \
	BOND_TOKEN_ITEM(KEY_THIS)                 \
	BOND_TOKEN_ITEM(KEY_UCHAR)                \
	BOND_TOKEN_ITEM(KEY_UINT)                 \
	BOND_TOKEN_ITEM(KEY_ULONG)                \
	BOND_TOKEN_ITEM(KEY_USHORT)               \
	BOND_TOKEN_ITEM(KEY_VOID)                 \
	BOND_TOKEN_ITEM(KEY_WHILE)                \
	                                          \
	/* Identifier */                          \
	BOND_TOKEN_ITEM(IDENTIFIER)               \
	                                          \
	/* Operators */                           \
	BOND_TOKEN_ITEM(OP_PLUS)      /* '+' */   \
	BOND_TOKEN_ITEM(OP_MINUS)     /* '-' */   \
	BOND_TOKEN_ITEM(OP_STAR)      /* '*' */   \
	BOND_TOKEN_ITEM(OP_DIV)       /* '/' */   \
	BOND_TOKEN_ITEM(OP_MOD)       /* '%' */   \
	BOND_TOKEN_ITEM(OP_INC)       /* '++' */  \
	BOND_TOKEN_ITEM(OP_DEC)       /* '--' */  \
	BOND_TOKEN_ITEM(OP_LT)        /* '<' */   \
	BOND_TOKEN_ITEM(OP_LTE)       /* '<=' */  \
	BOND_TOKEN_ITEM(OP_GT)        /* '>' */   \
	BOND_TOKEN_ITEM(OP_GTE)       /* '>=' */  \
	BOND_TOKEN_ITEM(OP_LEFT)      /* '<<' */  \
	BOND_TOKEN_ITEM(OP_RIGHT)     /* '>>' */  \
	BOND_TOKEN_ITEM(OP_EQUAL)     /* '==' */  \
	BOND_TOKEN_ITEM(OP_NOT_EQUAL) /* '!=' */  \
	BOND_TOKEN_ITEM(OP_NOT)       /* '!' */   \
	BOND_TOKEN_ITEM(OP_AND)       /* '&&' */  \
	BOND_TOKEN_ITEM(OP_OR)        /* '||' */  \
	BOND_TOKEN_ITEM(OP_AMP)       /* '&'  */  \
	BOND_TOKEN_ITEM(OP_BIT_OR)    /* '|'  */  \
	BOND_TOKEN_ITEM(OP_BIT_XOR)   /* '^'  */  \
	BOND_TOKEN_ITEM(OP_BIT_NOT)   /* '~'  */  \
	BOND_TOKEN_ITEM(OP_TERNARY)   /* '?'  */  \
	BOND_TOKEN_ITEM(OP_ARROW)     /* '->' */  \
	                                          \
	/* Assignment */                          \
	BOND_TOKEN_ITEM(ASSIGN)       /* '=' */   \
	BOND_TOKEN_ITEM(ASSIGN_LEFT)  /* '<<=' */ \
	BOND_TOKEN_ITEM(ASSIGN_RIGHT) /* '>>=' */ \
	BOND_TOKEN_ITEM(ASSIGN_PLUS)  /* '+=' */  \
	BOND_TOKEN_ITEM(ASSIGN_MINUS) /* '-=' */  \
	BOND_TOKEN_ITEM(ASSIGN_MULT)  /* '*=' */  \
	BOND_TOKEN_ITEM(ASSIGN_DIV)   /* '/=' */  \
	BOND_TOKEN_ITEM(ASSIGN_MOD)   /* '%=' */  \
	BOND_TOKEN_ITEM(ASSIGN_AND)   /* '&=' */  \
	BOND_TOKEN_ITEM(ASSIGN_OR)    /* '|=' */  \
	BOND_TOKEN_ITEM(ASSIGN_XOR)   /* '^=' */  \
	                                          \
	/* Punctuation */                         \
	BOND_TOKEN_ITEM(OPAREN)       /* '(' */   \
	BOND_TOKEN_ITEM(CPAREN)       /* ')' */   \
	BOND_TOKEN_ITEM(OBRACE)       /* '{' */   \
	BOND_TOKEN_ITEM(CBRACE)       /* '}' */   \
	BOND_TOKEN_ITEM(OBRACKET)     /* '[' */   \
	BOND_TOKEN_ITEM(CBRACKET)     /* ']' */   \
	BOND_TOKEN_ITEM(PERIOD)       /* '.' */   \
	BOND_TOKEN_ITEM(SEMICOLON)    /* ';' */   \
	BOND_TOKEN_ITEM(COLON)        /* ':' */   \
	BOND_TOKEN_ITEM(SCOPE)        /* '::' */  \
	BOND_TOKEN_ITEM(COMMA)        /* ',' */   \
	                                          \
	/* Literal constants */                   \
	BOND_TOKEN_ITEM(CONST_BOOL)               \
	BOND_TOKEN_ITEM(CONST_CHAR)               \
	BOND_TOKEN_ITEM(CONST_INT)                \
	BOND_TOKEN_ITEM(CONST_UINT)               \
	BOND_TOKEN_ITEM(CONST_LONG)               \
	BOND_TOKEN_ITEM(CONST_ULONG)              \
	BOND_TOKEN_ITEM(CONST_FLOAT)              \
	BOND_TOKEN_ITEM(CONST_DOUBLE)             \
	BOND_TOKEN_ITEM(CONST_STRING)             \
	BOND_TOKEN_ITEM(CONST_NULL)               \
	                                          \
	/* Special values */                      \
	BOND_TOKEN_ITEM(INVALID)                  \
	BOND_TOKEN_ITEM(END)                      \
	BOND_TOKEN_ITEM(NUM_TOKEN_TYPES)          \

namespace Bond
{

/// \addtogroup compiler
/// @{

/// \brief A token describing a syntactic unit of the Bond scripting language (e.g. keywords,
/// identifiers, literal constants and operators).
///
/// A Token is typically created by a Lexer as it scans Bond source code. It contains the text from
/// which the token is formed as it appears in the Bond source, the position at which it appears in
/// the Bond souce, a type and a value for tokens which are literal constants.
///
/// \sa Lexer
class Token
{
public:
	/// \brief An enumeration describing the various types of Tokens.
	enum TokenType
	{
#define BOND_TOKEN_ITEM(item) item,
		BOND_TOKEN_LIST
#undef BOND_TOKEN_ITEM
	};

	/// \brief An enumeration describing a characteristic of the token as it appears in the Bond
	/// source. Currently only used to qualify integer literals as being expressed as hex or octal.
	enum Annotation
	{
		OCTAL = 1,
		HEX = 2,
	};

	/// \brief Default constructs an empty Token object.
	Token():
		mText(nullptr),
		mStartPos(0, 0, 0),
		mEndIndex(0),
		mFileName(nullptr),
		mTokenType(INVALID),
		mAnnotations(0)
	{}

	/// \brief Constructs a Token object with the given text and type.
	/// \param text The text as the token appears in the Bond source.
	/// \param tokenType The type of token.
	Token(const char *text, TokenType tokenType):
		mText(text),
		mValue(),
		mStartPos(0, 0, 0),
		mEndIndex(0),
		mFileName(nullptr),
		mTokenType(tokenType),
		mAnnotations(0)
	{}

	/// \brief Constructs a fully described token.
	/// \param startPos The starting position of the token in the Bond source.
	/// \param endPos The ending position of the token in the Bond source.
	/// \param text The text as the token appears in the Bond source.
	/// \param value The value of a the token for constant literals.
	/// \param fileName The name of the Bond source file in which the token appears.
	/// \param tokenType The type of token.
	/// \param annotations Bit field of Annotation values.
	Token(
			const StreamPos &startPos,
			const StreamPos &endPos,
			const Value &value,
			const char *text,
			const char *fileName,
			TokenType tokenType,
			short annotations = 0):
		mText(text),
		mValue(value),
		mStartPos(startPos),
		mEndIndex(endPos.index),
		mFileName(fileName),
		mTokenType(tokenType),
		mAnnotations(annotations)
	{}

	/// \brief Returns the text that forms the token as it appears in the Bond source.
	const SimpleString &GetText() const { return mText; }

	/// \brief Like GetText, but returns the text as a C-style string. The text may be truncated
	/// if it contains a null character.
	const char *GetRawText() const { return mText.GetString(); }

	/// \brief Assigns the text that forms the token. The text is assumed to be null terminated.
	void SetText(const char *text) { mText = SimpleString(text); }

	/// \brief Assigns the text that forms the token. The text may contain null characters..
	/// \param text The pointer to the string of text.
	/// \param length The number of characters in the text.
	void SetText(const char *text, size_t length) { mText = SimpleString(text, length); }

	/// \brief Returns the name of the Bond source file from which the Token was generated.
	const char *GetFileName() const { return mFileName; }

	/// \brief Assigns the name of the Bond source file from which the Token was generated.
	void SetFileName(const char *fileName) { mFileName = fileName; }

	/// \brief Returns the starting position of the Token in the Bond source file.
	const StreamPos &GetStartPos() const { return mStartPos; }

	/// \brief Assigns the starting position of the Token in the Bond source file.
	void SetStartPos(const StreamPos &pos) { mStartPos = pos; }

	/// \brief Returns the offset into the Bond source of the end of the token. To save space, the
	/// entire position information is not available.
	size_t GetEndIndex() const { return mEndIndex; }

	/// \brief Assigns the offset into the Bond source of the end of the token.
	void SetEndIndex(size_t index) { mEndIndex = index; }

	/// \brief Returns the type of the token.
	TokenType GetTokenType() const { return mTokenType; }

	/// \brief Assigns the type of the Token.
	void SetTokenType(const TokenType &type) { mTokenType = type; }

	/// \brief Adds an annotation to the token.
	void AddAnnotation(const Annotation &annotation) { mAnnotations |= annotation; }

	/// \brief Returns whether the Token as a specified annotation.
	bool HasAnnotation(const Annotation &annotation) const { return (mAnnotations & annotation) == annotation; }

	/// \brief Returns the value of the token. Valid only for constant literal tokens.
	const Value &GetValue() const { return mValue; }

	/// \brief Returns the boolean value of the Token. Valid only for Tokens of type `CONST_BOOL`.
	bool GetBoolValue() const { return mValue.mBool; }

	/// \brief Assigns the boolean value of the Token. Valid only for Tokens of type `CONST_BOOL`.
	void SetBoolValue(bool value) { mValue.mBool = value; }

	/// \brief Returns the integral value of the Token. Valid only for Tokens of type `CONST_INT`.
	int32_t GetIntValue() const { return mValue.mInt; }

	/// \brief Assigns the integral value of the Token. Valid only for Tokens of type `CONST_INT`.
	void SetIntValue(int32_t value) { mValue.mInt = value; }

	/// \brief Returns the integral value of the Token. Valid only for Tokens of type `CONST_UINT`.
	uint32_t GetUIntValue() const { return mValue.mUInt; }

	/// \brief Assigns the integral value of the Token. Valid only for Tokens of type `CONST_UINT`.
	void SetUIntValue(uint32_t value) { mValue.mUInt = value; }

	/// \brief Returns the integral value of the Token. Valid only for Tokens of type `CONST_LONG`.
	int64_t GetLongValue() const { return mValue.mLong; }

	/// \brief Assigns the integral value of the Token. Valid only for Tokens of type `CONST_LONG`.
	void SetLongValue(int64_t value) { mValue.mLong = value; }

	/// \brief Returns the integral value of the Token. Valid only for Tokens of type `CONST_ULONG`.
	uint64_t GetULongValue() const { return mValue.mULong; }

	/// \brief Assigns the integral value of the Token. Valid only for Tokens of type `CONST_ULONG`.
	void SetULongValue(uint64_t value) { mValue.mULong = value; }

	/// \brief Returns the floating point value of the Token. Valid only for Tokens of type `CONST_FLOAT`.
	float GetFloatValue() const { return mValue.mFloat; }

	/// \brief Assigns the floating point value of the Token. Valid only for Tokens of type `CONST_FLOAT`.
	void SetFloatValue(float value) { mValue.mFloat = value; }

	/// \brief Returns the floating point value of the Token. Valid only for Tokens of type `CONST_DOUBLE`.
	double GetDoubleValue() const { return mValue.mDouble; }

	/// \brief Assigns the floating point value of the Token. Valid only for Tokens of type `CONST_DOUBLE`.
	void SetDoubleValue(double value) { mValue.mDouble = value; }

	/// \brief Returns the string value of the Token. Valid only for Tokens of type `CONST_STRING`.
	const SimpleString GetStringValue() const { return SimpleString(mValue.mString.buffer, mValue.mString.length); }

	/// \brief A short-hand for GetStringValue().GetLength().
	size_t GetStringLength() const { return mValue.mString.length; }

	/// \brief Assigns the string value of the Token. Valid only for Tokens of type `CONST_STRING`.
	void SetStringValue(const SimpleString &str);

	/// \brief Returns a descriptive name for the token's type.
	const char *GetTokenName() const;

	/// \brief Returns a descriptive name for the given TokenType.
	static const char *GetTokenName(TokenType type);

private:
	SimpleString mText;
	Value mValue;
	StreamPos mStartPos;
	size_t mEndIndex;
	const char *mFileName;
	TokenType mTokenType;
	short mAnnotations;
};


/// \brief Represents a collection of related TokenTypes.
///
/// A TokenTypeSet is used to validate that type of a Token belongs to a collection of related
/// TokenTypes. It is typically used by a Parser to validate that the Tokens contained in a
/// TokenStream are syntactically correct.
/// \sa Parser, TokenStream
class TokenTypeSet
{
 public:
	TokenTypeSet(
		const char *const typeName,
		Token::TokenType t0 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t1 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t2 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t3 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t4 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t5 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t6 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t7 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t8 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t9 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t10 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t11 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t12 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t13 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t14 = Token::NUM_TOKEN_TYPES,
		Token::TokenType t15 = Token::NUM_TOKEN_TYPES);

	/// \brief Returns a descriptive name for the type set.
	const char *GetTypeName() const { return mTypeName; }

	/// \brief Returns whether the give TokenType is contained in the type set.
	bool Contains(Token::TokenType type) const { return (mBitField[type / 32] & uint32_t(1 << (type % 32))) != 0; }

	/// \brief Adds the given TokenType to the set.
	void Add(Token::TokenType type) { mBitField[type / 32] |= uint32_t(1 << (type % 32)); }

private:
	static const int NUM_BIT_FIELD_ELEMENTS = (Token::NUM_TOKEN_TYPES / 32) + 1;
	uint32_t mBitField[NUM_BIT_FIELD_ELEMENTS];
	const char *mTypeName;
};


/// \brief Constant Token of type `KEY_VOID` representing the `void` keyword.
extern const Token VOID_TOKEN;

/// \brief Constant Token of type `KEY_BOOL` representing the `bool` keyword.
extern const Token BOOL_TOKEN;

/// \brief Constant Token of type `KEY_CHAR` representing the `char` keyword.
extern const Token CHAR_TOKEN;

/// \brief Constant Token of type `KEY_UCHAR` representing the `uchar` keyword.
extern const Token UCHAR_TOKEN;

/// \brief Constant Token of type `KEY_INT` representing the `int` keyword.
extern const Token INT_TOKEN;

/// \brief Constant Token of type `KEY_UINT` representing the `uint` keyword.
extern const Token UINT_TOKEN;

/// \brief Constant Token of type `KEY_LONG` representing the `long` keyword.
extern const Token LONG_TOKEN;

/// \brief Constant Token of type `KEY_ULONG` representing the `ulong` keyword.
extern const Token ULONG_TOKEN;

/// \brief Constant Token of type `KEY_FLOAT` representing the `float` keyword.
extern const Token FLOAT_TOKEN;

/// \brief Constant Token of type `KEY_DOUBLE` representing the `double` keyword.
extern const Token DOUBLE_TOKEN;

/// \brief Constant TokenTypeSet of all keywords representing Bond primitive typesn namely:
/// `KEY_BOOL`, `KEY_CHAR`, `KEY_UCHAR`, `KEY_SHORT`, `KEY_USHORT`, `KEY_INT`, `KEY_UINT`,
/// `KEY_LONG`, `KEY_ULONG`, `KEY_FLOAT`, `KEY_DOUBLE` and `KEY_VOID`.
extern const TokenTypeSet PRIMITIVE_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet containing only `KEY_BOOL`.
extern const TokenTypeSet BOOLEAN_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond character types, namely:
/// `KEY_CHAR` and `KEY_UCHAR`.
extern const TokenTypeSet CHAR_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond integral types, namely:
/// `KEY_CHAR`, `KEY_UCHAR`, `KEY_SHORT`, `KEY_USHORT`, `KEY_INT`, `KEY_UINT`, `KEY_LONG`
/// and `KEY_ULONG`.
extern const TokenTypeSet INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond signed integral types, namely:
/// `KEY_CHAR`, `KEY_SHORT`, `KEY_INT` and `KEY_LONG`.
extern const TokenTypeSet SIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond unsigned integral types, namely:
/// `KEY_UCHAR`, `KEY_USHORT`, `KEY_UINT` and `KEY_ULONG`.
extern const TokenTypeSet UNSIGNED_INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond integral types having at least
/// 32 bits, namely: `KEY_INT`, `KEY_UINT`, `KEY_LONG` and `KEY_ULONG`.
extern const TokenTypeSet LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond integral types having at most 32
/// bits, namely: `KEY_CHAR`, `KEY_UCHAR`, `KEY_SHORT`, `KEY_USHORT`, `KEY_INT` and `KEY_UINT`.
extern const TokenTypeSet MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond long integral types, namely:
/// `KEY_LONG` and `KEY_ULONG`.
extern const TokenTypeSet LONG_INTEGER_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords representing Bond numeric types, namely:
/// `KEY_CHAR`, `KEY_UCHAR`, `KEY_SHORT`, `KEY_USHORT`, `KEY_INT`, `KEY_UINT`, `KEY_LONG`,
/// `KEY_ULONG`, `KEY_FLOAT` and `KEY_DOUBLE`.
extern const TokenTypeSet NUMERIC_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet containing only `KEY_VOID`.
extern const TokenTypeSet VOID_TYPE_SPECIFIERS_TYPESET;

/// \brief Constant TokenTypeSet of all punctuation token types used to begin a new level of
/// indirection in type descriptors, namely: `OP_STAR` and `OBRACKET`.
extern const TokenTypeSet TYPE_DESCRIPTORS_TYPESET;

/// \brief Constant TokenTypeSet of all integral literal constants representable as an int or uint,
/// namely: ???
// TODO Figure out why this type set is defined as it is.
extern const TokenTypeSet INTEGER_CONSTANTS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords used for jumps in Bond source code, namely:
/// `KEY_BREAK`, `KEY_CONTINUE` and `KEY_RETURN`.
extern const TokenTypeSet JUMP_KEYWORDS_TYPESET;

/// \brief Constant TokenTypeSet of all keywords used for defining case labels in Bond switch
/// statements, namely: `KEY_CASE` and `KEY_DEFAULT`.
extern const TokenTypeSet SWITCH_LABELS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond assignment operators, namely: `ASSIGN`, `ASSIGN_LEFT`,
/// `ASSIGN_RIGHT`, `ASSIGN_PLUS`, `ASSIGN_MINUS`, `ASSIGN_MULT`, `ASSIGN_DIV`, `ASSIGN_MOD`,
/// `ASSIGN_AND`, `ASSIGN_OR` and `ASSIGN_XOR`.
extern const TokenTypeSet ASSIGNMENT_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond equality operators, namely: `OP_EQUAL`, `OP_NOT_EQUAL`.
extern const TokenTypeSet EQUALITY_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond relational operators, namely: `OP_LT`, `OP_LTE`,
/// `OP_GT` and `OP_GTE`.
extern const TokenTypeSet RELATIONAL_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond shift operators, namely: `OP_LEFT` and `OP_RIGHT`.
extern const TokenTypeSet SHIFT_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond additive arithmetic operators, namely: `OP_PLUS`
/// and `OP_MINUS`.
extern const TokenTypeSet ADDITIVE_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond multiplicative arithmetic operators, namely: `OP_STAR`,
/// `OP_DIV` and `OP_MOD`.
extern const TokenTypeSet MULTIPLICATIVE_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond prefix unary operators, namely: `OP_PLUS`, `OP_MINUS`,
/// `OP_STAR`, `OP_INC`, `OP_DEC`, `OP_NOT`, `OP_AMP` and `OP_BIT_NOT`.
extern const TokenTypeSet UNARY_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond postfix unary operators, namely: `OP_INC`, `OP_DEC`,
/// `OP_ARROW`, `OPAREN`, `OBRACKET` and `PERIOD`.
extern const TokenTypeSet POSTFIX_OPERATORS_TYPESET;

/// \brief Constant TokenTypeSet of all Bond constant literals, namely: `CONST_BOOL`, `CONST_CHAR`,
/// `CONST_INT`, `CONST_UINT`, `CONST_LONG`, `CONST_ULONG`, `CONST_FLOAT`, `CONST_DOUBLE`,
/// `CONST_STRING` and `CONST_NULL`.
extern const TokenTypeSet CONSTANT_LITERALS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of a block of Bond source, namely:
/// `CBRACE` and `END`.
extern const TokenTypeSet BLOCK_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond declarations, namely:
/// `KEY_ENUM`, `KEY_NAMESPACE`, `KEY_NATIVE`, `KEY_STRUCT`, `CBRACE`, `SEMICOLON`, and `END`.
extern const TokenTypeSet DECLARATION_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond declarations and includes,
/// namely: `KEY_ENUM`, `KEY_INCLUDE`, `KEY_NAMESPACE`, `KEY_NATIVE`, `KEY_STRUCT`, `CBRACE`,
/// `SEMICOLON`, and `END`.
extern const TokenTypeSet INCLUDE_AND_DECLARATION_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond enum declarations, namely:
/// `CBRACE`, `SEMICOLON`, and `END`.
extern const TokenTypeSet ENUM_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond enumerator declarations,
/// namely: `CBRACE`, `SEMICOLON`, and `COMMA`.
extern const TokenTypeSet ENUMERATOR_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond struct member declarations,
/// namely: `CBRACE` and `SEMICOLON`.
extern const TokenTypeSet STRUCT_MEMBER_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond initializers, namely:
/// `CBRACE`, `OBRACE` and `COMMA`.
extern const TokenTypeSet INITIALIZER_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond switch statement sections,
/// namely: `KEY_CASE`, `KEY_DEFAULT`, `CBRACE` and `COMMA`.
extern const TokenTypeSet SWITCH_SECTION_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond statements, namely:
/// `KEY_BREAK`, `KEY_CASE`, `KEY_CONTINUE`, `KEY_DEFAULT`, `KEY_DO`, `KEY_IF`, `KEY_FOR`,
/// `KEY_RETURN`, `KEY_SWITCH`, `KEY_WHILE`, `OBRACE`, `CBRACE` and `COLON`.
extern const TokenTypeSet STATEMENT_DELIMITERS_TYPESET;

/// \brief Constant TokenTypeSet of tokens delimiting the end of Bond switch labels, namely:
/// `KEY_BREAK`, `KEY_CASE`, `KEY_CONTINUE`, `KEY_DEFAULT`, `KEY_DO`, `KEY_IF`, `KEY_FOR`,
/// `KEY_RETURN`, `KEY_SWITCH`, `KEY_WHILE`, `OBRACE`, `CBRACE`, `SEMICOLON` and `COLON`.
extern const TokenTypeSet LABEL_DELIMITERS_TYPESET;

/// @}

}

#endif
