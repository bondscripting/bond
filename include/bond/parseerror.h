#ifndef BOND_PARSEERROR_H
#define BOND_PARSEERROR_H

#define BOND_PARSE_ERROR_LIST                                                                                 \
  BOND_PARSE_ERROR_ITEM(NO_ERROR, "")                                                                         \
  BOND_PARSE_ERROR_ITEM(PARSE_ERROR, "Parse error")                                                           \
  BOND_PARSE_ERROR_ITEM(UNEXPECTED_TOKEN, "Expected" )                                                        \
  BOND_PARSE_ERROR_ITEM(DUPLICATE_CONST, "Duplicate const keyword")                                           \
  BOND_PARSE_ERROR_ITEM(COMMA_IN_CONST_EXPRESSION, "Comma in constant expression")                            \
  BOND_PARSE_ERROR_ITEM(ASSIGNMENT_IN_CONST_EXPRESSION, "Assignment in constant expression" )                 \
  BOND_PARSE_ERROR_ITEM(INCREMENT_IN_CONST_EXPRESSION, "Increment/decrement operator in constant expression") \
  BOND_PARSE_ERROR_ITEM(FUNCTION_CALL_IN_CONST_EXPRESSION, "Function call in constant expression")            \
  BOND_PARSE_ERROR_ITEM(FUNCTION_DEFINITION_NOT_ALLOWED, "Function definition not allowed")                   \
  BOND_PARSE_ERROR_ITEM(INITIALIZER_NOT_ALLOWED, "Initializer not allowed")                                   \


namespace Bond
{

class TextWriter;
class Token;

class ParseError
{
public:
	enum Type
	{
#define BOND_PARSE_ERROR_ITEM(item, description) item,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR_ITEM
	};

	ParseError(): mType(NO_ERROR), mContext(0), mExpected("") {}

	ParseError(Type type, const Token *context, const char *expected):
		mType(type),
		mContext(context),
		mExpected(expected)
	{}

	ParseError(const ParseError &other):
		mType(other.mType),
		mContext(other.mContext),
		mExpected(other.mExpected)
	{}

	Type GetType() const { return mType; }
	const Token *GetContext() const { return mContext; }
	const char *GetExpected() const { return mExpected; }

	const char *GetDescription() const;
	static const char *GetDescription(Type type);

	void Print(TextWriter &writer) const;

private:
	static const char *ERROR_DESCRIPTIONS;

	Type mType;
	const Token *mContext;
	const char *mExpected;
};


class ParseErrorBuffer
{
public:
	ParseErrorBuffer();

	void Reset();

	void PushError(ParseError::Type type, const Token *context, const char *expected);

	bool HasErrors() const { return mNumErrors > 0; }
	int GetNumErrors() const { return mNumErrors; }
	const ParseError *GetError(int index) const { return mErrors + index; }

private:
	static const int MAX_ERRORS = 16;

	ParseError mErrors[MAX_ERRORS];
	int mNumErrors;
};

}

#endif
