#include "framework/testparserframework.h"
#include "bond/stdouttextwriter.h"
#include "bond/prettyprinter.h"

bool PrettyPrint(Bond::TextWriter &logger, Bond::Parser &parser)
{
	Bond::PrettyPrinter printer(logger);
	printer.Print(parser.GetTranslationUnitList());

	const Bond::ParseErrorBuffer &errorBuffer = parser.GetErrorBuffer();
	if (errorBuffer.HasErrors())
	{
		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			const Bond::ParseError *error = errorBuffer.GetError(i);
			error->Print(logger);
			logger.Write("\n");
		}
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
