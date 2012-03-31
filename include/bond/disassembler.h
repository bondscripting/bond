#ifndef BOND_DISASSEMBLER_H
#define BOND_DISASSEMBLER_H

#include "bond/types.h"

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

	void Disassemble(TextWriter &writer, unsigned char *byteCode, size_t length);

private:
	Allocator &mAllocator;
};

}

#endif
