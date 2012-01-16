#include "framework/testsemanticanalyzerframework.h"
#include "framework/testparserframework.h"
#include "bond/parsenodeutil.h"
#include "bond/stringutil.h"

DEFINE_SEMANTICANALYZER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *outerSpace = globalScope->FindSymbol("OuterSpace");
	ASSERT_MESSAGE(outerSpace != NULL, "Failed to find symbol 'OuterSpace'.");
	ASSERT_MESSAGE(outerSpace->GetSymbolType() == Bond::Symbol::TYPE_NAMESPACE, "Expected 'OuterSpace' to be a namespace.");

	const Bond::Symbol *firstInnerSpace = outerSpace->FindSymbol("FirstInnerSpace");
	ASSERT_MESSAGE(firstInnerSpace != NULL, "Failed to find symbol 'FirstInnerSpace'.");

	const Bond::Symbol *secondInnerSpace = outerSpace->FindSymbol("SecondInnerSpace");
	ASSERT_MESSAGE(secondInnerSpace != NULL, "Failed to find symbol 'SecondInnerSpace'.");

	const Bond::bu32_t actualHash = secondInnerSpace->GetGlobalHashCode();
	const Bond::bu32_t expectedHash = Bond::StringHash("::OuterSpace::SecondInnerSpace");
	ASSERT_FORMAT(actualHash == expectedHash, ("Expected 'SecondInnerSpace' global hash to be " BOND_UHEX_FORMAT ", but was " BOND_UHEX_FORMAT ".", expectedHash, actualHash));

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *empty = globalScope->FindSymbol("Empty");
	ASSERT_MESSAGE(empty != NULL, "Failed to find symbol 'Empty'.");
	ASSERT_MESSAGE(empty->GetSymbolType() == Bond::Symbol::TYPE_ENUM, "Expected 'Empty' to be an enum.");

	const Bond::Symbol *eighth = globalScope->FindSymbol("EIGHTH");
	ASSERT_MESSAGE(eighth != NULL, "Failed to find symbol 'EIGHTH'.");
	ASSERT_MESSAGE(eighth->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'EIGHTH' to be a value.");

	const Bond::Enumerator *enumerator = Bond::CastNode<Bond::Enumerator>(eighth);
	ASSERT_MESSAGE(enumerator != NULL, "Expected 'EIGHTH' to be an enumerator.");

	const Bond::TypeAndValue *tav = enumerator->GetTypeAndValue();
	ASSERT_MESSAGE(tav->GetTypeDescriptor()->GetPrimitiveType() == Bond::Token::KEY_INT,
		"Expected 'EIGHTH' to have an integer value.");
	ASSERT_MESSAGE(tav->GetIntValue() == 5, "Expected 'EIGHTH' to have the value 5");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *symbol = globalScope->FindSymbol("VoidOneParameter");
	ASSERT_MESSAGE(symbol != NULL, "Failed to find symbol 'VoidOneParameter'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'VoidOneParameter' to be a function.");
	const Bond::FunctionDefinition *function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	ASSERT_MESSAGE(function->GetBody() == NULL, "Expected 'VoidOneParameter' to have no body.");

	symbol = globalScope->FindSymbol("ComplexFunctionPrototype2");
	ASSERT_MESSAGE(symbol != NULL, "Failed to find symbol 'ComplexFunctionPrototype2'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'ComplexFunctionPrototype2' to be a function.");
	function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	ASSERT_MESSAGE(function->GetBody() != NULL, "Expected 'ComplexFunctionPrototype2' to have a body.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Initializers, "scripts/parser_Initializers.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *structSymbol = globalScope->FindSymbol("DataAndFunctions");
	ASSERT_MESSAGE(structSymbol != NULL, "Failed to find symbol 'DataAndFunctions'.");
	ASSERT_MESSAGE(structSymbol->GetSymbolType() == Bond::Symbol::TYPE_STRUCT, "Expected 'DataAndFunctions' to be a struct.");

	const Bond::Symbol *memberSymbol = structSymbol->FindSymbol("x");
	ASSERT_MESSAGE(memberSymbol != NULL, "Failed to find symbol 'x'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'x' to be a value.");

	memberSymbol = structSymbol->FindSymbol("DoStuff");
	ASSERT_MESSAGE(memberSymbol != NULL, "Failed to find symbol 'DoStuff'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'DoStuff to be a function.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(WhileStatements, "scripts/parser_WhileStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ForStatements, "scripts/parser_ForStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(SwitchStatements, "scripts/parser_SwitchStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(errorBuffer);
	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeSpecifierErrors, "scripts/sanalyzer_TypeSpecifierErrors.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::SYMBOL_IS_NOT_A_TYPE, Bond::Token::IDENTIFIER, 5},
		{Bond::ParseError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 6},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors, "scripts/sanalyzer_TypeEvaluationErrors.bond")
{
	// Focus on expression type evaluation erros.
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 16},
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 17},
		{Bond::ParseError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 18},
		{Bond::ParseError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 19},
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 38},
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 39},
		{Bond::ParseError::INVALID_TYPE_ASSIGNMENT, Bond::Token::ASSIGN, 40},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_LEFT, 41},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_MOD, 42},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 43},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 44},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_DIV, 45},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_OR, 46},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_AMP, 47},
		{Bond::ParseError::INVALID_COMPARISON, Bond::Token::OP_LT, 48},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_MINUS, 49},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_NOT, 50},
		{Bond::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_STAR, 51},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_BIT_NOT, 52},
		{Bond::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_INC, 53},
		{Bond::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_ARROW, 54},
		{Bond::ParseError::NON_STRUCT_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 55},
		{Bond::ParseError::INVALID_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 56},
		{Bond::ParseError::INVALID_TYPE_FOR_INDEX_OPERATOR, Bond::Token::OBRACKET, 57},
		{Bond::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OBRACKET, 58},
		{Bond::ParseError::EXPRESSION_IS_NOT_CALLABLE, Bond::Token::OPAREN, 59},
		{Bond::ParseError::INVALID_TYPE_CONVERSION, Bond::Token::KEY_INT, 60},
		{Bond::ParseError::INVALID_SYMBOL_IN_EXPRESSION, Bond::Token::IDENTIFIER, 61},
		{Bond::ParseError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 62},
		{Bond::ParseError::TERNARY_OPERAND_TYPE_MISMATCH, Bond::Token::OP_TERNARY, 63},
		{Bond::ParseError::NON_LVALUE_TYPE, Bond::Token::OP_AMP, 64},
		{Bond::ParseError::NON_LVALUE_ASSIGNMENT, Bond::Token::ASSIGN, 64},
		{Bond::ParseError::VOID_POINTER_DEREFERENCE, Bond::Token::OP_STAR, 65},
		{Bond::ParseError::UNINITIALIZED_CONST, Bond::Token::IDENTIFIER, 67},
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 68},
		{Bond::ParseError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 77},
		{Bond::ParseError::INCORRECT_NUMBER_OF_ARGS, Bond::Token::OPAREN, 78},
		{Bond::ParseError::INVALID_TYPE_CONVERSION, Bond::Token::IDENTIFIER, 79},
		{Bond::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 80},
		{Bond::ParseError::NON_CONST_MEMBER_FUNCTION_REQUEST, Bond::Token::IDENTIFIER, 81},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors2, "scripts/sanalyzer_TypeEvaluationErrors2.bond")
{
	// Focus on remaining type evaluation erros.
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 3},
		{Bond::ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 4},
		{Bond::ParseError::NON_CONST_DECLARATION, Bond::Token::IDENTIFIER, 16},
		{Bond::ParseError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 18},
		{Bond::ParseError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 19},
		{Bond::ParseError::RECURSIVE_STRUCT, Bond::Token::IDENTIFIER, 20},
		{Bond::ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 31},
		{Bond::ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 33},
		{Bond::ParseError::SWITCH_CONTROL_IS_NOT_INTEGER, Bond::Token::OP_LT, 29},
		{Bond::ParseError::IF_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 37},
		{Bond::ParseError::WHILE_CONDITION_IS_NOT_BOOLEAN, Bond::Token::ASSIGN_MINUS, 38},
		{Bond::ParseError::FOR_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 39},
		{Bond::ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::OP_INC, 41},
		{Bond::ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION, Bond::Token::OPAREN, 41},
		{Bond::ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::COMMA, 41},
		{Bond::ParseError::MISSING_BRACES_IN_INITIALIZER, Bond::Token::CONST_INT, 42},
		{Bond::ParseError::MISSING_BRACES_IN_INITIALIZER, Bond::Token::CONST_INT, 42},
		{Bond::ParseError::BRACES_AROUND_SCALAR_INITIALIZER, Bond::Token::CONST_INT, 43},
		{Bond::ParseError::INVALID_TYPE_CONVERSION, Bond::Token::CONST_BOOL, 44},
		{Bond::ParseError::THIS_IN_NON_MEMBER_FUNCTION, Bond::Token::KEY_THIS, 45},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValueEvaluationErrors, "scripts/sanalyzer_ValueEvaluationErrors.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::INVALID_STRUCT_SIZE, Bond::Token::CONST_INT, 1},
		{Bond::ParseError::INVALID_STRUCT_ALIGNMENT, Bond::Token::CONST_INT, 2},
		{Bond::ParseError::STRUCT_SIZE_ALIGNMENT_MISMATCH, Bond::Token::CONST_INT, 3},
		{Bond::ParseError::ARRAY_SIZE_IS_ZERO, Bond::Token::IDENTIFIER, 9},
		{Bond::ParseError::ARRAY_SIZE_IS_NOT_CONST_INTEGER, Bond::Token::IDENTIFIER, 10},
		{Bond::ParseError::ARRAY_SIZE_IS_UNSPECIFIED, Bond::Token::KEY_INT, 11},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValueEvaluationErrors2, "scripts/sanalyzer_ValueEvaluationErrors2.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 5},
		{Bond::ParseError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 10},
		{Bond::ParseError::CANNOT_RESOLVE_SYMBOL_VALUE, Bond::Token::IDENTIFIER, 14},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ValidationErrors, "scripts/sanalyzer_ValidationErrors.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::UNTERMINATED_SWITCH_SECTION, Bond::Token::KEY_CASE, 5},
		{Bond::ParseError::INVALID_BREAK, Bond::Token::KEY_BREAK, 17},
		{Bond::ParseError::INVALID_CONTINUE, Bond::Token::KEY_CONTINUE, 21},
		{Bond::ParseError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 25},
		{Bond::ParseError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 37},
		{Bond::ParseError::NOT_ALL_PATHS_RETURN_A_VALUE, Bond::Token::IDENTIFIER, 46},
		{Bond::ParseError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 68},
		{Bond::ParseError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 81},
		{Bond::ParseError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 94},
		{Bond::ParseError::UNREACHABLE_CODE, Bond::Token::ASSIGN, 109},
		{Bond::ParseError::INVALID_RETURN_TYPE_CONVERSION, Bond::Token::KEY_RETURN, 115},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

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
  TEST_ITEM(ValidationErrors)                   \

RUN_TESTS(SemanticAnalyzer, TEST_ITEMS)
