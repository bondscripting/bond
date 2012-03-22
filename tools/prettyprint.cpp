#include "bond/compilererror.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/lexer.h"
#include "bond/parser.h"
#include "bond/prettyprinter.h"
#include "bond/semanticanalyzer.h"
#include "bond/stdouttextwriter.h"
#include <string.h>


void PrintErrors(Bond::TextWriter &writer, const Bond::CompilerErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			const Bond::CompilerError *error = errorBuffer.GetError(i);
			error->Print(writer);
			writer.Write("\n");
		}
	}
}


void PrettyPrint(const char *scriptName, bool doSemanticAnalysis, bool foldConstants)
{
	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	if (script.mData != NULL)
	{
		Bond::Lexer lexer(allocator);
		lexer.Lex(script.mData, script.mLength);
		fileLoader.DisposeFile(script);

		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Parser parser(allocator, errorBuffer);
		parser.Parse(stream);

		if (!errorBuffer.HasErrors() && (doSemanticAnalysis || foldConstants))
		{
			Bond::SemanticAnalyzer analyzer(errorBuffer);
			analyzer.Analyze(parser.GetTranslationUnitList());
		}

		Bond::StdOutTextWriter writer;
		Bond::PrettyPrinter printer(writer, foldConstants);
		printer.PrintList(parser.GetTranslationUnitList());

		PrintErrors(writer, errorBuffer);
	}
}


int main(int argc, const char *argv[])
{
	bool foldConstants = false;
	bool doSemanticAnalysis = false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			foldConstants = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			doSemanticAnalysis = true;
		}
		else
		{
			PrettyPrint(argv[i], doSemanticAnalysis, foldConstants);
		}
	}

	return 0;
}
