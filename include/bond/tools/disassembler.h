#ifndef BOND_TOOLS_DISASSEMBLER_H
#define BOND_TOOLS_DISASSEMBLER_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class InputStream;
class OutputStream;

/// \brief A disassembler for Bond bytecode and CBO streams.
///
/// The Disassembler decodes Bond bytecode and writes a human-readable representation to an
/// OutputStream. A single Disassembler can be reused to disassemble many bytecode buffers or CBO
/// streams.
///
/// \sa InputStream, OutputStream
/// \ingroup tools
class Disassembler
{
public:
	/// \brief Constructs a Disassembler object.
	/// \param allocator The memory allocator used for temporary allocations during disassembly.
	explicit Disassembler(Allocator &allocator):
		mAllocator(allocator)
	{}
	~Disassembler() {}

	/// \brief Decodes the given bytecode buffer and writes disassembled output.
	/// \param byteCode Pointer to the start of the bytecode buffer.
	/// \param length The number of bytes in the bytecode buffer.
	/// \param outputStream Stream where the disassembled output is written.
	void Disassemble(const void *byteCode, size_t length, OutputStream &outputStream);

	/// \brief Reads bytecode from the given CBO stream and writes disassembled output.
	/// \param cboStream Input stream containing CBO data to disassemble.
	/// \param outputStream Stream where the disassembled output is written.
	void Disassemble(InputStream &cboStream, OutputStream &outputStream);

	Disassembler &operator=(const Disassembler &other) = delete;

private:
	Allocator &mAllocator;
};

}

#endif
