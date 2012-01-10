#ifndef BOND_DEFAULTFILELOADER_H
#define BOND_DEFAULTFILELOADER_H

#include "bond/allocator.h"
#include "bond/fileloader.h"

namespace Bond
{

class DefaultFileLoader
{
public:
	explicit DefaultFileLoader(Allocator &allocator): mAllocator(allocator) {}
	virtual ~DefaultFileLoader() {}

	virtual FileData LoadFile(const char *fileName);
	virtual void DisposeFile(FileData &fileData);

private:
	Allocator &mAllocator;
};

}

#endif
