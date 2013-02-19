#include "bond/codegenerator.h"
#include "bond/compilererror.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/exception.h"
#include "bond/filebinarywriter.h"
#include "bond/frontend.h"
#include "bond/lexer.h"
#include "bond/list.h"
#include "bond/parser.h"
#include "bond/semanticanalyzer.h"
#include "bond/stdouttextwriter.h"
#include <stdio.h>
#include <string.h>

typedef Bond::List<const char *> StringList;

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
		Bond::DefaultFileLoader fileLoader(allocator);
		Bond::FrontEnd frontEnd(allocator, lexer, parser, analyzer, fileLoader);

		StringList::Type includePathList((StringList::Allocator(&allocator)));
		const char *outputFileName = "bond.cbo";

		for (int i = 1; i < argc; ++i)
		{
			if (strcmp(argv[i], "-I") == 0)
			{
				if (++i < argc)
				{
					includePathList.push_back(argv[i]);
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
