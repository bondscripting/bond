#include "bond/allocator.h"
#include "bond/assert.h"
#include "bond/defaultfileloader.h"
#include "bond/types.h"
#include <stdio.h>

namespace Bond
{

FileData DefaultFileLoader::LoadFile(const char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	BOND_ASSERT_FORMAT(file != NULL, ("Failed to load file '%s'.", fileName));
	const FileData fileData = LoadFile(file);
	fclose(file);
	return fileData;
}


FileData DefaultFileLoader::LoadFile(FILE *file)
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

	return FileData(dataHandle.Release(), length);
}


void DefaultFileLoader::DisposeFile(FileData &fileData)
{
	mAllocator.Free(fileData.mData);
	fileData = FileData();
}

}
