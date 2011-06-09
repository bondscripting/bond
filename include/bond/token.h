#ifndef BOND_TOKEN_H
#define BOND_TOKEN_H

#include "bond/streampos.h"
#include "bond/value.h"

#define BOND_TOKEN_LIST \
	/* Keywords */												    \
	BOND_TOKEN_ITEM(KEY_BOOL)                 \
	BOND_TOKEN_ITEM(KEY_BREAK)                \
	BOND_TOKEN_ITEM(KEY_CHAR)                 \
	BOND_TOKEN_ITEM(KEY_CASE)                 \
	BOND_TOKEN_ITEM(KEY_CAST)                 \
	BOND_TOKEN_ITEM(KEY_CONST)                \
	BOND_TOKEN_ITEM(KEY_CONTINUE)             \
	BOND_TOKEN_ITEM(KEY_DEFAULT)              \
	BOND_TOKEN_ITEM(KEY_DO)                   \
	BOND_TOKEN_ITEM(KEY_ELSE)                 \
	BOND_TOKEN_ITEM(KEY_ENUM)                 \
	BOND_TOKEN_ITEM(KEY_FLOAT)                \
	BOND_TOKEN_ITEM(KEY_FOR)                  \
	BOND_TOKEN_ITEM(KEY_IF)                   \
	BOND_TOKEN_ITEM(KEY_INT)                  \
	BOND_TOKEN_ITEM(KEY_NAMESPACE)            \
	BOND_TOKEN_ITEM(KEY_NATIVE)               \
	BOND_TOKEN_ITEM(KEY_REF)                  \
	BOND_TOKEN_ITEM(KEY_RETURN)               \
	BOND_TOKEN_ITEM(KEY_SIZEOF)               \
	BOND_TOKEN_ITEM(KEY_STRUCT)               \
	BOND_TOKEN_ITEM(KEY_SWITCH)               \
	BOND_TOKEN_ITEM(KEY_UINT)                 \
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
	BOND_TOKEN_ITEM(CONST_FLOAT)              \
	BOND_TOKEN_ITEM(CONST_STRING)             \
	                                          \
	/* Special values */                      \
	BOND_TOKEN_ITEM(INVALID)                  \
	BOND_TOKEN_ITEM(END)                      \
	BOND_TOKEN_ITEM(NUM_TOKEN_TYPES)          \

#define BOND_TOKEN_ERROR_LIST \
	BOND_TOKEN_ERROR_ITEM(NO_ERROR)                 \
	BOND_TOKEN_ERROR_ITEM(INVALID_ESCAPE)           \
	BOND_TOKEN_ERROR_ITEM(INVALID_OCTAL_INT)        \
	BOND_TOKEN_ERROR_ITEM(INVALID_HEX_INT)          \
	BOND_TOKEN_ERROR_ITEM(INVALID_INT)              \
	BOND_TOKEN_ERROR_ITEM(INVALID_FLOAT)            \
	BOND_TOKEN_ERROR_ITEM(EMPTY_CHARACTER_CONSTANT) \
	BOND_TOKEN_ERROR_ITEM(MULTI_CHARACTER_CONSTANT) \
	BOND_TOKEN_ERROR_ITEM(UNTERMINATED_CHARACTER)   \
	BOND_TOKEN_ERROR_ITEM(UNTERMINATED_STRING)      \
	BOND_TOKEN_ERROR_ITEM(UNTERMINATED_COMMENT)     \


namespace Bond
{

class Token
{
public:
	enum TokenType
	{
#define BOND_TOKEN_ITEM(item) item,
		BOND_TOKEN_LIST
#undef BOND_TOKEN_ITEM
	};

	enum ErrorType
	{
#define BOND_TOKEN_ERROR_ITEM(item) item,
		BOND_TOKEN_ERROR_LIST
#undef BOND_TOKEN_ERROR_ITEM
	};

	enum Annotation
	{
		OCTAL = 1,
		HEX = 2,
	};

	Token():
		mText(0),
		mTokenType(INVALID),
		mErrorType(NO_ERROR),
		mAnnotations(0)
	{
	}

	Token(const char *text, TokenType tokenType):
		mStartPos(-1, -1, -1),
		mEndPos(-1, -1, -1),
		mErrorPos(-1, -1, -1),
		mValue(),
		mText(text),
		mTokenType(tokenType),
		mErrorType(NO_ERROR),
		mAnnotations(0)
	{
	}

	Token(
			const StreamPos &startPos,
			const StreamPos &endPos,
			const StreamPos &errorPos,
			const Value &value,
			const char *text,
			TokenType tokenType,
			ErrorType errorType = NO_ERROR,
			short annotations = 0):
		mStartPos(startPos),
		mEndPos(endPos),
		mErrorPos(errorPos),
		mValue(value),
		mText(text),
		mTokenType(tokenType),
		mErrorType(errorType),
		mAnnotations(annotations)
	{
	}

	Token(const Token &other):
		mStartPos(other.mStartPos),
		mEndPos(other.mEndPos),
		mErrorPos(other.mErrorPos),
		mValue(other.mValue),
		mText(other.mText),
		mTokenType(other.mTokenType),
		mErrorType(other.mErrorType),
		mAnnotations(other.mAnnotations)
	{
	}

	Token &operator=(const Token &other);

	const StreamPos &GetStartPos() const { return mStartPos; }
	void SetStartPos(const StreamPos &pos) { mStartPos = pos; }

