#ifndef TEST_FRAMEWORK_UTILS_H
#define TEST_FRAMEWORK_UTILS_H

namespace TestFramework
{

struct FileData
{
	FileData(): data(0), length(-1) {}

	char *data;
	int length;
};


FileData ReadFile(const char *fileName);
void DisposeFile(FileData &fileData);

}

#endif
