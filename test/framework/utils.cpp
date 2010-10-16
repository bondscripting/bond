#include "framework/utils.h"
#include <stdio.h>
#include <stdarg.h>

namespace TestFramework
{

FileData ReadFile(const char *fileName)
{
	FileData fileData;
	FILE *file = fopen(fileName, "rb");

	if (file != 0)
	{
		fseek(file, 0, SEEK_END);
		fileData.length = static_cast<int>(ftell(file));
		fseek(file, 0, SEEK_SET);

		if (fileData.length > 0)
		{
			fileData.data = new char[fileData.length];
			fread(fileData.data, sizeof(char), fileData.length, file);
		}

		fclose(file);
	}

	return fileData;
}


void DisposeFile(FileData &fileData)
{
	delete [] fileData.data;
	fileData = FileData();
}

}
