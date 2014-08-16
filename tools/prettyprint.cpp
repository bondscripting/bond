#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/diskfileloader.h"
#include "bond/io/stdiotextwriter.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/tools/parsetreeprinter.h"
#include "bond/tools/prettyprinter.h"
#include <cstdio>
#include <cstring>


void PrintErrors(Bond::TextWriter &writer, const Bond::CompilerErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		for (size_t i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			const Bond::CompilerError &error = errorBuffer.GetError(i);
			error.Print(writer);
			writer.Write("\n");
		}
	}
}


void PrintScript(const char *scriptName, bool doSemanticAnalysis, bool foldConstants, bool printParseTree)
{
	try
	{
		Bond::DefaultAllocator allocator;
		Bond::DiskFileLoader fileLoader(allocator);
		Bond::FileLoader::Handle scriptHandle = fileLoader.LoadFile(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		lexer.Lex(scriptName, reinterpret_cast<const char *>(scriptHandle.Get().mData), scriptHandle.Get().mLength);

		Bond::Parser parser(allocator, errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();
			parser.Parse(stream);
		}

		if (!errorBuffer.HasErrors() && (doSemanticAnalysis || foldConstants))
		{
			Bond::SemanticAnalyzer analyzer(errorBuffer);
			analyzer.Analyze(parser.GetTranslationUnitList());
		}

		Bond::StdOutTextWriter outputWriter;
		if (printParseTree)
		{
			Bond::ParseTreePrinter printer;
			printer.PrintList(parser.GetTranslationUnitList(), outputWriter);
		}
		else
		{
			Bond::PrettyPrinter printer;
			printer.PrintList(parser.GetTranslationUnitList(), outputWriter, foldConstants);
		}

		Bond::StdErrTextWriter errorWriter;
		PrintErrors(errorWriter, errorBuffer);
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
	}
}


int main(int argc, const char *argv[])
{
	bool foldConstants = false;
	bool doSemanticAnalysis = false;
	bool printParseTree = false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			foldConstants = true;
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			printParseTree = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			doSemanticAnalysis = true;
		}
		else
		{
			PrintScript(argv[i], doSemanticAnalysis, foldConstants, printParseTree);
		}
	}

	return 0;
}
