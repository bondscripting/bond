#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiooutputstream.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/assert.h"

namespace Bond
{

InputStreamHandle StdioStreamFactory::CreateInputStream(const char *fileName)
{
	InputStreamHandle streamHandle;
	StdioFileHandle fileHandle = OpenFileHandle(fileName, "rb");

	if (fileHandle.IsBound())
	{
		streamHandle = mAllocator.AllocOwnedObject<StdioInputStream>(move(fileHandle));
	}
	else if (mDelegateFactory != nullptr)
	{
		streamHandle = mDelegateFactory->CreateInputStream(fileName);
	}

	if (!streamHandle && mThrowOnFailure)
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for reading.", fileName));
	}

	return streamHandle;
}


OutputStreamHandle StdioStreamFactory::CreateOutputStream(const char *fileName, bool append)
{
	OutputStreamHandle streamHandle;
	StdioFileHandle fileHandle = OpenFileHandle(fileName, append ? "ab" : "wb");

	if (fileHandle.IsBound())
	{
		streamHandle = mAllocator.AllocOwnedObject<StdioOutputStream>(move(fileHandle));
	}
	else if (mDelegateFactory != nullptr)
	{
		streamHandle = mDelegateFactory->CreateOutputStream(fileName, append);
	}

	if (!streamHandle && mThrowOnFailure)
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for writing.", fileName));
	}

	return streamHandle;
}


StdioFileHandle StdioStreamFactory::OpenFileHandle(const char *fileName, const char *mode)
{
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
	return StdioFileHandle(fullPath, mode);
}

}
