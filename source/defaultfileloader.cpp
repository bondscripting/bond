#include "bond/defaultfileloader.h"
#include <stdio.h>

namespace Bond
{

FileData DefaultFileLoader::LoadFile(const char *fileName)
{
	unsigned char *data = NULL;
	size_t length = -1;
	FILE *file = fopen(fileName, "rb");

	if (file != 0)
	{
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

	return FileData(data, length);
}


void DefaultFileLoader::DisposeFile(FileData &fileData)
{
	mAllocator.Free(fileData.mData);
	fileData = FileData();
}

}
