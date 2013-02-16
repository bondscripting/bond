#include "bond/allocator.h"
#include "bond/assert.h"
#include "bond/defaultfileloader.h"
#include "bond/types.h"
#include <stdio.h>

namespace Bond
{

FileLoader::Handle DefaultFileLoader::LoadFile(const char *fileName)
{
	Handle handle(*this);
	FILE *file = NULL;
	if (mRootPath != NULL)
	{
		const size_t MAX_PATH_LENGTH = 512;
		char fullPath[MAX_PATH_LENGTH];
		snprintf(fullPath, MAX_PATH_LENGTH, "%s%c%s", mRootPath, BOND_PATH_SEPARATOR_CHAR, fileName);
		file = fopen(fullPath, "rb");
	}
	else
	{
		file = fopen(fileName, "rb");
	}

	if (file != NULL)
	{
		handle = LoadFile(file);
		fclose(file);
	}
	else if (mParentLoader != NULL)
	{
		handle = mParentLoader->LoadFile(fileName);
	}
	else
	{
		BOND_FAIL_FORMAT(("Failed to load file '%s'.", fileName));
	}

	return handle;
}


FileLoader::Handle DefaultFileLoader::LoadFile(FILE *file)
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

	return Handle(*this, FileData(dataHandle.Release(), length));
}


void DefaultFileLoader::DisposeFile(FileData &fileData)
{
	mAllocator.Free(fileData.mData);
	fileData = FileData();
}

}
