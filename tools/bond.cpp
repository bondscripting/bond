#include "bond/cboloader.h"
#include "bond/codesegment.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/exception.h"
#include "bond/list.h"
#include "bond/stdouttextwriter.h"
#include "bond/vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t MIN_STACK_SIZE = 1;
const size_t DEFAULT_STACK_SIZE = 32;
const char * const DEFAULT_ENTRY_POINT = "::main";
typedef Bond::List<const char *> StringList;

int main(int argc, const char *argv[])
{
	Bond::DefaultAllocator allocator;
	StringList::Type cboFileNameList((StringList::Allocator(&allocator)));
	size_t stackSize = DEFAULT_STACK_SIZE;
	const char *entryPoint = DEFAULT_ENTRY_POINT;
	bool error = false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-e") == 0)
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
					fprintf(stderr, "Stack size must be at least %u kB.\n", MIN_STACK_SIZE);
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
		Bond::DefaultFileLoader fileLoader(allocator);
		const size_t numCboFiles = cboFileNameList.size();
		Bond::Allocator::ArrayHandle<Bond::FileLoader::Handle> cboFileHandles(allocator, allocator.Alloc<Bond::FileLoader::Handle>(numCboFiles));
		Bond::Allocator::Handle<Bond::FileData> cboFiles(allocator, allocator.Alloc<Bond::FileData>(numCboFiles));

		StringList::Type::const_iterator it = cboFileNameList.begin();
		size_t index = 0;
		while (it != cboFileNameList.end())
		{
			Bond::FileLoader::Handle &handle = cboFileHandles.Get()[index];
			handle = fileLoader.LoadFile(*it);
			cboFiles[index] = handle.Get();
			++index;
			++it;
			cboFileHandles.SetNumElements(index);
		}

		Bond::CboLoader cboLoader(allocator);
		Bond::CboLoader::Handle codeSegmentHandle = cboLoader.Load(cboFiles.Get(), numCboFiles);

		cboFiles.Reset();
		cboFileHandles.Reset();

		Bond::VM vm(allocator, *codeSegmentHandle.Get(), stackSize * 1024);

		// Test code.
		Bond::bi32_t returnValue = -9999;
		Bond::VM::CallerStackFrame stackFrame(vm, entryPoint, &returnValue);
		//stackFrame.PushArg(65);
		//stackFrame.PushArg(13);
		stackFrame.Call();
		printf("return: %d\n", returnValue);
		// End test code.
	}
	catch (const Bond::Exception &e)
	{
		error = true;
		fprintf(stderr, "%s\n", e.GetMessage());
	}

	return error ? 1 : 0;
}
