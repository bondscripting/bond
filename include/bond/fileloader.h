#ifndef BOND_FILELOADER_H
#define BOND_FILELOADER_H

#include "bond/conf.h"

namespace Bond
{

struct FileData
{
	FileData(): mData(NULL), mLength(0), mValid(false) {}

	FileData(void *data, size_t length, bool valid):
		mData(data),
		mLength(length),
		mValid(valid)
	{}

	void *mData;
	size_t mLength;
	bool mValid;
};


class FileLoader
{
public:
	virtual ~FileLoader() {}

	virtual FileData LoadFile(const char *fileName) = 0;
	virtual void DisposeFile(FileData &fileData) = 0;
};

}

#endif
