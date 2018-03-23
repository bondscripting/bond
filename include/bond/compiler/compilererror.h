#ifndef BOND_COMPILER_COMPILERERROR_H
#define BOND_COMPILER_COMPILERERROR_H

#include "bond/types/types.h"

#define BOND_COMPILER_ERROR_LIST                                                           \
  BOND_COMPILER_ERROR(NO_ERROR,                                                            \
    "")                                                                                    \
  BOND_COMPILER_ERROR(INVALID_ESCAPE,                                                      \
    "Invalid escape sequence in token %c.")                                                \
  BOND_COMPILER_ERROR(MISSING_HEX_ESCAPE,                                                  \
    "\\x used with no following hex digits.")                                              \
  BOND_COMPILER_ERROR(OCTAL_ESCAPE_RANGE,                                                  \
    "Octal escape sequence out of range.")                                                 \
  BOND_COMPILER_ERROR(HEX_ESCAPE_RANGE,                                                    \
    "Hex escape sequence out of range.")                                                   \
  BOND_COMPILER_ERROR(INVALID_OCTAL_INT,                                                   \
    "Invalid octal integer constant '%c'.")                                                \
  BOND_COMPILER_ERROR(INVALID_HEX_INT,                                                     \
    "Invalid hexadecimal integer constant '%c'.")                                          \
  BOND_COMPILER_ERROR(INVALID_INT,                                                         \
    "Invalid integer constant '%c'.")                                                      \
  BOND_COMPILER_ERROR(INVALID_FLOAT,                                                       \
    "Invalid floating point constant '%c'.")                                               \
  BOND_COMPILER_ERROR(EMPTY_CHARACTER_CONSTANT,                                            \
    "Empty character constant.")                                                           \
  BOND_COMPILER_ERROR(MULTI_CHARACTER_CONSTANT,                                            \
    "Multi-character constant %c.")                                                        \
  BOND_COMPILER_ERROR(UNTERMINATED_CHARACTER,                                              \
    "Unterminated character constant.")                                                    \
  BOND_COMPILER_ERROR(UNTERMINATED_STRING,                                                 \
    "Unterminated string literal.")                                                        \
  BOND_COMPILER_ERROR(UNTERMINATED_COMMENT,                                                \
    "Unterminated comment.")                                                               \
  BOND_COMPILER_ERROR(DUPLICATE_CONST,                                                     \
    "Duplicate 'const' keyword.")                                                          \
  BOND_COMPILER_ERROR(PARSE_ERROR,                                                         \
    "Parse error near '%c'.")                                                              \
  BOND_COMPILER_ERROR(SIZE_AND_ALIGNMENT_NOT_ALLOWED,                                      \
    "Size and allignment specification only allowed on native structs.")                   \
  BOND_COMPILER_ERROR(VOID_NOT_ALLOWED,                                                    \
    "Invalid use of 'void'.")                                                              \
  BOND_COMPILER_ERROR(ARRAY_OF_VOID,                                                       \
    "Cannot specify an array of void.")                                                    \
  BOND_COMPILER_ERROR(MULTIDIMENTIONAL_ARRAY_BOUNDS,                                       \
    "Multidimensional array must have bounds for all dimensions except the first.")        \
  BOND_COMPILER_ERROR(INITIALIZER_NOT_ALLOWED,                                             \
    "Initializer not allowed near '%c'.")                                                  \
  BOND_COMPILER_ERROR(CONST_NON_MEMBER_FUNCTION,                                           \
    "Non-member function '%t' cannot be const.")                                           \
  BOND_COMPILER_ERROR(NATIVE_FUNCTION_DEFINITION,                                          \
    "Native function '%c' cannot be defined.")                                             \
  BOND_COMPILER_ERROR(NON_NATIVE_FUNCTION_DECLARATION,                                     \
    "Non-native function '%c' must be defined.")                                           \
  BOND_COMPILER_ERROR(FUNCTION_RETURNS_ARRAY,                                              \
    "Function '%c' returns an array.")                                                     \
  BOND_COMPILER_ERROR(EMPTY_SWITCH_STATEMENT,                                              \
    "Empty switch statement.")                                                             \
  BOND_COMPILER_ERROR(EMPTY_SWITCH_LABEL_LIST,                                             \
    "Empty switch label list.")                                                            \
  BOND_COMPILER_ERROR(UNTERMINATED_SWITCH_SECTION,                                         \
    "Switch section does not end with a jump statement.")                                  \
  BOND_COMPILER_ERROR(DUPLICATE_DEFAULT_IN_SWITCH,                                         \
    "Switch statement contains duplicate default label.")                                  \
  BOND_COMPILER_ERROR(DUPLICATE_CASE_IN_SWITCH,                                            \
    "Switch statement contains duplicate case '%d'.")                                      \
  BOND_COMPILER_ERROR(INVALID_OPERATOR_IN_CONST_EXPRESSION,                                \
    "Operator '%c' not allowed in constant expression.")                                   \
  BOND_COMPILER_ERROR(FUNCTION_CALL_IN_CONST_EXPRESSION,                                   \
    "Function call in constant expression near '%c'.")                                     \
  BOND_COMPILER_ERROR(DUPLICATE_SYMBOL,                                                    \
    "Duplicate symbol '%c' previously defined on line %l of '%f'.")                        \
  BOND_COMPILER_ERROR(UNEXPECTED_TOKEN,                                                    \
    "Expected '%s' before '%c'.")                                                          \
  BOND_COMPILER_ERROR(SYMBOL_IS_NOT_DEFINED,                                               \
    "Symbol '%n' is not defined.")                                                         \
  BOND_COMPILER_ERROR(NATIVE_STRUCT_IS_NOT_DEFINED,                                        \
    "Native struct '%c' is not defined.")                                                  \
  BOND_COMPILER_ERROR(DUPLICATE_NATIVE_STRUCT_STUB,                                        \
    "Duplicate native struct size specifier '%c' previously defined on line %l of '%f'.")  \
  BOND_COMPILER_ERROR(SYMBOL_IS_NOT_A_TYPE,                                                \
    "Symbol '%n' is not a type.")                                                          \
  BOND_COMPILER_ERROR(INVALID_SYMBOL_IN_EXPRESSION,                                        \
    "Symbol '%n' cannot be used in an expression.")                                        \
  BOND_COMPILER_ERROR(NON_STRUCT_MEMBER_REQUEST,                                           \
    "Request for member '%c' in non-struct type '%n'.")                                    \
  BOND_COMPILER_ERROR(INVALID_MEMBER_REQUEST,                                              \
    "Struct '%n' has no member named '%c'.")                                               \
  BOND_COMPILER_ERROR(NON_CONST_MEMBER_FUNCTION_REQUEST,                                   \
    "Cannot access '%0n' from '%1n'.")                                                     \
  BOND_COMPILER_ERROR(THIS_IN_NON_MEMBER_FUNCTION,                                         \
    "Invalid use of 'this' in non-member function.")                                       \
  BOND_COMPILER_ERROR(EXPRESSION_IS_NOT_CALLABLE,                                          \
    "Expression is not callable.")                                                         \
  BOND_COMPILER_ERROR(INCORRECT_NUMBER_OF_ARGS,                                            \
    "Incorrect number of arguments to function '%n'.")                                     \
  BOND_COMPILER_ERROR(INVALID_TYPE_FOR_OPERATOR,                                           \
    "Operator '%c' cannot be applied to type '%n'.")                                       \
  BOND_COMPILER_ERROR(INVALID_TYPE_FOR_POINTER_OPERATOR,                                   \
    "Operator '%c' cannot be applied to non-pointer type '%n'.")                           \
  BOND_COMPILER_ERROR(INVALID_TYPE_FOR_INDEX_OPERATOR,                                     \
    "Index operator cannot be applied with non-integer operand '%n'.")                     \
  BOND_COMPILER_ERROR(INVALID_TYPE_CONVERSION,                                             \
    "Cannot convert type '%0n' to '%1n'.")                                                 \
  BOND_COMPILER_ERROR(INVALID_RETURN_TYPE_CONVERSION,                                      \
    "Cannot convert type '%0n' to '%1n' in return.")                                       \
  BOND_COMPILER_ERROR(INVALID_TYPE_ASSIGNMENT,                                             \
    "Cannot assign type '%0n' to '%1n'.")                                                  \
  BOND_COMPILER_ERROR(ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER,                               \
    "Enumerator value for '%t' is not a 32 bit integer constant.")                         \
  BOND_COMPILER_ERROR(ARRAY_SIZE_IS_NOT_CONST_INTEGER,                                     \
    "Array size is not an integer constant.")                                              \
  BOND_COMPILER_ERROR(ARRAY_SIZE_IS_ZERO,                                                  \
    "Array size cannot be zero.")                                                          \
  BOND_COMPILER_ERROR(ARRAY_SIZE_IS_UNSPECIFIED,                                           \
    "Array size must be specified in type '%n'.")                                          \
  BOND_COMPILER_ERROR(SWITCH_CONTROL_IS_NOT_INTEGER,                                       \
    "Switch control is not a 32 bit integer.")                                             \
  BOND_COMPILER_ERROR(SWITCH_LABEL_IS_NOT_CONST_INTEGER,                                   \
    "Case label is not a 32 bit integer constant.")                                        \
  BOND_COMPILER_ERROR(IF_CONDITION_IS_NOT_BOOLEAN,                                         \
    "If statement condition is not boolean.")                                              \
  BOND_COMPILER_ERROR(WHILE_CONDITION_IS_NOT_BOOLEAN,                                      \
    "While statement condition is not boolean.")                                           \
  BOND_COMPILER_ERROR(FOR_CONDITION_IS_NOT_BOOLEAN,                                        \
    "For statement condition is not boolean.")                                             \
  BOND_COMPILER_ERROR(TERNARY_OPERAND_TYPE_MISMATCH,                                       \
    "Operands to ?: have different types '%0n' and '%1n'.")                                \
  BOND_COMPILER_ERROR(UNINITIALIZED_CONST,                                                 \
    "Uninitialized const '%c'.")                                                           \
  BOND_COMPILER_ERROR(NON_CONST_DECLARATION,                                               \
    "Declaration of '%c' must be const.")                                                  \
  BOND_COMPILER_ERROR(NON_LVALUE_TYPE,                                                     \
    "Cannot apply operator '%c' to non lvalue type '%n'.")                                 \
  BOND_COMPILER_ERROR(NON_LVALUE_ASSIGNMENT,                                               \
    "Cannot assign to non l-value near operator '%c'.")                                    \
  BOND_COMPILER_ERROR(UNASSIGNABLE_TYPE,                                                   \
    "Cannot assign to type '%n' near operator '%c'.")                                      \
  BOND_COMPILER_ERROR(VOID_POINTER_DEREFERENCE,                                            \
    "Cannot dereference pointer of type '%n'.")                                            \
  BOND_COMPILER_ERROR(INVALID_COMPARISON,                                                  \
    "Operator '%c' cannot compare type '%0n' with type '%1n'.")                            \
  BOND_COMPILER_ERROR(TOO_MANY_INITIALIZERS,                                               \
    "Too many initializers for type '%n'.")                                                \
  BOND_COMPILER_ERROR(CANNOT_INITIALIZE_NATIVE_TYPE_WITH_INITIALIZER_LIST,                 \
    "Cannot initialize native type '%n' with initializer list. Must call a function that returns an instance of that type.") \
  BOND_COMPILER_ERROR(MISSING_BRACES_IN_INITIALIZER,                                       \
    "Missing braces in initializer for type '%n'.")                                        \
  BOND_COMPILER_ERROR(BRACES_AROUND_SCALAR_INITIALIZER,                                    \
    "Braces around initializer for scalar type '%n'.")                                     \
  BOND_COMPILER_ERROR(RECURSIVE_STRUCT,                                                    \
    "Struct '%c' is recursive.")                                                           \
  BOND_COMPILER_ERROR(CANNOT_RESOLVE_SYMBOL_VALUE,                                         \
    "Cannot resolve the value of symbol '%c'.")                                            \
  BOND_COMPILER_ERROR(INVALID_STRUCT_SIZE,                                                 \
    "Struct size must be greater than zero.")                                              \
  BOND_COMPILER_ERROR(INVALID_STRUCT_ALIGNMENT,                                            \
    "Struct alignment must be a power of 2 greater than or equal to 1.")                   \
  BOND_COMPILER_ERROR(STRUCT_SIZE_ALIGNMENT_MISMATCH,                                      \
    "Struct size must be a multiple of alignment.")                                        \
  BOND_COMPILER_ERROR(DECLARATION_OF_UNDEFINED_SIZE,                                       \
    "Cannot declare '%c' with type '%n' of undefined size.")                               \
  BOND_COMPILER_ERROR(RETURN_OF_UNDEFINED_SIZE,                                            \
    "Function '%c' cannot return type '%n' of undefined size.")                            \
  BOND_COMPILER_ERROR(ASSIGNMENT_OF_UNDEFINED_SIZE,                                        \
    "Cannot assign type '%n' of undefined size.")                                          \
  BOND_COMPILER_ERROR(POINTER_ARITHMETIC_OF_UNDEFINED_SIZE,                                \
    "Cannot do pointer artithmetic on type '%n' of undefined size.")                       \
  BOND_COMPILER_ERROR(NOT_ALL_PATHS_RETURN_A_VALUE,                                        \
    "Not all paths return a value in function '%c'.")                                      \
  BOND_COMPILER_ERROR(INVALID_BREAK,                                                       \
    "Break statement not within a loop or switch statement.")                              \
  BOND_COMPILER_ERROR(INVALID_CONTINUE,                                                    \
    "Continue statement not within a loop.")                                               \
  BOND_COMPILER_ERROR(UNREACHABLE_CODE,                                                    \
    "Unreachable code.")                                                                   \
  BOND_COMPILER_ERROR(LOSSY_CONVERSION,                                                    \
    "Possible loss of data in conversion from '%0n' to '%1n'.")                            \
  BOND_COMPILER_ERROR(VALUE32_TABLE_OVERFLOW,                                              \
    "32-bit constant table overflow.")                                                     \
  BOND_COMPILER_ERROR(VALUE64_TABLE_OVERFLOW,                                              \
    "64-bit constant table overflow.")                                                     \
  BOND_COMPILER_ERROR(STRING_TABLE_OVERFLOW,                                               \
    "String table overflow.")                                                              \
  BOND_COMPILER_ERROR(QUALIFIED_NAME_TABLE_OVERFLOW,                                       \
    "Qualified name table overflow.")                                                      \
  BOND_COMPILER_ERROR(STRING_OVERFLOW,                                                     \
    "String overflow.")                                                                    \
  BOND_COMPILER_ERROR(INTERNAL_ERROR,                                                      \
    "Internal compiler error.")                                                            \


