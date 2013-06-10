#ifndef BOND_TOOLS_DISASSEMBLER_H
#define BOND_TOOLS_DISASSEMBLER_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class TextWriter;

class Disassembler
{
public:
	Disassembler(Allocator &allocator):
		mAllocator(allocator)
	{}
	~Disassembler() {}

	void Disassemble(TextWriter &writer, const void *byteCode, size_t length);

private:
	Allocator &mAllocator;
};

}

#endif
