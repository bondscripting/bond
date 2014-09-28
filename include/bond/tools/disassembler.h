#ifndef BOND_TOOLS_DISASSEMBLER_H
#define BOND_TOOLS_DISASSEMBLER_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class OutputStream;

class Disassembler
{
public:
	Disassembler(Allocator &allocator):
		mAllocator(allocator)
	{}
	~Disassembler() {}

	void Disassemble(OutputStream &stream, const void *byteCode, size_t length);

private:
	Allocator &mAllocator;
};

}

#endif
