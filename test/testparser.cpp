#include "framework/testparserframework.h"
#include "bond/prettyprinter.h"
#include <stdio.h>

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
	expectedCount.mEnumDeclaration = 2;
	expectedCount.mEnumerator = 6;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}


DEFINE_PARSER_TEST(EnumsWithInitializers, "scripts/parser_EnumsWithInitializers.bond")
{
	ASSERT_NO_PARSE_ERRORS();

	const Bond::ParseNode *root = parser.GetTranslationUnit();

	TestFramework::ParseNodeCount expectedCount(-1);
	expectedCount.mEnumDeclaration = 2;
	expectedCount.mEnumerator = 4;

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
	expectedCount.mParameter = 5;
	expectedCount.mCompoundStatement = 0;

	ASSERT_PARSE_NODE_COUNT(root, expectedCount);

	return true;
}

/*
DEFINE_PARSER_TEST(All, "scripts/parse.bond")
{
	if (parser.HasErrors())
	{
		const int numErrors = parser.GetNumErrors();
		for (int i = 0; i < numErrors; ++i)
		{
			const Bond::Parser::Error *error = parser.GetError(i);
			const Bond::Token *token = error->token;
			const Bond::StreamPos &pos = token->GetStartPos();
			printf("Error %d (%d, %d): expected %s before '%s'\n",
				error->type,
				pos.line,
				pos.column,
				error->expected,
				token->GetText());
		}
	}
	else
	{
		Bond::PrettyPrinter printer;
		printer.Print(parser.GetTranslationUnit());
	}

	return true;
}
*/

#define TEST_ITEMS                 \
  TEST_ITEM(Namespaces)            \
  TEST_ITEM(Enums)                 \
  TEST_ITEM(EnumsWithInitializers) \
  TEST_ITEM(FunctionDeclarations)  \


RUN_TESTS(Parser, TEST_ITEMS)
