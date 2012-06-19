#include "bond/defaultfileloader.h"
#include <stdio.h>

namespace Bond
{

FileData DefaultFileLoader::LoadFile(const char *fileName)
{
	unsigned char *data = NULL;
	size_t length = -1;
	bool valid = false;
	FILE *file = fopen(fileName, "rb");

	if (file != 0)
	{
		valid = true;
		fseek(file, 0, SEEK_END);
		length = static_cast<size_t>(ftell(file));
		fseek(file, 0, SEEK_SET);

		if (length > 0)
		{
			data = mAllocator.Alloc<unsigned char>(length);
			fread(data, sizeof(char), length, file);
		}

		fclose(file);
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
