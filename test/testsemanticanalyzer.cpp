#include "framework/asserts.h"
#include "framework/testsemanticanalyzerframework.h"
#include "framework/testparserframework.h"
#include "bond/compiler/parsenodeutil.h"

DEFINE_SEMANTICANALYZER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *outerSpace = globalScope->FindSymbol("OuterSpace");
	ASSERT_MESSAGE(outerSpace != nullptr, "Failed to find symbol 'OuterSpace'.");
	ASSERT_MESSAGE(outerSpace->GetSymbolType() == Bond::Symbol::TYPE_NAMESPACE, "Expected 'OuterSpace' to be a namespace.");

	const Bond::Symbol *firstInnerSpace = outerSpace->FindSymbol("FirstInnerSpace");
	ASSERT_MESSAGE(firstInnerSpace != nullptr, "Failed to find symbol 'FirstInnerSpace'.");

	const Bond::Symbol *secondInnerSpace = outerSpace->FindSymbol("SecondInnerSpace");
	ASSERT_MESSAGE(secondInnerSpace != nullptr, "Failed to find symbol 'SecondInnerSpace'.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *empty = globalScope->FindSymbol("Empty");
	ASSERT_MESSAGE(empty != nullptr, "Failed to find symbol 'Empty'.");
	ASSERT_MESSAGE(empty->GetSymbolType() == Bond::Symbol::TYPE_ENUM, "Expected 'Empty' to be an enum.");

	const Bond::Symbol *eighth = globalScope->FindSymbol("EIGHTH");
	ASSERT_MESSAGE(eighth != nullptr, "Failed to find symbol 'EIGHTH'.");
	ASSERT_MESSAGE(eighth->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'EIGHTH' to be a value.");

	const Bond::Enumerator *enumerator = Bond::CastNode<Bond::Enumerator>(eighth);
	ASSERT_MESSAGE(enumerator != nullptr, "Expected 'EIGHTH' to be an enumerator.");

	const Bond::TypeAndValue *tav = enumerator->GetTypeAndValue();
	ASSERT_MESSAGE(tav->GetTypeDescriptor().GetPrimitiveType() == Bond::Token::KEY_INT,
		"Expected 'EIGHTH' to have an integer value.");
	ASSERT_MESSAGE(tav->GetIntValue() == 5, "Expected 'EIGHTH' to have the value 5");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *symbol = globalScope->FindSymbol("VoidOneParameter");
	ASSERT_MESSAGE(symbol != nullptr, "Failed to find symbol 'VoidOneParameter'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'VoidOneParameter' to be a function.");
	const Bond::FunctionDefinition *function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	ASSERT_MESSAGE(function->GetBody() == nullptr, "Expected 'VoidOneParameter' to have no body.");

	symbol = globalScope->FindSymbol("ComplexFunctionPrototype2");
	ASSERT_MESSAGE(symbol != nullptr, "Failed to find symbol 'ComplexFunctionPrototype2'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'ComplexFunctionPrototype2' to be a function.");
	function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	ASSERT_MESSAGE(function->GetBody() != nullptr, "Expected 'ComplexFunctionPrototype2' to have a body.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Initializers, "scripts/parser_Initializers.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *structSymbol = globalScope->FindSymbol("DataAndFunctions");
	ASSERT_MESSAGE(structSymbol != nullptr, "Failed to find symbol 'DataAndFunctions'.");
	ASSERT_MESSAGE(structSymbol->GetSymbolType() == Bond::Symbol::TYPE_STRUCT, "Expected 'DataAndFunctions' to be a struct.");

	const Bond::Symbol *memberSymbol = structSymbol->FindSymbol("x");
	ASSERT_MESSAGE(memberSymbol != nullptr, "Failed to find symbol 'x'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'x' to be a value.");

	memberSymbol = structSymbol->FindSymbol("DoStuff");
	ASSERT_MESSAGE(memberSymbol != nullptr, "Failed to find symbol 'DoStuff'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'DoStuff to be a function.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(WhileStatements, "scripts/parser_WhileStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ForStatements, "scripts/parser_ForStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(SwitchStatements, "scripts/parser_SwitchStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeSpecifierErrors, "scripts/sanalyzer_TypeSpecifierErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::SYMBOL_IS_NOT_A_TYPE, Bond::Token::IDENTIFIER, 5},
		{Bond::CompilerError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 6},
		{Bond::CompilerError::DUPLICATE_NATIVE_STRUCT_STUB, Bond::Token::IDENTIFIER, 14},
		{Bond::CompilerError::NATIVE_STRUCT_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 15},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors, "scripts/sanalyzer_TypeEvaluationErrors.bond")
{
	// Focus on expression type evaluation erros.
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 16},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 17},
		{Bond::CompilerError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 18},
		{Bond::CompilerError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 19},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 38},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 39},
		{Bond::CompilerError::INVALID_TYPE_ASSIGNMENT, Bond::Token::ASSIGN, 40},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_LEFT, 41},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_MOD, 42},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 43},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 44},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_DIV, 45},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_OR, 46},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_AMP, 47},
		{Bond::CompilerError::INVALID_COMPARISON, Bond::Token::OP_LT, 48},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_MINUS, 49},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_NOT, 50},
		{Bond::CompilerError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_STAR, 51},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_BIT_NOT, 52},
		{Bond::CompilerError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_INC, 53},
		{Bond::CompilerError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_ARROW, 54},
		{Bond::CompilerError::NON_STRUCT_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 55},
		{Bond::CompilerError::INVALID_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 56},
		{Bond::CompilerError::INVALID_TYPE_FOR_INDEX_OPERATOR, Bond::Token::OBRACKET, 57},
		{Bond::CompilerError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OBRACKET, 58},
		{Bond::CompilerError::EXPRESSION_IS_NOT_CALLABLE, Bond::Token::OPAREN, 59},
		{Bond::CompilerError::INVALID_TYPE_CONVERSION, Bond::Token::KEY_INT, 60},
		{Bond::CompilerError::INVALID_SYMBOL_IN_EXPRESSION, Bond::Token::IDENTIFIER, 61},
		{Bond::CompilerError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 62},
		{Bond::CompilerError::TERNARY_OPERAND_TYPE_MISMATCH, Bond::Token::OP_TERNARY, 63},
		{Bond::CompilerError::NON_LVALUE_TYPE, Bond::Token::OP_AMP, 64},
		{Bond::CompilerError::NON_LVALUE_ASSIGNMENT, Bond::Token::ASSIGN, 64},
		{Bond::CompilerError::VOID_POINTER_DEREFERENCE, Bond::Token::OP_STAR, 65},
		{Bond::CompilerError::UNINITIALIZED_CONST, Bond::Token::IDENTIFIER, 67},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 68},
		{Bond::CompilerError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 77},
		{Bond::CompilerError::INCORRECT_NUMBER_OF_ARGS, Bond::Token::OPAREN, 78},
		{Bond::CompilerError::INVALID_TYPE_CONVERSION, Bond::Token::IDENTIFIER, 79},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 80},
		{Bond::CompilerError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 81},
		{Bond::CompilerError::INVALID_TYPE_CONVERSION, Bond::Token::IDENTIFIER, 84},
		{Bond::CompilerError::NON_LVALUE_TYPE, Bond::Token::OP_AMP, 86},
		{Bond::CompilerError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 88},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors2, "scripts/sanalyzer_TypeEvaluationErrors2.bond")
{
	// Focus on remaining type evaluation erros.
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 3},
		{Bond::CompilerError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 4},
		{Bond::CompilerError::NON_CONST_DECLARATION, Bond::Token::IDENTIFIER, 16},
		{Bond::CompilerError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 18},
		{Bond::CompilerError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 19},
		{Bond::CompilerError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 20},
		{Bond::CompilerError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 42},
		{Bond::CompilerError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 44},
		{Bond::CompilerError::SWITCH_CONTROL_IS_NOT_INTEGER, Bond::Token::OP_LT, 40},
		{Bond::CompilerError::IF_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 48},
		{Bond::CompilerError::WHILE_CONDITION_IS_NOT_BOOLEAN, Bond::Token::ASSIGN_MINUS, 49},
		{Bond::CompilerError::FOR_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 50},
		{Bond::CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::OP_INC, 52},
		{Bond::CompilerError::FUNCTION_CALL_IN_CONST_EXPRESSION, Bond::Token::OPAREN, 52},
		{Bond::CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::COMMA, 52},
		{Bond::CompilerError::MISSING_BRACES_IN_INITIALIZER, Bond::Token::CONST_INT, 53},
		{Bond::CompilerError::MISSING_BRACES_IN_INITIALIZER, Bond::Token::CONST_INT, 53},
		{Bond::CompilerError::BRACES_AROUND_SCALAR_INITIALIZER, Bond::Token::OBRACE, 54},
		{Bond::CompilerError::INVALID_TYPE_CONVERSION, Bond::Token::CONST_BOOL, 55},
		{Bond::CompilerError::BRACES_AROUND_SCALAR_INITIALIZER, Bond::Token::OBRACE, 56},
		{Bond::CompilerError::THIS_IN_NON_MEMBER_FUNCTION, Bond::Token::KEY_THIS, 57},
		{Bond::CompilerError::CANNOT_INITIALIZE_NATIVE_TYPE_WITH_INITIALIZER_LIST, Bond::Token::OBRACE, 58},
		{Bond::CompilerError::TOO_MANY_INITIALIZERS, Bond::Token::CONST_INT, 60},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValueEvaluationErrors, "scripts/sanalyzer_ValueEvaluationErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::INVALID_STRUCT_SIZE, Bond::Token::CONST_INT, 3},
		{Bond::CompilerError::INVALID_STRUCT_ALIGNMENT, Bond::Token::CONST_INT, 4},
		{Bond::CompilerError::STRUCT_SIZE_ALIGNMENT_MISMATCH, Bond::Token::CONST_INT, 5},
		{Bond::CompilerError::ARRAY_SIZE_IS_ZERO, Bond::Token::IDENTIFIER, 12},
		{Bond::CompilerError::ARRAY_SIZE_IS_NOT_CONST_INTEGER, Bond::Token::IDENTIFIER, 13},
		{Bond::CompilerError::ARRAY_SIZE_IS_UNSPECIFIED, Bond::Token::KEY_INT, 14},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValueEvaluationErrors2, "scripts/sanalyzer_ValueEvaluationErrors2.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 5},
		{Bond::CompilerError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 10},
		{Bond::CompilerError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 14},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(LossyConversions1, "scripts/sanalyzer_LossyConversions1.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 4},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 5},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 7},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 8},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 9},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 11},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 12},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 13},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 15},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 16},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 17},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 19},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 20},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 22},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 26},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 27},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 28},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 30},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 31},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 32},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 34},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 35},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 37},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 38},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 39},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 41},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 42},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 44},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 45},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 47},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 48},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 50},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 51},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 52},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 54},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 55},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 57},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 58},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::CONST_UINT, 60},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 62},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 63},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 64},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 66},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 67},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 68},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 70},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 71},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 72},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 74},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 76},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 77},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 78},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 80},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 81},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 83},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 84},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(LossyConversions2, "scripts/sanalyzer_LossyConversions2.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::CONST_ULONG, 4},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 6},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 7},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 8},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 10},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 11},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 12},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::OP_MINUS, 14},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 16},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 17},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 19},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 20},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::CONST_UINT, 22},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 23},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 24},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 25},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 27},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 28},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::CONST_LONG, 30},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 31},
		{Bond::CompilerError::LOSSY_CONVERSION, Bond::Token::ASSIGN, 32},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValidationErrors, "scripts/sanalyzer_ValidationErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNTERMINATED_SWITCH_SECTION, Bond::Token::KEY_CASE, 5},
		{Bond::CompilerError::INVALID_BREAK, Bond::Token::KEY_BREAK, 17},
		{Bond::CompilerError::INVALID_CONTINUE, Bond::Token::KEY_CONTINUE, 21},
		{Bond::CompilerError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 25},
		{Bond::CompilerError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 37},
		{Bond::CompilerError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 46},
		{Bond::CompilerError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 68},
		{Bond::CompilerError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 81},
		{Bond::CompilerError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 94},
		{Bond::CompilerError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 109},
		{Bond::CompilerError::INVALID_RETURN_TYPE_CONVERSION, Bond::Token::KEY_RETURN, 115},
		{Bond::CompilerError::DECLARATION_OF_UNDEFINED_SIZE, Bond::Token::IDENTIFIER, 196},
		{Bond::CompilerError::RETURN_OF_UNDEFINED_SIZE, Bond::Token::IDENTIFIER, 198},
		{Bond::CompilerError::DECLARATION_OF_UNDEFINED_SIZE, Bond::Token::IDENTIFIER, 198},
		{Bond::CompilerError::DECLARATION_OF_UNDEFINED_SIZE, Bond::Token::IDENTIFIER, 200},
		{Bond::CompilerError::ASSIGNMENT_OF_UNDEFINED_SIZE, Bond::Token::ASSIGN, 201},
		{Bond::CompilerError::POINTER_ARITHMETIC_OF_UNDEFINED_SIZE, Bond::Token::OBRACKET, 201},
		{Bond::CompilerError::POINTER_ARITHMETIC_OF_UNDEFINED_SIZE, Bond::Token::OP_PLUS, 202},
		{Bond::CompilerError::DUPLICATE_DEFAULT_IN_SWITCH, Bond::Token::KEY_SWITCH, 207},
		{Bond::CompilerError::DUPLICATE_CASE_IN_SWITCH, Bond::Token::KEY_SWITCH, 218},
		{Bond::CompilerError::TOO_MANY_INITIALIZERS, Bond::Token::CONST_INT, 229},
		{Bond::CompilerError::TOO_MANY_INITIALIZERS, Bond::Token::CONST_STRING, 234},
		{Bond::CompilerError::INVALID_TYPE_ASSIGNMENT, Bond::Token::ASSIGN, 240},
	};
	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(IncludeLibraries, "scripts/sanalyzer_IncludeLibraries.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);
	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Namespaces)                         \
  TEST_ITEM(Enums)                              \
  TEST_ITEM(FunctionDefinitions)                \
  TEST_ITEM(Initializers)                       \
  TEST_ITEM(Structs)                            \
  TEST_ITEM(IfStatements)                       \
  TEST_ITEM(WhileStatements)                    \
  TEST_ITEM(ForStatements)                      \
  TEST_ITEM(SwitchStatements)                   \
  TEST_ITEM(TypeSpecifierErrors)                \
  TEST_ITEM(TypeEvaluationErrors)               \
  TEST_ITEM(TypeEvaluationErrors2)              \
  TEST_ITEM(ValueEvaluationErrors)              \
  TEST_ITEM(ValueEvaluationErrors2)             \
  TEST_ITEM(LossyConversions1)                  \
  TEST_ITEM(LossyConversions2)                  \
  TEST_ITEM(ValidationErrors)                   \
  TEST_ITEM(IncludeLibraries)                   \

RUN_TESTS(SemanticAnalyzer, TEST_ITEMS)
