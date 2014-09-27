#ifndef BOND_IO_DISKFILELOADER_H
#define BOND_IO_DISKFILELOADER_H

#include "bond/io/fileloader.h"
#include <cstdio>

namespace Bond
{

class Allocator;

class DiskFileLoader: public FileLoader
{
public:
	explicit DiskFileLoader(Allocator &allocator, const char *rootPath = nullptr, FileLoader *delegateLoader = nullptr):
		mAllocator(allocator),
		mRootPath(rootPath),
		mDelegateLoader(delegateLoader)
	{}

	virtual ~DiskFileLoader() {}

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
