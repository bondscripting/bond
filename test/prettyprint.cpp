#include "framework/testparserframework.h"
#include "bond/stdouttextwriter.h"
#include "bond/prettyprinter.h"

bool PrettyPrint(Bond::TextWriter &logger, Bond::Parser &parser)
{
	if (parser.HasErrors())
	{
		for (int i = 0; i < parser.GetNumErrors(); ++i)
		{
			const Bond::ParseError *error = parser.GetError(i);
			error->Print(logger);
			logger.Write("\n");
		}
	}
	else
	{
		Bond::PrettyPrinter printer(logger);
		printer.Print(parser.GetTranslationUnit());
	}

	return true;
}


int main(int argc, const char *argv[])
{
	Bond::StdoutTextWriter logger;

	for (int i = 1; i < argc; ++i)
	{
		TestFramework::RunParserTest(logger, __FILE__, __LINE__, argv[i], PrettyPrint);
	}

	return 0;
}
