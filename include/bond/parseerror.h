#ifndef BOND_PARSEERROR_H
#define BOND_PARSEERROR_H

#define BOND_PARSE_ERROR_LIST                                               \
  BOND_PARSE_ERROR(NO_ERROR,                                                \
    "")                                                                     \
  BOND_PARSE_ERROR(DUPLICATE_CONST,                                         \
    "Duplicate 'const' keyword.")                                           \
  BOND_PARSE_ERROR(PARSE_ERROR,                                             \
    "Parse error near '%c'.")                                               \
  BOND_PARSE_ERROR(COMMA_IN_CONST_EXPRESSION,                               \
    "Comma in constant expression near '%c'.")                              \
  BOND_PARSE_ERROR(ASSIGNMENT_IN_CONST_EXPRESSION,                          \
    "Assignment in constant expression at '%c'." )                          \
  BOND_PARSE_ERROR(INCREMENT_IN_CONST_EXPRESSION,                           \
    "'%c' operator in constant expression.")                                \
  BOND_PARSE_ERROR(FUNCTION_CALL_IN_CONST_EXPRESSION,                       \
    "Function call in constant expression near '%c'.")                      \
  BOND_PARSE_ERROR(FUNCTION_DEFINITION_NOT_ALLOWED,                         \
    "Function definition not allowed near '%c'.")                           \
  BOND_PARSE_ERROR(INITIALIZER_NOT_ALLOWED,                                 \
    "Initializer not allowed near '%c'.")                                   \
  BOND_PARSE_ERROR(DUPLICATE_SYMBOL,                                        \
    "Duplicate symbol '%c' previously defined on line '%l'.")               \
  BOND_PARSE_ERROR(DUPLICATE_FUNCTION_DEFINITION,                           \
    "Duplicate function definition '%c' previously defined on line '%l'.")  \
  BOND_PARSE_ERROR(FUNCTION_PROTOTYPE_MISMATCH,                             \
    "Mismatching function prototype '%c' previously defined on line '%l'.") \
  BOND_PARSE_ERROR(UNEXPECTED_TOKEN,                                        \
    "Expected '%s' before '%c'." )                                          \
  BOND_PARSE_ERROR(INVALID_TYPE_FOR_OPERATOR,                               \
    "Operator '%c' cannot be applied to type '%n'." )                       \
  BOND_PARSE_ERROR(INVALID_TYPE_FOR_POINTER_OPERATOR,                       \
    "Operator '%c' cannot be applied to non-pointer type '%n'." )           \
  BOND_PARSE_ERROR(INVALID_TYPE_CONVERSION,                                 \
    "Cannot convert type '%0n' to '%1n'." )                                 \
  BOND_PARSE_ERROR(UNASSIGNABLE_TYPE,                                       \
    "Cannot assign to type '%n' on lhs of '%c'." )                          \
  BOND_PARSE_ERROR(INVALID_COMPARISON,                                      \
    "Operator '%c' cannot compare type '%0n' with type '%1n'." )            \


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
#define BOND_PARSE_ERROR(type, format) type,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR
	};

	ParseError(): mType(NO_ERROR), mContext(0) {}

	ParseError(Type type, const Token *context, const void *arg0 = 0, const void *arg1 = 0):
		mType(type),
		mContext(context),
		mArg0(arg0),
		mArg1(arg1)
	{}

	ParseError(const ParseError &other):
		mType(other.mType),
		mContext(other.mContext),
		mArg0(other.mArg0),
		mArg1(other.mArg1)
	{}

	Type GetType() const { return mType; }
	const Token *GetContext() const { return mContext; }

	const char *GetFormat() const;
	static const char *GetFormat(Type type);

	void Print(TextWriter &writer) const;

private:
	Type mType;
	const Token *mContext;
	const void *mArg0;
	const void *mArg1;
};


class ParseErrorBuffer
{
public:
	ParseErrorBuffer();

	void Reset();

	void PushError(ParseError::Type type, const Token *context, const void *arg0 = 0, const void *arg1 = 0);
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
