#ifndef BOND_DEFAULTALLOCATOR_H
#define BOND_DEFAULTALLOCATOR_H

#include "bond/allocator.h"

namespace Bond
{

class DefaultAllocator: public Allocator
{
public:
	virtual ~DefaultAllocator() {}
	virtual void *Alloc(int size) { return static_cast<void *>(new char[size]); }
	virtual void Free(void *buffer) { delete [] static_cast<char *>(buffer); }
};

}

#endif
