#include "bond/assert.h"
#include "bond/defaultfileloader.h"
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
	unsigned char *data = NULL;
	size_t length = -1;
	bool valid = false;

	if (file != 0)
	{
		valid = true;
		const long pos = ftell(file);
		fseek(file, 0, SEEK_END);
		length = static_cast<size_t>(ftell(file) - pos);
		fseek(file, pos, SEEK_SET);

		if (length > 0)
		{
			data = mAllocator.Alloc<unsigned char>(length);
			fread(data, sizeof(char), length, file);
		}
	}

	return FileData(data, length, valid);
}


void DefaultFileLoader::DisposeFile(FileData &fileData)
{
	if (fileData.mValid)
	{
		mAllocator.Free(fileData.mData);
		fileData = FileData();
	}
}

}
