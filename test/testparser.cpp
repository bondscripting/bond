#include "framework/testparserframework.h"

DEFINE_PARSER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mNamespaceDefinition = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mEnumDeclaration = 4;
	expectedCount.mEnumerator = 9;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(FunctionDeclarations, "scripts/parser_FunctionDeclarations.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 4;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(DeclarativeAndExpressionStatements, "scripts/parser_DeclarativeAndExpressionStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 3;
	expectedCount.mNamedInitializer = 4;
	expectedCount.mInitializer = 16;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Structs, "scripts/parser_Structs.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mStructDeclaration = 4;
	expectedCount.mDeclarativeStatement = 4;
	expectedCount.mFunctionPrototype = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

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
	ASSERT_NO_PARSE_ERRORS(parser.GetErrorBuffer());

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mJumpStatement = 4;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(EnumErrors, "scripts/parser_EnumErrors.bond")
{
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::COMMA, 6},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 11},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 20},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(parser.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

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
	const TestFramework::ExpectedParseError EXPECTED_ERRORS[] =
	{
		{Bond::ParseError::INITIALIZER_NOT_ALLOWED, Bond::Token::ASSIGN, 6},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 9},
		{Bond::ParseError::FUNCTION_DEFINITION_NOT_ALLOWED, Bond::Token::OBRACE, 12},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OP_DIV, 15},
		{Bond::ParseError::FUNCTION_DEFINITION_NOT_ALLOWED, Bond::Token::OBRACE, 15},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::OBRACE, 19},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::KEY_INT, 26},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::SEMICOLON, 32},
		{Bond::ParseError::UNEXPECTED_TOKEN, Bond::Token::COMMA, 34},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_PARSE_ERRORS(parser.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

	const Bond::ListParseNode *root = parser.GetTranslationUnitList();

	Bond::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 6;
	expectedCount.mStructDeclaration = 4;
	expectedCount.mFunctionPrototype = 3;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(MiscErrors, "scripts/parser_MiscErrors.bond")
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

	ASSERT_PARSE_ERRORS(parser.GetErrorBuffer(), EXPECTED_ERRORS, NUM_ERRORS);

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