namespace Bond
{

/// \addtogroup compiler
/// @{

class OutputStream;
class Token;
class ParseNode;

/// \brief A container for error information reported by the various components of the Bond
/// compiler.
///
/// A CompilerError contains information regarding an error reported by some component of the
/// Bond compiler, namely the Lexer, Parser, SemanticAnalyzer and CodeGenerator. The information
/// includes the type of error, a token corresponding to the location in the source code where
/// the error was detected as well as a couple of other arguments giving contextual information
/// about the nature of the error.
///
/// CompilerError instances are seldom manipulated individually, since the compiler populates
/// a CompilerErrorBuffer which implements functionality for dumping out information about
/// all errors it contains.
///
/// \sa CodeGenerator, CompilerErrorBuffer, Lexer, Parser, SemanticAnalyzer
class CompilerError
{
public:
	/// \brief An enumeration identifying all types of compiler errors.
	enum Type
	{
#define BOND_COMPILER_ERROR(type, format) type,
		BOND_COMPILER_ERROR_LIST
#undef BOND_COMPILER_ERROR
	};

	/// \brief Default constructs an empty CompilerError object with the `NO_ERROR` error type.
	CompilerError(): mType(NO_ERROR), mContext(nullptr), mArg0(0), mArg1(0) {}

	/// \brief Constructs a CompilerError object with the given type and arguments supplied by the
	/// compiler to format the error message.
	/// \param type An enumerator identifying what type of error the compiler has detected.
	/// \param context A Token identifying where in the source code the error was detected.
	/// \param arg0 An argument supplying information to be inserted into the format string for the
	/// error message. Can be an integer, a C-style string, a pointer to a Token, or a pointer to
	/// a ParseNode depending on the requirements of the error message.
	/// \param arg1 An additional argument used in the same way as arg0.
	CompilerError(Type type, const Token *context, intptr_t arg0, intptr_t arg1):
		mType(type),
		mContext(context),
		mArg0(arg0),
		mArg1(arg1)
	{}

