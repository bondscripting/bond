#include "framework/testparserframework.h"

DEFINE_PARSER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mNamespaceDefinition = 3;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Enums, "scripts/parser_Enums.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mEnumDeclaration = 4;
	expectedCount.mEnumerator = 9;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(FunctionDeclarations, "scripts/parser_FunctionDeclarations.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 0;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(FunctionDefinitions, "scripts/parser_FunctionDefinitions.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mFunctionPrototype = 4;
	expectedCount.mFunctionDefinition = 4;
	expectedCount.mCompoundStatement = 4;
	expectedCount.mParameter = 5;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(DeclarativeAndExpressionStatements, "scripts/parser_DeclarativeAndExpressionStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 2;
	expectedCount.mExpressionStatement = 2;
	expectedCount.mNamedInitializer = 3;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(Initializers, "scripts/parser_Initializers.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mDeclarativeStatement = 3;
	expectedCount.mNamedInitializer = 4;
	expectedCount.mInitializer = 16;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(IfStatements, "scripts/parser_IfStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mIfStatement = 8;
	expectedCount.mCompoundStatement = 2;
	expectedCount.mExpressionStatement = 11;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(WhileStatements, "scripts/parser_WhileStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
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
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
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
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mSwitchStatement = 1;
	expectedCount.mSwitchSection = 2;
	expectedCount.mSwitchLabel = 3;
	expectedCount.mJumpStatement = 2;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(JumpStatements, "scripts/parser_JumpStatements.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mJumpStatement = 4;

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
  TEST_ITEM(IfStatements)                       \
  TEST_ITEM(WhileStatements)                    \
  TEST_ITEM(ForStatements)                      \
  TEST_ITEM(SwitchStatements)                   \


RUN_TESTS(Parser, TEST_ITEMS)
