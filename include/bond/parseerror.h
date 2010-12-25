#ifndef BOND_PARSEERROR_H
#define BOND_PARSEERROR_H

#define BOND_PARSE_ERROR_LIST                                               \
  BOND_PARSE_ERROR(BASIC, NO_ERROR,                                         \
    "")                                                                     \
  BOND_PARSE_ERROR(BASIC, DUPLICATE_CONST,                                  \
    "Duplicate 'const' keyword.")                                           \
  BOND_PARSE_ERROR(CONTEXT, PARSE_ERROR,                                    \
    "Parse error near '%s'.")                                               \
  BOND_PARSE_ERROR(CONTEXT, COMMA_IN_CONST_EXPRESSION,                      \
    "Comma in constant expression near '%s'.")                              \
  BOND_PARSE_ERROR(CONTEXT, ASSIGNMENT_IN_CONST_EXPRESSION,                 \
    "Assignment in constant expression at '%s'." )                          \
  BOND_PARSE_ERROR(CONTEXT, INCREMENT_IN_CONST_EXPRESSION,                  \
    "'%s' operator in constant expression.")                                \
  BOND_PARSE_ERROR(CONTEXT, FUNCTION_CALL_IN_CONST_EXPRESSION,              \
    "Function call in constant expression near '%s'.")                      \
  BOND_PARSE_ERROR(CONTEXT, FUNCTION_DEFINITION_NOT_ALLOWED,                \
    "Function definition not allowed near '%s'.")                           \
  BOND_PARSE_ERROR(CONTEXT, INITIALIZER_NOT_ALLOWED,                        \
    "Initializer not allowed near '%s'.")                                   \
  BOND_PARSE_ERROR(CONTEXT_LINE, DUPLICATE_SYMBOL,                          \
    "Duplicate symbol '%s' previously defined on line '%d'.")               \
  BOND_PARSE_ERROR(CONTEXT_LINE, DUPLICATE_FUNCTION_DEFINITION,             \
    "Duplicate function definition '%s' previously defined on line '%d'.")  \
  BOND_PARSE_ERROR(CONTEXT_LINE, FUNCTION_PROTOTYPE_MISMATCH,               \
    "Mismatching function prototype '%s' previously defined on line '%d'.") \
  BOND_PARSE_ERROR(STR_CONTEXT, UNEXPECTED_TOKEN,                           \
    "Expected '%s' before '%s'." )                                          \
  BOND_PARSE_ERROR(CONTEXT_NODE, INVALID_TYPE_FOR_OPERATOR,                 \
    "Operator '%s' cannot be applied to type '%s'." )                       \


namespace Bond
{

class TextWriter;
class Token;
class ParseNode;

class ParseError
{
public:
	enum Type
	{
#define BOND_PARSE_ERROR(category, type, format) type,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR
	};

	enum Category
	{
		BASIC,
		CONTEXT,
		CONTEXT_LINE,
		STR_CONTEXT,
		CONTEXT_NODE,
	};

	ParseError(): mType(NO_ERROR), mContext(0) {}

	ParseError(Type type, const Token *context, const char *arg):
		mType(type),
		mContext(context),
		mArg(arg)
	{}

	ParseError(Type type, const Token *context, const Token *arg):
		mType(type),
		mContext(context),
		mArg(arg)
	{}

	ParseError(Type type, const Token *context, const ParseNode *arg):
		mType(type),
		mContext(context),
		mArg(arg)
	{}

	ParseError(const ParseError &other):
		mType(other.mType),
		mContext(other.mContext),
		mArg(other.mArg)
	{}

	Type GetType() const { return mType; }
	const Token *GetContext() const { return mContext; }
	const char *GetStringArg() const { return static_cast<const char *>(mArg); }
	const Token *GetTokenArg() const { return static_cast<const Token *>(mArg); }
	const ParseNode *GetParseNodeArg() const { return static_cast<const ParseNode *>(mArg); }

	Category GetCategory() const;
	static Category GetCategory(Type type);

	const char *GetFormat() const;
	static const char *GetFormat(Type type);

	void Print(TextWriter &writer) const;

private:
	Type mType;
	const Token *mContext;
	const void *mArg;
};


class ParseErrorBuffer
{
public:
	ParseErrorBuffer();

	void Reset();

	void PushError(ParseError::Type type, const Token *context, const char *arg);
	void PushError(ParseError::Type type, const Token *context, const Token *arg);
	void PushError(ParseError::Type type, const Token *context, const ParseNode *arg);

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
