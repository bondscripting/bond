#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiooutputstream.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/error.h"

namespace Bond
{

InputStreamHandle StdioStreamFactory::CreateInputStream(const char *fileName)
{
	InputStreamHandle streamHandle;
	StdioFileHandle fileHandle = OpenFileHandle(fileName, "rb");

	if (fileHandle.IsBound())
	{
		streamHandle = mAllocator.AllocOwnedObject<StdioInputStream>(Bond::move(fileHandle));
	}
	else if (mDelegateFactory != nullptr)
	{
		streamHandle = mDelegateFactory->CreateInputStream(fileName);
	}

	if (!streamHandle && mThrowOnFailure)
	{
		BOND_ERROR_FORMAT("Failed to open file '%s' for reading.", fileName);
	}

	return streamHandle;
}


OutputStreamHandle StdioStreamFactory::CreateOutputStream(const char *fileName, bool append)
{
	OutputStreamHandle streamHandle;
	StdioFileHandle fileHandle = OpenFileHandle(fileName, append ? "ab" : "wb");

	if (fileHandle.IsBound())
	{
		streamHandle = mAllocator.AllocOwnedObject<StdioOutputStream>(Bond::move(fileHandle));
	}
	else if (mDelegateFactory != nullptr)
	{
		streamHandle = mDelegateFactory->CreateOutputStream(fileName, append);
	}

	if (!streamHandle && mThrowOnFailure)
	{
		BOND_ERROR_FORMAT("Failed to open file '%s' for writing.", fileName);
	}

	return streamHandle;
}


StdioFileHandle StdioStreamFactory::OpenFileHandle(const char *fileName, const char *mode)
{
	char fullPath[BOND_MAX_PATH_LENGTH];
	int length;

	if (mRootPath != nullptr)
	{
		length = snprintf(fullPath, BOND_MAX_PATH_LENGTH, "%s%s%c%s", BOND_FILESYSTEM_PREFIX, mRootPath, BOND_PATH_SEPARATOR_CHAR, fileName);
	}
	else
	{
		length = snprintf(fullPath, BOND_MAX_PATH_LENGTH, "%s%s", BOND_FILESYSTEM_PREFIX, fileName);
	}

	// Fail rather than open a truncated path, which could name a different file.
	if ((length < 0) || (length >= int(BOND_MAX_PATH_LENGTH)))
	{
		return StdioFileHandle();
	}

	return StdioFileHandle(fullPath, mode);
}

}
