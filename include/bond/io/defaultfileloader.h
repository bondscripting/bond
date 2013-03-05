#ifndef BOND_DEFAULTFILELOADER_H
#define BOND_DEFAULTFILELOADER_H

#include "bond/io/fileloader.h"
#include <stdio.h>

namespace Bond
{

class Allocator;

class DefaultFileLoader: public FileLoader
{
public:
	explicit DefaultFileLoader(Allocator &allocator, const char *rootPath = NULL, FileLoader *delegateLoader = NULL):
		mAllocator(allocator),
		mRootPath(rootPath),
		mDelegateLoader(delegateLoader)
	{}

	virtual ~DefaultFileLoader() {}

	virtual Handle LoadFile(const char *fileName);
	virtual void DisposeFile(FileData &fileData);

	Handle LoadFile(FILE *file);

	void SetRootPath(const char *rootPath) { mRootPath = rootPath; }
	void SetDelegateLoader(FileLoader *delegateLoader) { mDelegateLoader = delegateLoader; }

private:
	Allocator &mAllocator;
	const char *mRootPath;
	FileLoader *mDelegateLoader;
};

}

#endif
