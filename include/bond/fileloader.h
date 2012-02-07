#ifndef BOND_FILELOADER_H
#define BOND_FILELOADER_H

#include "bond/conf.h"

namespace Bond
{

struct FileData
{
	FileData(): mData(NULL), mLength(-1) {}
	FileData(unsigned char *data, size_t length): mData(data), mLength(length) {}

	unsigned char *mData;
	size_t mLength;
};


class FileLoader
{
public:
	virtual ~FileLoader() {}

	virtual FileData ReadFile(const char *fileName) = 0;
	virtual void DisposeFile(FileData &fileData) = 0;
};

}

#endif