	/// \brief Returns the error's type.
	Type GetType() const { return mType; }

	/// \brief Returns the context Token providing the location of the error in the Bond source.
	const Token *GetContext() const { return mContext; }

	/// \brief Returns the error message format string for the CompilerError's type.
	const char *GetFormat() const;

	/// \brief Returns the error message format string for the given error type.
	static const char *GetFormat(Type type);

	/// \brief Returns a string representation of the CompilerError's type.
	const char *GetErrorName() const;

	/// \brief Returns a string representation of the given error type.
	static const char *GetErrorName(Type type);

	/// \brief Prints a formatted error message to the given OutputStream.
	/// \param stream The OutputStream to which the error message is printed.
	void Print(OutputStream &stream) const;

private:
	Type mType;
	const Token *mContext;
	intptr_t mArg0;
	intptr_t mArg1;
};


/// \brief A container for CompilerError instances generated by the various components of the
/// Bond compiler.
///
/// A CompilerErrorBuffer is used by the Lexer, Parser, SemanticAnalyzer and CodeGenerator of
/// the Bond compiler to collect the errors that they detect. The CompilerErrorBuffer provides
/// functionality for printing formatted error messages to an OutputStream.
///
/// The size of the buffer is fixed, and no memory allocations are incurred. If the buffer
/// overflows, any additional errors are dropped since they are not considered relevant because
/// they are most likely the consequence of the compiler being confused by the initial set of
/// errors that caused the buffer to fill up.
///
/// \sa CodeGenerator, CompilerError, Lexer, Parser, SemanticAnalyzer
class CompilerErrorBuffer
{
public:
	/// \brief Default constructs an empty CompilerErrorBuffer object.
	CompilerErrorBuffer();

