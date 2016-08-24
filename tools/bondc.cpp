#include "bond/api/libinclude.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/diskfileloader.h"
#include "bond/io/stdiooutputstream.h"
#include "bond/stl/list.h"
#include "bond/tools/nativebindinggenerator.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include <cstdio>
#include <cstring>

typedef Bond::List<Bond::DiskFileLoader> FileLoaderList;

int main(int argc, const char *argv[])
{
	bool error = false;

	try
	{
		Bond::PointerSize pointerSize = Bond::BOND_NATIVE_POINTER_SIZE;
		Bond::DefaultAllocator allocator;
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::TokenCollectionStore tokenStore((Bond::TokenCollectionStore::allocator_type(&allocator)));
		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&allocator)));
		Bond::Lexer lexer(allocator, errorBuffer);
		Bond::Parser parser(allocator, errorBuffer, parseNodeStore);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		FileLoaderList loaderList((FileLoaderList::allocator_type(&allocator)));
		loaderList.push_back(Bond::DiskFileLoader(allocator));
		Bond::MemoryFileLoader stdIncludeLoader(Bond::INCLUDE_FILE_INDEX, &loaderList.back());
		Bond::FrontEnd frontEnd(allocator, tokenStore, lexer, parser, analyzer, stdIncludeLoader);
		const char *cboFileName = nullptr;
		const char *cppFileName = nullptr;
		const char *hFileName = nullptr;
		const char *bindingCollectionName = nullptr;
		const char *includeName = nullptr;
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

		if ((cppFileName != nullptr) || (hFileName != nullptr) || (bindingCollectionName != nullptr) || (includeName != nullptr))
		{
			if ((cppFileName == nullptr) || (hFileName == nullptr) || (bindingCollectionName == nullptr) || (includeName == nullptr))
			{
				fprintf(stderr, "Options -b -c -h and -i must all be specified together.\n");
				error = true;
			}
			else
			{
				generateBindings = true;
			}
		}

		if (!generateBindings && (cboFileName == nullptr))
		{
			// If no output file name specified, then compile to bond.cbo.
			cboFileName = "bond.cbo";
		}

		if (error)
		{
			return 1;
		}

		Bond::TranslationUnit *translationUnitList = frontEnd.Analyze();

		if (!errorBuffer.HasErrors())
		{
			if (cboFileName != nullptr)
			{
				Bond::StdioOutputStream cboStream(cboFileName);
				Bond::CodeGenerator generator(allocator, errorBuffer, pointerSize);
				generator.Generate(translationUnitList, cboStream);
			}
			if (generateBindings)
			{
					Bond::StdioOutputStream cppStream(cppFileName);
					Bond::StdioOutputStream hStream(hFileName);
					Bond::NativeBindingGenerator generator;
					generator.Generate(translationUnitList, cppStream, hStream, bindingCollectionName, includeName);
			}
		}

		Bond::StdErrOutputStream errorStream;
		errorBuffer.Print(errorStream);
		error = error || errorBuffer.HasErrors();
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
		error = true;
	}

	return error ? 1 : 0;
}
