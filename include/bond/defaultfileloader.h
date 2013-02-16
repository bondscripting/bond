#ifndef BOND_DEFAULTFILELOADER_H
#define BOND_DEFAULTFILELOADER_H

#include "bond/fileloader.h"
#include <stdio.h>

namespace Bond
{

class Allocator;

class DefaultFileLoader: public FileLoader
{
public:
	explicit DefaultFileLoader(Allocator &allocator, const char *rootPath = NULL, FileLoader *parentLoader = NULL):
		mAllocator(allocator),
		mRootPath(rootPath),
		mParentLoader(parentLoader)
	{}

	virtual ~DefaultFileLoader() {}

	virtual Handle LoadFile(const char *fileName);
	virtual void DisposeFile(FileData &fileData);

	Handle LoadFile(FILE *file);

	void SetRootPath(const char *rootPath) { mRootPath = rootPath; }
	void SetParentLoader(FileLoader *parentLoader) { mParentLoader = parentLoader; }

private:
	Allocator &mAllocator;
	const char *mRootPath;
	FileLoader *mParentLoader;
};

}

#endif