	/// \brief Clears the contents of the error buffer.
	void Reset();

	/// \brief Constructs a CompilerError object with the given type and arguments to format the
	/// error message and adds it to the buffer.
	/// \sa CompilerError::CompilerError.
	void PushError(CompilerError::Type type, const Token *context = nullptr, const void *arg0 = nullptr, const void *arg1 = nullptr);

	/// \brief Constructs a CompilerError object with the given type and arguments to format the
	/// error message and adds it to the buffer.
	/// \sa CompilerError::CompilerError.
	void PushErrorInt(CompilerError::Type type, const Token *context = nullptr, intptr_t arg0 = 0, intptr_t arg1 = 0);

	/// \brief Returns whether the buffer contains any errors.
	bool HasErrors() const { return mNumErrors > 0; }

	/// \brief Returns the number of errors in the buffer.
	size_t GetNumErrors() const { return mNumErrors; }

	/// \brief Returns the CompilerError at the specified index in the buffer.
	/// \param index The index of the error to be retrieved.
	const CompilerError &GetError(size_t index) const { return mErrors[index]; }

	/// \brief Appends the contents of the given CompilerErrorBuffer to this one.
	/// \param other The CompilerErrorBuffer whose contents are appended.
	void CopyFrom(const CompilerErrorBuffer &other);

	/// \brief Prints a formatted error message for each error to the given OutputStream.
	/// \param stream The OutputStream to which the error messages are printed.
	void Print(OutputStream &stream) const;

private:
	static const size_t MAX_ERRORS = 64;

	CompilerError mErrors[MAX_ERRORS];
	size_t mNumErrors;
};

/// @}

}

#endif
