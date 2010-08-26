#ifndef BOND_ALLOCATOR_H
#define BOND_ALLOCATOR_H

namespace Bond
{

class Allocator
{
public:
	virtual ~Allocator() {}
	virtual void *Alloc(int size) = 0;
	virtual void Free(void *buffer) = 0;
};

}

#endif
