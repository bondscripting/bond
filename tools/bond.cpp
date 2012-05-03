#include "bond/cboloader.h"
#include "bond/codesegment.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/list.h"
#include "bond/vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t MIN_STACK_SIZE = 1;
const size_t DEFAULT_STACK_SIZE = 32;
const char * const DEFAULT_ENTRY_POINT = "main";
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
				stackSize = static_cast<size_t>(atol(argv[i]));
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

	const size_t numCboFiles = cboFileNameList.size();
	Bond::FileData *cboFiles = allocator.Alloc<Bond::FileData>(numCboFiles);
	Bond::DefaultFileLoader fileLoader(allocator);
	StringList::Type::const_iterator it = cboFileNameList.begin();
	size_t index = 0;
	while (it != cboFileNameList.end())
	{
		cboFiles[index] = fileLoader.LoadFile(*it);
		if (cboFiles[index].mData == NULL)
		{
			fprintf(stderr, "Failed to load '%s'\n", *it);
			error = true;
		}
		++index;
		++it;
	}

	if (!error)
	{
		Bond::CboLoader cboLoader(allocator);
		const Bond::CodeSegment *codeSegment = cboLoader.Load(cboFiles, numCboFiles);
		Bond::VM vm(allocator, *codeSegment, stackSize);

		// Test code.
		Bond::bu32_t returnValue;
		Bond::VM::CallerStackFrame stackFrame(vm, "::TheSpace::Blah", &returnValue);
		stackFrame.PushArg(34);
		stackFrame.PushArg(45);
		stackFrame.Call();
		printf("return: %u\n", returnValue);
		// End test code.

		cboLoader.Dispose(codeSegment);
	}

	for (size_t i = 0; i < numCboFiles; ++i)
	{
		fileLoader.DisposeFile(cboFiles[i]);
	}

	return error ? 1 : 0;
}
