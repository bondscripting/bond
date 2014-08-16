#include "bond/api/libinclude.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/diskfileloader.h"
#include "bond/io/stdiobinarywriter.h"
#include "bond/io/stdiotextwriter.h"
#include "bond/stl/list.h"
#include "bond/tools/nativebindinggenerator.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include <cstdio>
#include <cstring>

typedef Bond::List<Bond::DiskFileLoader> FileLoaderList;

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
		Bond::PointerSize pointerSize = Bond::BOND_NATIVE_POINTER_SIZE;
		Bond::DefaultAllocator allocator;
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		Bond::Parser parser(allocator, errorBuffer);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		FileLoaderList::Type loaderList((FileLoaderList::Allocator(&allocator)));
		loaderList.push_back(Bond::DiskFileLoader(allocator));
		Bond::MemoryFileLoader stdLibLoader(Bond::INCLUDE_FILE_INDEX, &loaderList.back());
		Bond::FrontEnd frontEnd(allocator, lexer, parser, analyzer, stdLibLoader);
		const char *cboFileName = NULL;
		const char *cppFileName = NULL;
		const char *hFileName = NULL;
		const char *bindingCollectionName = NULL;
		const char *includeName = NULL;
		bool generateBindings = false;

		for (int i = 1; i < argc; ++i)
		{
			if (strcmp(argv[i], "-b") == 0)
			{
				if (++i < argc)
				{
					bindingCollectionName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -b\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-c") == 0)
			{
				if (++i < argc)
				{
					cppFileName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -c\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-h") == 0)
			{
				if (++i < argc)
				{
					hFileName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -h\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-i") == 0)
			{
				if (++i < argc)
				{
					includeName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -i\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-I") == 0)
			{
				if (++i < argc)
				{
					Bond::DiskFileLoader &prev = loaderList.back();
					loaderList.push_back(Bond::DiskFileLoader(allocator, argv[i]));
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
					cboFileName = argv[i];
				}
				else
				{
					fprintf(stderr, "Missing argument to -o\n");
					error = true;
				}
			}
			else if (strcmp(argv[i], "-p32") == 0)
			{
				pointerSize = Bond::POINTER_32BIT;
			}
			else if (strcmp(argv[i], "-p64") == 0)
			{
				pointerSize = Bond::POINTER_64BIT;
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

		if ((cppFileName != NULL) || (hFileName != NULL) || (bindingCollectionName != NULL) || (includeName != NULL))
		{
			if ((cppFileName == NULL) || (hFileName == NULL) || (bindingCollectionName == NULL) || (includeName == NULL))
			{
				fprintf(stderr, "Options -b -c -h and -i must all be specified together.\n");
				error = true;
			}
			else
			{
				generateBindings = true;
			}
		}

		if (!generateBindings && (cboFileName == NULL))
		{
			// If no output file name specified, then compile to bond.cbo.
			cboFileName = "bond.cbo";
		}

		if (error)
		{
			return 1;
		}

		frontEnd.Analyze();

		if (!errorBuffer.HasErrors())
		{
			if (cboFileName != NULL)
			{
				FILE *cboFile = fopen(cboFileName, "wb");
				if (cboFile != NULL)
				{
					Bond::StdioBinaryWriter cboWriter(cboFile);
					Bond::CodeGenerator generator(allocator, errorBuffer, pointerSize);
					generator.Generate(parser.GetTranslationUnitList(), cboWriter);
				}
				else
				{
					fprintf(stderr, "Failed to open '%s'.\n", cboFileName);
					error = true;
				}
				fclose(cboFile);
			}
			if (generateBindings)
			{
				FILE *cppFile = fopen(cppFileName, "w");
				FILE *hFile = fopen(hFileName, "w");
				if ((cppFile != NULL) && (hFile != NULL))
				{
					Bond::StdioTextWriter cppWriter(cppFile);
					Bond::StdioTextWriter hWriter(hFile);
					Bond::NativeBindingGenerator generator;
					generator.Generate(parser.GetTranslationUnitList(), cppWriter, hWriter, bindingCollectionName, includeName);
				}
				if (cppFile == NULL)
				{
					fprintf(stderr, "Failed to open '%s'.\n", cppFileName);
					error = true;
				}
				if (hFile == NULL)
				{
					fprintf(stderr, "Failed to open '%s'.\n", hFileName);
					error = true;
				}
				fclose(cppFile);
				fclose(hFile);
			}
		}

		Bond::StdErrTextWriter errorWriter;
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
