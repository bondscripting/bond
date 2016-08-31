#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/assert.h"

namespace Bond
{

InputStreamHandle StdioStreamFactory::CreateInputStream(const char *fileName)
{
	InputStreamHandle streamHandle;
	const size_t MAX_PATH_LENGTH = 1024;
	char fullPath[MAX_PATH_LENGTH];

	if (mRootPath != nullptr)
	{
		snprintf(fullPath, MAX_PATH_LENGTH, "%s%s%c%s", BOND_FILESYSTEM_PREFIX, mRootPath, BOND_PATH_SEPARATOR_CHAR, fileName);
	}
	else
	{
		snprintf(fullPath, MAX_PATH_LENGTH, "%s%s", BOND_FILESYSTEM_PREFIX, fileName);
	}

	fullPath[MAX_PATH_LENGTH - 1] = '\0';
	StdioFileHandle fileHandle(fullPath, "rb");
	
	if (fileHandle.IsBound())
	{
		streamHandle = InputStreamHandle(mAllocator, mAllocator.AllocObject<StdioInputStream>(move(fileHandle)));
	}
	else if (mDelegateFactory != nullptr)
	{
		streamHandle = mDelegateFactory->CreateInputStream(fileName);
	}
	else
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for reading.", fileName));
	}

	return streamHandle;
}

}
