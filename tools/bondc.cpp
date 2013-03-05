#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/defaultfileloader.h"
#include "bond/io/filebinarywriter.h"
#include "bond/io/stdouttextwriter.h"
#include "bond/stl/list.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include <stdio.h>
#include <string.h>

typedef Bond::List<Bond::DefaultFileLoader> FileLoaderList;

void PrintErrors(Bond::TextWriter &writer, const Bond::CompilerErrorBuffer &errorBuffer)
{
	for (size_t i = 0; i < errorBuffer.GetNumErrors(); ++i)
	{
		const Bond::CompilerError &error = errorBuffer.GetError(i);
		error.Print(writer);
		writer.Write("\n");
	}
}


int main(int argc, const char *argv[])
{
	bool error = false;

	try
	{
		Bond::DefaultAllocator allocator;
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		Bond::Parser parser(allocator, errorBuffer);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		FileLoaderList::Type loaderList((FileLoaderList::Allocator(&allocator)));
		loaderList.push_back(Bond::DefaultFileLoader(allocator));
		Bond::FrontEnd frontEnd(allocator, lexer, parser, analyzer, loaderList.back());
		const char *outputFileName = "bond.cbo";

		for (int i = 1; i < argc; ++i)
		{
			if (strcmp(argv[i], "-I") == 0)
			{
				if (++i < argc)
				{
					Bond::DefaultFileLoader &prev = loaderList.back();
					loaderList.push_back(Bond::DefaultFileLoader(allocator, argv[i]));
					prev.SetDelegateLoader(&loaderList.back());
				}
				else
				{
					fprintf(stderr, "Missing argument to -I\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-o") == 0)
			{
				if (++i < argc)
				{
					outputFileName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -o\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-r") == 0)
			{
				frontEnd.SetRecursiveCompileEnabled(true);
			}
			else if (argv[i][0] == '-')
			{
				fprintf(stderr, "Unknown option '%s'\n", argv[i]);
				error = true;
			}
			else
			{
				frontEnd.AddInputFile(argv[i]);
			}
		}

		if (error)
		{
			return 1;
		}

		frontEnd.Analyze();

		if (!errorBuffer.HasErrors())
		{
			FILE *outputFile = fopen(outputFileName, "wb");
			if (outputFile != NULL)
			{
				Bond::FileBinaryWriter cboWriter(outputFile);
				Bond::CodeGenerator generator(allocator, errorBuffer);
				generator.Generate(parser.GetTranslationUnitList(), cboWriter);
				fclose(outputFile);
			}
			else
			{
				fprintf(stderr, "Failed to open '%s'.\n", outputFileName);
			}
		}

		Bond::StdOutTextWriter errorWriter;
		PrintErrors(errorWriter, errorBuffer);
		error = error || errorBuffer.HasErrors();
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
		error = true;
	}

	return error ? 1 : 0;
}
