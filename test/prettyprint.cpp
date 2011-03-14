#include "framework/testparserframework.h"
#include "framework/testsemanticanalyzerframework.h"
#include "bond/stdouttextwriter.h"
#include "bond/prettyprinter.h"
#include <string.h>

void PrintErrors(Bond::TextWriter &logger, const Bond::ParseErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			const Bond::ParseError *error = errorBuffer.GetError(i);
			error->Print(logger);
			logger.Write("\n");
		}
	}
}


bool PrettyPrint(Bond::TextWriter &logger, Bond::Parser &parser)
{
	Bond::PrettyPrinter printer(logger);
	printer.PrintList(parser.GetTranslationUnitList());
	PrintErrors(logger, parser.GetErrorBuffer());

	return true;
}


bool PrettyPrint(Bond::TextWriter &logger, Bond::Parser &parser, Bond::SemanticAnalyzer &analyzer)
{
	PrettyPrint(logger, parser);
	PrintErrors(logger, analyzer.GetErrorBuffer());
	return true;
}


int main(int argc, const char *argv[])
{
	Bond::StdoutTextWriter logger;

	// Do semantic analysis after parsing.
	if ((argc > 1) && (strcmp(argv[1], "-s") == 0))
	{
		for (int i = 2; i < argc; ++i)
		{
			TestFramework::RunSemanticAnalyzerTest(logger, __FILE__, __LINE__, argv[i], PrettyPrint);
		}
	}

	// Otherwise, just parse.
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			TestFramework::RunParserTest(logger, __FILE__, __LINE__, argv[i], PrettyPrint);
		}
	}

	return 0;
}