	const StreamPos &GetEndPos() const { return mEndPos; }
	void SetEndPos(const StreamPos &pos) { mEndPos = pos; }

	const StreamPos &GetErrorPos() const { return mErrorPos; }
	void SetErrorPos(const StreamPos &pos) { mErrorPos = pos; }

	const char *GetText() const { return mText; }
	void SetText(const char *text) { mText = text; }

	TokenType GetTokenType() const { return mTokenType; }
	void SetTokenType(const TokenType &type) { mTokenType = type; }

	ErrorType GetErrorType() const { return mErrorType; }
	void SetErrorType(const ErrorType &type) { mErrorType = type; }

	void AddAnnotation(const Annotation &annotation) { mAnnotations |= annotation; }
	bool HasAnnotation(const Annotation &annotation) const { return (mAnnotations & annotation) == annotation; }

	const Value &GetValue() const { return mValue; }

	bool GetBoolValue() const { return mValue.mBool; }
	void SetBoolValue(bool value) { mValue.mBool = value; }

	char GetCharValue() const { return mValue.mChar; }
	void SetCharValue(char value) { mValue.mChar = value; }

	bf32_t GetFloatValue() const { return mValue.mFloat; }
	void SetFloatValue(bf32_t value) { mValue.mFloat = value; }

	bi32_t GetIntValue() const { return mValue.mInt; }
	void SetIntValue(bi32_t value) { mValue.mInt = value; }

	bu32_t GetUIntValue() const { return mValue.mUInt; }
	void SetUIntValue(bu32_t value) { mValue.mUInt = value; }

	const char *GetStringValue() const { return mValue.mString.buffer; }
	int GetStringLength() const { return mValue.mString.length; }
	void SetStringValue(const char *buffer, int length);

	bu32_t GetHashCode() const { return mValue.mUInt; }
	void SetHashCode(bu32_t hash) { mValue.mUInt = hash; }

	const char *GetTokenName() const;
	static const char *GetTokenName(TokenType type);

	const char *GetErrorName() const;
	static const char *GetErrorName(ErrorType type);

private:
	StreamPos mStartPos;
	StreamPos mEndPos;
	StreamPos mErrorPos;
	Value mValue;
	const char *mText;
	TokenType mTokenType;
	ErrorType mErrorType;
	short mAnnotations;
};


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

	const char *GetTypeName() const { return mTypeName; }

	bool Contains(Token::TokenType type) const { return (mBitField[type / 32] & (1 << (type % 32))) != 0; }

	void Set(Token::TokenType type) { mBitField[type / 32] |= 1 << (type % 32); }

private:
	static const int NUM_BIT_FIELD_ELEMENTS = (Token::NUM_TOKEN_TYPES / 32) + 1;
	bu32_t mBitField[NUM_BIT_FIELD_ELEMENTS];
	const char *mTypeName;
};


extern const Token BOOL_TOKEN;
extern const Token CHAR_TOKEN;
extern const Token INT_TOKEN;
extern const Token UINT_TOKEN;
extern const Token FLOAT_TOKEN;

extern const TokenTypeSet PRIMITIVE_TYPE_SPECIFIERS_TYPESET;
extern const TokenTypeSet BOOLEAN_TYPE_SPECIFIERS_TYPESET;
extern const TokenTypeSet INTEGER_TYPE_SPECIFIERS_TYPESET;
extern const TokenTypeSet NUMERIC_TYPE_SPECIFIERS_TYPESET;
extern const TokenTypeSet VOID_TYPE_SPECIFIERS_TYPESET;
extern const TokenTypeSet TYPE_DESCRIPTORS_TYPESET;
extern const TokenTypeSet STRUCT_VARIANT_TYPESET;
extern const TokenTypeSet JUMP_OPERATORS_TYPESET;
extern const TokenTypeSet SWITCH_LABELS_TYPESET;
extern const TokenTypeSet ASSIGNMENT_OPERATORS_TYPESET;
extern const TokenTypeSet EQUALITY_OPERATORS_TYPESET;
extern const TokenTypeSet RELATIONAL_OPERATORS_TYPESET;
extern const TokenTypeSet SHIFT_OPERATORS_TYPESET;
extern const TokenTypeSet ADDITIVE_OPERATORS_TYPESET;
extern const TokenTypeSet MULTIPLICATIVE_OPERATORS_TYPESET;
extern const TokenTypeSet UNARY_OPERATORS_TYPESET;
extern const TokenTypeSet POSTFIX_OPERATORS_TYPESET;
extern const TokenTypeSet CONSTANT_VALUES_TYPESET;
extern const TokenTypeSet BLOCK_DELIMITERS_TYPESET;
extern const TokenTypeSet DECLARATION_DELIMITERS_TYPESET;
extern const TokenTypeSet ENUM_DELIMITERS_TYPESET;
extern const TokenTypeSet ENUMERATOR_DELIMITERS_TYPESET;
extern const TokenTypeSet STRUCT_MEMBER_DELIMITERS_TYPESET;
extern const TokenTypeSet INITIALIZER_DELIMITERS_TYPESET;
extern const TokenTypeSet SWITCH_SECTION_DELIMITERS_TYPESET;
extern const TokenTypeSet STATEMENT_DELIMITERS_TYPESET;
extern const TokenTypeSet LABEL_DELIMITERS_TYPESET;

}

#endif
