#include "framework/testsemanticanalyzerframework.h"
#include "framework/testparserframework.h"

#include "bond/symboltable.h"

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
	ASSERT_MESSAGE(eighth->GetSymbolType() == Bond::Symbol::TYPE_CONSTANT, "Expected 'EIGHTH' to be a constant.");

	return true;
}

/*
DEFINE_SEMANTICANALYZER_TEST(FunctionDeclarations, "scripts/parser_FunctionDeclarations.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 0;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 4;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(DeclarativeAndExpressionStatements, "scripts/parser_DeclarativeAndExpressionStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 2;
	expectedCount.mExpressionStatement = 2;
	expectedCount.mNamedInitializer = 3;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Initializers, "scripts/parser_Initializers.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 3;
	expectedCount.mNamedInitializer = 4;
	expectedCount.mInitializer = 16;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mStructDeclaration = 4;
	expectedCount.mDeclarativeStatement = 4;
	expectedCount.mFunctionPrototype = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mIfStatement = 8;
	expectedCount.mCompoundStatement = 2;
	expectedCount.mExpressionStatement = 11;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(WhileStatements, "scripts/parser_WhileStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mWhileStatement = 6;
	expectedCount.mCompoundStatement = 3;
	expectedCount.mExpressionStatement = 6;
	expectedCount.mUnaryExpression = 4;
	expectedCount.mBinaryExpression = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(ForStatements, "scripts/parser_ForStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mForStatement = 7;
	expectedCount.mCompoundStatement = 2;
	expectedCount.mDeclarativeStatement = 3;
	expectedCount.mExpressionStatement = 11;
	expectedCount.mUnaryExpression = 5;
	expectedCount.mBinaryExpression = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(SwitchStatements, "scripts/parser_SwitchStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mSwitchStatement = 1;
	expectedCount.mSwitchSection = 2;
	expectedCount.mSwitchLabel = 3;
	expectedCount.mJumpStatement = 2;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_SEMANTICANALYZER_TEST(JumpStatements, "scripts/parser_JumpStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(analyzer.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mJumpStatement = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}
*/
/*
DEFINE_SEMANTICANALYZER_TEST(MiscErrors, "scripts/parser_MiscErrors.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::DUPLICATE_CONST, Bond::Token::KEY_CONST, 3},
		{Bond::ParseError::PARSE_ERROR, Bond::Token::OP_DIV, 5},
		{Bond::ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION, Bond::Token::OPAREN, 10},
		{Bond::ParseError::INCREMENT_IN_CONST_EXPRESSION, Bond::Token::OP_INC, 13},
		{Bond::ParseError::INCREMENT_IN_CONST_EXPRESSION, Bond::Token::OP_DEC, 16},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 21},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 22},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::KEY_INT, 23},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 26},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 27},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 28},
		{Bond::ParseError::PARSE_ERROR, Bond::Token::CPAREN, 31},
		{Bond::ParseError::PARSE_ERROR, Bond::Token::CPAREN, 32},
		{Bond::ParseError::PARSE_ERROR, Bond::Token::SEMICOLON, 33},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 36},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 39},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(analyzer.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 6;
	expectedCount.mIfStatement = 3;
	expectedCount.mSwitchStatement = 1;
	expectedCount.mWhileStatement = 3;
	expectedCount.mForStatement = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}
*/

#define TEST_ITEMS                              \
  TEST_ITEM(Namespaces)                         \
  TEST_ITEM(Enums)                              \
	/*
  TEST_ITEM(FunctionDeclarations)               \
  TEST_ITEM(FunctionDefinitions)                \
  TEST_ITEM(DeclarativeAndExpressionStatements) \
  TEST_ITEM(Initializers)                       \
  TEST_ITEM(Structs)                            \
  TEST_ITEM(IfStatements)                       \
  TEST_ITEM(WhileStatements)                    \
  TEST_ITEM(ForStatements)                      \
  TEST_ITEM(SwitchStatements)                   \
  TEST_ITEM(EnumErrors)                         \
  TEST_ITEM(StructErrors)                       \
  TEST_ITEM(MiscErrors)                         \
	*/

RUN_TESTS(Parser, TEST_ITEMS)
