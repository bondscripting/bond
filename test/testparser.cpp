#include "framework/testparserframework.h"
#include "bond/prettyprinter.h"
#include <stdio.h>

//DEFINE_PARSER_TEST(Namespaces, "scripts/parser_Namespaces.bond")
DEFINE_PARSER_TEST(Namespaces, "scripts/parse.bond")
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


#define TEST_ITEMS         \
  TEST_ITEM(Namespaces)    \


RUN_TESTS(Parser, TEST_ITEMS)
