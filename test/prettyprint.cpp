#include "framework/testparserframework.h"
#include "framework/testsemanticanalyzerframework.h"
#include "bond/stdouttextwriter.h"
#include "bond/prettyprinter.h"
#include <string.h>

static bool gFoldConstants = false;

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
	Bond::PrettyPrinter printer(logger, gFoldConstants);
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
	bool doSemanticAnalysis = false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			// Constant folding happens during semantic analysis.
			gFoldConstants = true;
			doSemanticAnalysis = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			doSemanticAnalysis = true;
		}
		else
		{
			if (doSemanticAnalysis)
			{
				TestFramework::RunSemanticAnalyzerTest(logger, __FILE__, __LINE__, argv[i], PrettyPrint);
			}
			else
			{
				TestFramework::RunParserTest(logger, __FILE__, __LINE__, argv[i], PrettyPrint);
			}
		}
	}

	return 0;
}
