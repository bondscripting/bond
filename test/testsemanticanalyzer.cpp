#include "framework/testsemanticanalyzerframework.h"
#include "framework/testparserframework.h"
#include "bond/parsenodeutil.h"

DEFINE_SEMANTICANALYZER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *outerSpace = globalScope->FindSymbol("OuterSpace");
	ASSERT_MESSAGE(outerSpace != 0, "Failed to find symbol 'OuterSpace'.");
	ASSERT_MESSAGE(outerSpace->GetSymbolType() == Bond::Symbol::TYPE_NAMESPACE, "Expected 'OuterSpace' to be a namespace.");

	const Bond::Symbol *firstInnerSpace = outerSpace->FindSymbol("FirstInnerSpace");
	ASSERT_MESSAGE(firstInnerSpace != 0, "Failed to find symbol 'FirstInnerSpace'.");

	const Bond::Symbol *secondInnerSpace = outerSpace->FindSymbol("SecondInnerSpace");
	ASSERT_MESSAGE(secondInnerSpace != 0, "Failed to find symbol 'SecondInnerSpace'.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *empty = globalScope->FindSymbol("Empty");
	ASSERT_MESSAGE(empty != 0, "Failed to find symbol 'Empty'.");
	ASSERT_MESSAGE(empty->GetSymbolType() == Bond::Symbol::TYPE_ENUM, "Expected 'Empty' to be an enum.");

	const Bond::Symbol *eighth = globalScope->FindSymbol("EIGHTH");
	ASSERT_MESSAGE(eighth != 0, "Failed to find symbol 'EIGHTH'.");
	ASSERT_MESSAGE(eighth->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'EIGHTH' to be a value.");

	// TODO: check type and value.

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *symbol = globalScope->FindSymbol("VoidOneParameter");
	ASSERT_MESSAGE(symbol != 0, "Failed to find symbol 'VoidOneParameter'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'VoidOneParameter' to be a function.");

	const Bond::FunctionDefinition *function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	ASSERT_MESSAGE(function->GetNextDefinition() == 0, "Expected 'VoidOneParameter' to have a single definition.");

	symbol = globalScope->FindSymbol("ComplexFunctionPrototype");
	ASSERT_MESSAGE(symbol != 0, "Failed to find symbol 'ComplexFunctionPrototype'.");
	ASSERT_MESSAGE(symbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'ComplexFunctionPrototype' to be a function.");

	function = Bond::CastNode<Bond::FunctionDefinition>(symbol);
	function = function->GetNextDefinition();
	ASSERT_MESSAGE(function != 0, "Expected 'ComplexFunctionPrototype' to have a second definition.");
	ASSERT_MESSAGE(function->GetNextDefinition() == 0, "Expected 'ComplexFunctionPrototype' to have only two definitions.");

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::SymbolTable &table = analyzer.GetSymbolTable();
	const Bond::Symbol *globalScope = table.GetGlobalScope();

	const Bond::Symbol *structSymbol = globalScope->FindSymbol("DataAndFunctions");
	ASSERT_MESSAGE(structSymbol != 0, "Failed to find symbol 'DataAndFunctions'.");
	ASSERT_MESSAGE(structSymbol->GetSymbolType() == Bond::Symbol::TYPE_STRUCT, "Expected 'DataAndFunctions' to be a struct.");

	const Bond::Symbol *memberSymbol = structSymbol->FindSymbol("x");
	ASSERT_MESSAGE(memberSymbol != 0, "Failed to find symbol 'x'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_VALUE, "Expected 'x' to be a value.");

	memberSymbol = structSymbol->FindSymbol("DoStuff");
	ASSERT_MESSAGE(memberSymbol != 0, "Failed to find symbol 'DoStuff'.");
	ASSERT_MESSAGE(memberSymbol->GetSymbolType() == Bond::Symbol::TYPE_FUNCTION, "Expected 'DoStuff to be a function.");

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

	ASSERT_PARSE_ERRORS(analyzer.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors, "scripts/sanalyzer_TypeEvaluationErrors.bond")
{
	// Focus on expression type evaluation erros.
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 31},
		{Bond::ParseError::ParseError::UNASSIGNABLE_TYPE, Bond::Token::ASSIGN, 32},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_LEFT, 33},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_MOD, 34},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 35},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_PLUS, 36},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::ASSIGN_DIV, 37},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_OR, 38},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_AMP, 39},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_AMP, 39},
		{Bond::ParseError::ParseError::INVALID_COMPARISON, Bond::Token::OP_LT, 40},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_MINUS, 41},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_NOT, 42},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_STAR, 43},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_BIT_NOT, 44},
		{Bond::ParseError::ParseError::UNASSIGNABLE_TYPE, Bond::Token::OP_INC, 45},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_OPERATOR, Bond::Token::OP_INC, 45},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OP_ARROW, 46},
		{Bond::ParseError::ParseError::NON_STRUCT_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 47},
		{Bond::ParseError::ParseError::INVALID_MEMBER_REQUEST, Bond::Token::IDENTIFIER, 48},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_INDEX_OPERATOR, Bond::Token::OBRACKET, 49},
		{Bond::ParseError::ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, Bond::Token::OBRACKET, 50},
		{Bond::ParseError::ParseError::EXPRESSION_IS_NOT_CALLABLE, Bond::Token::OPAREN, 51},
		{Bond::ParseError::ParseError::INVALID_TYPE_CONVERSION, Bond::Token::KEY_INT, 52},
		{Bond::ParseError::ParseError::INVALID_SYMBOL_IN_EXPRESSION, Bond::Token::IDENTIFIER, 53},
		{Bond::ParseError::ParseError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 54},
		{Bond::ParseError::ParseError::SYMBOL_IS_NOT_DEFINED, Bond::Token::IDENTIFIER, 63},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(analyzer.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(TypeEvaluationErrors2, "scripts/sanalyzer_TypeEvaluationErrors2.bond")
{
	// Focus on remaining type evaluation erros.
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 3},
		{Bond::ParseError::ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 4},
		{Bond::ParseError::ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_FLOAT, 24},
		{Bond::ParseError::ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, Bond::Token::CONST_BOOL, 26},
		{Bond::ParseError::ParseError::SWITCH_CONTROL_IS_NOT_INTEGER, Bond::Token::OP_LT, 22},
		{Bond::ParseError::ParseError::IF_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 30},
		{Bond::ParseError::ParseError::WHILE_CONDITION_IS_NOT_BOOLEAN, Bond::Token::ASSIGN_MINUS, 31},
		{Bond::ParseError::ParseError::FOR_CONDITION_IS_NOT_BOOLEAN, Bond::Token::OP_PLUS, 32},
		{Bond::ParseError::ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::OP_INC, 34},
		{Bond::ParseError::ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION, Bond::Token::OPAREN, 34},
		{Bond::ParseError::ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::COMMA, 34},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(analyzer.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Namespaces)                         \
  TEST_ITEM(Enums)                              \
  TEST_ITEM(FunctionDefinitions)                \
  TEST_ITEM(Structs)                            \
  TEST_ITEM(TypeSpecifierErrors)                \
  TEST_ITEM(TypeEvaluationErrors)               \
  TEST_ITEM(TypeEvaluationErrors2)              \

RUN_TESTS(Parser, TEST_ITEMS)
