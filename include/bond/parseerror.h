#ifndef BOND_PARSEERROR_H
#define BOND_PARSEERROR_H

#define BOND_PARSE_ERROR_LIST                                                       \
  BOND_PARSE_ERROR(NO_ERROR,                                                        \
    "")                                                                             \
  BOND_PARSE_ERROR(DUPLICATE_CONST,                                                 \
    "Duplicate 'const' keyword.")                                                   \
  BOND_PARSE_ERROR(PARSE_ERROR,                                                     \
    "Parse error near '%c'.")                                                       \
  BOND_PARSE_ERROR(MULTIDIMENTIONAL_ARRAY_BOUNDS,                                   \
    "Multidimensional array must have bounds for all dimensions except the first.") \
  BOND_PARSE_ERROR(FUNCTION_DEFINITION_NOT_ALLOWED,                                 \
    "Function definition not allowed near '%c'.")                                   \
  BOND_PARSE_ERROR(INITIALIZER_NOT_ALLOWED,                                         \
    "Initializer not allowed near '%c'.")                                           \
  BOND_PARSE_ERROR(INVALID_OPERATOR_IN_CONST_EXPRESSION,                            \
    "Operator '%c' not allowed in constant expression.")                            \
  BOND_PARSE_ERROR(FUNCTION_CALL_IN_CONST_EXPRESSION,                               \
    "Function call in constant expression near '%c'.")                              \
  BOND_PARSE_ERROR(DUPLICATE_SYMBOL,                                                \
    "Duplicate symbol '%c' previously defined on line '%l'.")                       \
  BOND_PARSE_ERROR(DUPLICATE_FUNCTION_DEFINITION,                                   \
    "Duplicate function definition '%c' previously defined on line '%l'.")          \
  BOND_PARSE_ERROR(FUNCTION_PROTOTYPE_MISMATCH,                                     \
    "Mismatching function prototype '%c' previously defined on line '%l'.")         \
  BOND_PARSE_ERROR(UNEXPECTED_TOKEN,                                                \
    "Expected '%s' before '%c'.")                                                   \
  BOND_PARSE_ERROR(SYMBOL_IS_NOT_DEFINED,                                           \
    "Symbol '%n' is not defined.")                                                  \
  BOND_PARSE_ERROR(SYMBOL_IS_NOT_A_TYPE,                                            \
    "Symbol '%n' is not a type.")                                                   \
  BOND_PARSE_ERROR(INVALID_SYMBOL_IN_EXPRESSION,                                    \
    "Symbol '%n' cannot be used in an expression.")                                 \
  BOND_PARSE_ERROR(NON_STRUCT_MEMBER_REQUEST,                                       \
    "Request for member '%c' in non-struct type '%n'.")                             \
  BOND_PARSE_ERROR(INVALID_MEMBER_REQUEST,                                          \
    "Struct '%n' has no member named '%c'.")                                        \
  BOND_PARSE_ERROR(EXPRESSION_IS_NOT_CALLABLE,                                      \
    "Expression is not callable.")                                                  \
  BOND_PARSE_ERROR(INCORRECT_NUMBER_OF_ARGS,                                        \
    "Incorrect number of arguments to function '%n'.")                              \
  BOND_PARSE_ERROR(INVALID_TYPE_FOR_OPERATOR,                                       \
    "Operator '%c' cannot be applied to type '%n'.")                                \
  BOND_PARSE_ERROR(INVALID_TYPE_FOR_POINTER_OPERATOR,                               \
    "Operator '%c' cannot be applied to non-pointer type '%n'.")                    \
  BOND_PARSE_ERROR(INVALID_TYPE_FOR_INDEX_OPERATOR,                                 \
    "Index operator cannot be applied to non-integer type '%n'.")                   \
  BOND_PARSE_ERROR(INVALID_TYPE_CONVERSION,                                         \
    "Cannot convert type '%0n' to '%1n'.")                                          \
  BOND_PARSE_ERROR(INVALID_TYPE_ASSIGNMENT,                                         \
    "Cannot assign type '%0n' to '%1n'.")                                           \
  BOND_PARSE_ERROR(ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER,                           \
    "Enumerator value for '%t' is not an integer constant.")                        \
  BOND_PARSE_ERROR(ARRAY_SIZE_IS_NOT_CONST_INTEGER,                                 \
    "Array size is not an integer constant.")                                       \
  BOND_PARSE_ERROR(ARRAY_SIZE_IS_ZERO,                                              \
    "Array size cannot be zero.")                                                   \
  BOND_PARSE_ERROR(ARRAY_SIZE_IS_UNSPECIFIED,                                       \
    "Array size must be specified in type '%n'.")                                   \
  BOND_PARSE_ERROR(SWITCH_CONTROL_IS_NOT_INTEGER,                                   \
    "Switch control is not an integer.")                                            \
  BOND_PARSE_ERROR(SWITCH_LABEL_IS_NOT_CONST_INTEGER,                               \
    "Case label is not an integer constant.")                                       \
  BOND_PARSE_ERROR(IF_CONDITION_IS_NOT_BOOLEAN,                                     \
    "If statement condition is not boolean.")                                       \
  BOND_PARSE_ERROR(WHILE_CONDITION_IS_NOT_BOOLEAN,                                  \
    "While statement condition is not boolean.")                                    \
  BOND_PARSE_ERROR(FOR_CONDITION_IS_NOT_BOOLEAN,                                    \
    "For statement condition is not boolean.")                                      \
  BOND_PARSE_ERROR(TERNARY_OPERAND_TYPE_MISMATCH,                                   \
    "Operands to ?: have different types '%0n' and '%1n'.")                         \
  BOND_PARSE_ERROR(UNINITIALIZED_CONST,                                             \
    "Uninitialized const '%c'.")                                                    \
  BOND_PARSE_ERROR(NON_CONST_DECLARATION,                                           \
    "Declaration of '%c' must be const.")                                           \
  BOND_PARSE_ERROR(NON_LVALUE_TYPE,                                                 \
    "Cannot apply operator '%c' to non lvalue type '%n'.")                          \
  BOND_PARSE_ERROR(NON_LVALUE_ASSIGNMENT,                                           \
    "Cannot assign to non l-value near operator '%c'.")                             \
  BOND_PARSE_ERROR(UNASSIGNABLE_TYPE,                                               \
    "Cannot assign to type '%n' near operator '%c'.")                               \
  BOND_PARSE_ERROR(INVALID_COMPARISON,                                              \
    "Operator '%c' cannot compare type '%0n' with type '%1n'.")                     \
  BOND_PARSE_ERROR(MISSING_BRACES_IN_INITIALIZER,                                   \
    "Missing braces in initializer for type '%n'.")                                 \
  BOND_PARSE_ERROR(BRACES_AROUND_SCALAR_INITIALIZER,                                \
    "Braces around initializer for scalar type '%n'.")                              \


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

	const char *GetErrorName() const;
	static const char *GetErrorName(Type type);

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
	static const int MAX_ERRORS = 64;

	ParseError mErrors[MAX_ERRORS];
	int mNumErrors;
};

}

#endif
