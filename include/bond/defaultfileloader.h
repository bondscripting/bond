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
	explicit DefaultFileLoader(Allocator &allocator): mAllocator(allocator) {}
	virtual ~DefaultFileLoader() {}

	virtual FileData LoadFile(const char *fileName);
	virtual void DisposeFile(FileData &fileData);

	FileData LoadFile(FILE *file);

private:
	Allocator &mAllocator;
};

}

#endif
