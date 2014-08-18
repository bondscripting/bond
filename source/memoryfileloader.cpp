#include "bond/io/memoryfileloader.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/assert.h"

namespace Bond
{

FileLoader::Handle MemoryFileLoader::LoadFile(const char *fileName)
{
	Handle handle;
	const SimpleString name(fileName);
	const SimpleString *firstFileName = mIndex.mFileNames;
	const SimpleString *lastFileName = firstFileName + mIndex.mNumFiles;
	const SimpleString *result = lower_bound(firstFileName, lastFileName, name);

	if ((result != lastFileName) && (*result == name))
	{
		handle = Handle(mIndex.mFiles[result - firstFileName], this);
	}
	else if (mDelegateLoader != NULL)
	{
		handle = mDelegateLoader->LoadFile(fileName);
	}
	else
	{
		BOND_FAIL_FORMAT(("Failed to load file '%s'.", fileName));
	}

	return handle;
}

}
