#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiooutputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/tools/parsetreeprinter.h"
#include "bond/tools/prettyprinter.h"
#include <cstdio>
#include <cstring>


void PrintScript(const char *scriptName, bool doSemanticAnalysis, bool foldConstants, bool printParseTree)
{
	try
	{
		Bond::DefaultAllocator allocator;
		Bond::StdioInputStream scriptStream(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		auto tokenCollectionHandle = lexer.Lex(scriptName, scriptStream);

		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&allocator)));
		Bond::Parser parser(allocator, errorBuffer, parseNodeStore);
		Bond::TranslationUnit *translationUnit = nullptr;
		if (!errorBuffer.HasErrors())
		{
			Bond::TokenStream stream = tokenCollectionHandle->GetTokenStream();
			translationUnit = parser.Parse(stream);
		}

		if (!errorBuffer.HasErrors() && (translationUnit != nullptr))
		{
			if (doSemanticAnalysis || foldConstants)
			{
				Bond::SemanticAnalyzer analyzer(errorBuffer);
				analyzer.Analyze(translationUnit);
			}

			Bond::StdOutOutputStream outputStream;
			if (printParseTree)
			{
				Bond::ParseTreePrinter printer;
				printer.PrintList(translationUnit, outputStream);
			}
			else
			{
				Bond::PrettyPrinter printer;
				printer.PrintList(translationUnit, outputStream, foldConstants);
			}
		}

		Bond::StdErrOutputStream errorStream;
		errorBuffer.Print(errorStream);
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
	}
}

class Boogup;

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
