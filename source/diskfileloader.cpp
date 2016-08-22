#include "bond/io/diskfileloader.h"
#include "bond/systems/allocator.h"
#include "bond/systems/assert.h"

namespace Bond
{

FileLoader::Handle DiskFileLoader::LoadFile(const char *fileName)
{
	Handle handle;
	FILE *file = nullptr;
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
	file = fopen(fullPath, "rb");

	if (file != nullptr)
	{
		handle = LoadFile(file);
		fclose(file);
	}
	else if (mDelegateLoader != nullptr)
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
	Allocator::Handle<uint8_t> dataHandle(mAllocator);
	size_t length = 0;

	if (file != 0)
	{
		const long pos = ftell(file);
		fseek(file, 0, SEEK_END);
		length = size_t(ftell(file) - pos);
		fseek(file, pos, SEEK_SET);

		if (length > 0)
		{
			dataHandle.reset(mAllocator.Alloc<uint8_t>(length));
			fread(dataHandle.get(), sizeof(uint8_t), length, file);
		}
	}

	return Handle(FileData(dataHandle.release(), length), this);
}


void DiskFileLoader::DisposeFile(FileData &fileData)
{
	mAllocator.Free(fileData.mData);
	fileData = FileData();
}

}
