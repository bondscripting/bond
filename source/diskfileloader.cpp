#include "bond/io/diskfileloader.h"
#include "bond/systems/allocator.h"
#include "bond/systems/assert.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Bond
{

FileLoader::Handle DiskFileLoader::LoadFile(const char *fileName)
{
	Handle handle;
	FILE *file = NULL;
	const size_t MAX_PATH_LENGTH = 1024;
	char fullPath[MAX_PATH_LENGTH];

	if (mRootPath != NULL)
	{
		snprintf(fullPath, MAX_PATH_LENGTH, "%s%s%c%s", BOND_FILESYSTEM_PREFIX, mRootPath, BOND_PATH_SEPARATOR_CHAR, fileName);
	}
	else
	{
		snprintf(fullPath, MAX_PATH_LENGTH, "%s%s", BOND_FILESYSTEM_PREFIX, fileName);
	}

	file = fopen(fullPath, "rb");

	if (file != NULL)
	{
		handle = LoadFile(file);
		fclose(file);
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


FileLoader::Handle DiskFileLoader::LoadFile(FILE *file)
{
	Allocator::Handle<bu8_t> dataHandle(mAllocator);
	size_t length = 0;

	if (file != 0)
	{
		const long pos = ftell(file);
		fseek(file, 0, SEEK_END);
		length = size_t(ftell(file) - pos);
		fseek(file, pos, SEEK_SET);

		if (length > 0)
		{
			dataHandle.Reset(mAllocator.Alloc<bu8_t>(length));
			fread(dataHandle.Get(), sizeof(bu8_t), length, file);
		}
	}

	return Handle(FileData(dataHandle.Release(), length), this);
}


void DiskFileLoader::DisposeFile(FileData &fileData)
{
	mAllocator.Free(fileData.mData);
	fileData = FileData();
}

}
