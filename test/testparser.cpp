#include "framework/asserts.h"
#include "framework/testparserframework.h"

DEFINE_PARSER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mNamespaceDefinition = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mEnumDeclaration = 4;
	expectedCount.mEnumerator = 9;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(FunctionDeclarations, "scripts/parser_FunctionDeclarations.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 0;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 8;
	expectedCount.mFunctionDefinition = 8;
	expectedCount.mCompoundStatement = 4;
	expectedCount.mParameter = 10;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(DeclarativeAndExpressionStatements, "scripts/parser_DeclarativeAndExpressionStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 2;
	expectedCount.mExpressionStatement = 2;
	expectedCount.mNamedInitializer = 3;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Initializers, "scripts/parser_Initializers.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 10;
	expectedCount.mNamedInitializer = 11;
	expectedCount.mInitializer = 25;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mStructDeclaration = 7;
	expectedCount.mDeclarativeStatement = 7;
	expectedCount.mFunctionPrototype = 8;
	expectedCount.mFunctionDefinition = 8;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mIfStatement = 8;
	expectedCount.mCompoundStatement = 2;
	expectedCount.mExpressionStatement = 11;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(WhileStatements, "scripts/parser_WhileStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

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


DEFINE_PARSER_TEST(ForStatements, "scripts/parser_ForStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

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


DEFINE_PARSER_TEST(SwitchStatements, "scripts/parser_SwitchStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mSwitchStatement = 1;
	expectedCount.mSwitchSection = 2;
	expectedCount.mSwitchLabel = 3;
	expectedCount.mJumpStatement = 2;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(JumpStatements, "scripts/parser_JumpStatements.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mJumpStatement = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(EnumErrors, "scripts/parser_EnumErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::COMMA, 6},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 11},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 20},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 2;
	expectedCount.mEnumDeclaration = 3;
	expectedCount.mEnumerator = 6;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(StructErrors, "scripts/parser_StructErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::INITIALIZER_NOT_ALLOWED, Bond::Token::ASSIGN, 6},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 9},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OP_DIV, 12},
		{Bond::CompilerError::NON_NATIVE_FUNCTION_DECLARATION, Bond::Token::IDENTIFIER, 15},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 19},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::KEY_INT, 26},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 32},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::COMMA, 34},
		{Bond::CompilerError::SIZE_AND_ALIGNMENT_NOT_ALLOWED, Bond::Token::CONST_INT, 37},
		{Bond::CompilerError::NATIVE_FUNCTION_DEFINITION, Bond::Token::IDENTIFIER, 43},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 6;
	expectedCount.mStructDeclaration = 6;
	expectedCount.mFunctionPrototype = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(MiscErrors, "scripts/parser_MiscErrors.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::DUPLICATE_CONST, Bond::Token::KEY_CONST, 3},
		{Bond::CompilerError::PARSE_ERROR, Bond::Token::OP_DIV, 5},
		{Bond::CompilerError::FUNCTION_CALL_IN_CONST_EXPRESSION, Bond::Token::OPAREN, 10},
		{Bond::CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::OP_INC, 13},
		{Bond::CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, Bond::Token::OP_DEC, 16},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 21},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 22},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::KEY_INT, 23},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 26},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 27},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 28},
		{Bond::CompilerError::PARSE_ERROR, Bond::Token::CPAREN, 31},
		{Bond::CompilerError::PARSE_ERROR, Bond::Token::CPAREN, 32},
		{Bond::CompilerError::PARSE_ERROR, Bond::Token::SEMICOLON, 33},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 36},
		{Bond::CompilerError::UNEXPECTED_TOKEN, Bond::Token::IDENTIFIER, 39},
		{Bond::CompilerError::MULTIDIMENTIONAL_ARRAY_BOUNDS, Bond::Token::OBRACKET, 44},
		{Bond::CompilerError::MULTIDIMENTIONAL_ARRAY_BOUNDS, Bond::Token::OBRACKET, 45},
		{Bond::CompilerError::ARRAY_OF_VOID, Bond::Token::OBRACKET, 51},
		{Bond::CompilerError::VOID_NOT_ALLOWED, Bond::Token::KEY_VOID, 54},
		{Bond::CompilerError::EMPTY_SWITCH_STATEMENT, Bond::Token::KEY_SWITCH, 57},
		{Bond::CompilerError::EMPTY_SWITCH_LABEL_LIST, Bond::Token::IDENTIFIER, 62},
		{Bond::CompilerError::VOID_NOT_ALLOWED, Bond::Token::KEY_VOID, 66},
		{Bond::CompilerError::CONST_NON_MEMBER_FUNCTION, Bond::Token::KEY_CONST, 67},
		{Bond::CompilerError::NON_NATIVE_FUNCTION_DECLARATION, Bond::Token::IDENTIFIER, 68},
		{Bond::CompilerError::FUNCTION_RETURNS_ARRAY, Bond::Token::IDENTIFIER, 71},
		{Bond::CompilerError::NATIVE_FUNCTION_DEFINITION, Bond::Token::IDENTIFIER, 72},
		{Bond::CompilerError::VOID_NOT_ALLOWED, Bond::Token::KEY_VOID, 77},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 12;
	expectedCount.mIfStatement = 3;
	expectedCount.mSwitchStatement = 3;
	expectedCount.mWhileStatement = 3;
	expectedCount.mForStatement = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Namespaces)                         \
  TEST_ITEM(Enums)                              \
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


RUN_TESTS(Parser, TEST_ITEMS)
