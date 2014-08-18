#include "bond/api/libruntime.h"
#include "bond/io/diskfileloader.h"
#include "bond/stl/vector.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
#include "bond/vm/vm.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

const size_t MIN_STACK_SIZE = 1;
const size_t DEFAULT_STACK_SIZE = 32;
const char * const DEFAULT_ENTRY_POINT = "::main";
typedef Bond::Vector<const char *> StringList;
typedef Bond::Vector<Bond::FileLoader::Handle> FileHandleList;

int main(int argc, const char *argv[])
{
	Bond::ValidateConfiguration();

	Bond::DefaultAllocator allocator;
	StringList::Type cboFileNameList((StringList::Allocator(&allocator)));
	StringList::Type argList((StringList::Allocator(&allocator)));
	size_t stackSize = DEFAULT_STACK_SIZE;
	const char *entryPoint = DEFAULT_ENTRY_POINT;
	Bond::bi32_t exitCode = 0;
	bool pushingApplicationArgs = false;
	bool error = false;

	cboFileNameList.reserve(size_t(argc));
	argList.reserve(size_t(argc));

	for (int i = 1; i < argc; ++i)
	{
		if (pushingApplicationArgs)
		{
			argList.push_back(argv[i]);
		}
		else if (strcmp(argv[i], "-") == 0)
		{
			pushingApplicationArgs = true;
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			if (++i < argc)
			{
				entryPoint = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -e\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			if (++i < argc)
			{
				stackSize = size_t(atol(argv[i]));
				if (stackSize < MIN_STACK_SIZE)
				{
					fprintf(stderr, "Stack size must be at least %u kB.\n", unsigned(MIN_STACK_SIZE));
				}
			}
			else
			{
				fprintf(stderr, "Missing argument to -s\n");
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
			cboFileNameList.push_back(argv[i]);
		}
	}

	if (error)
	{
		return 1;
	}

	try
	{
		Bond::CboLoader::Handle codeSegmentHandle;
		{
			Bond::DiskFileLoader fileLoader(allocator);
			Bond::CboLoader cboLoader(allocator);
			FileHandleList::Type cboFileHandleList((FileHandleList::Allocator(&allocator)));
			cboFileHandleList.reserve(cboFileNameList.size());

			Bond::LoadAllLibs(cboLoader);

			StringList::Type::const_iterator it = cboFileNameList.begin();
			for (size_t i = 0; it != cboFileNameList.end(); ++it, ++i)
			{
				cboFileHandleList.push_back(fileLoader.LoadFile(*it));
				cboLoader.AddCboFile(cboFileHandleList[i].Get());
			}

			codeSegmentHandle = cboLoader.Load();
		}

		const Bond::bu32_t numArgs = Bond::bu32_t(argList.size());
		const char **args = (numArgs > Bond::bu32_t(0)) ? &argList[0] : NULL;
		Bond::VM vm(allocator, *codeSegmentHandle.Get(), stackSize * 1024);
		vm.CallFunction(entryPoint, &exitCode, numArgs, args);
	}
	catch (const Bond::Exception &e)
	{
		error = true;
		fprintf(stderr, "%s\n", e.GetMessage());
	}

	return error ? 1 : exitCode;
}
