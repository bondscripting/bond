#ifndef BOND_TOOLS_DISASSEMBLER_H
#define BOND_TOOLS_DISASSEMBLER_H

#include "bond/conf.h"
#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class InputStream;
class OutputStream;

/// \brief A disassembler for CBO files.
///
/// The Disassembler decodes CBO data and writes a human-readable representation of its contents,
/// including the bytecode of its functions, to an OutputStream. A single Disassembler can be
/// reused to disassemble many CBO buffers or streams.
///
/// \sa InputStream, OutputStream
/// \ingroup tools
class BOND_API Disassembler
{
public:
	/// \brief Constructs a Disassembler object.
	/// \param allocator The memory allocator used for temporary allocations during disassembly.
	explicit Disassembler(Allocator &allocator):
		mAllocator(allocator)
	{}

	/// \brief Destroys a Disassembler object.
	~Disassembler() {}

	/// \brief Decodes the CBO data in the given buffer and writes disassembled output.
	///
	/// The data is first validated with a CboValidator, which throws a Bond::Exception if it is
	/// invalid.
	/// \param byteCode Pointer to the start of the CBO data.
	/// \param length The number of bytes of CBO data.
	/// \param outputStream Stream where the disassembled output is written.
	void Disassemble(const void *byteCode, size_t length, OutputStream &outputStream);

	/// \brief Reads CBO data from the given stream, starting at its current position, and writes
	/// disassembled output.
	///
	/// The data is first validated with a CboValidator, which throws a Bond::Exception if it is
	/// invalid.
	/// \param cboStream Input stream containing CBO data to disassemble.
	/// \param outputStream Stream where the disassembled output is written.
	void Disassemble(InputStream &cboStream, OutputStream &outputStream);

	Disassembler &operator=(const Disassembler &other) = delete;

private:
	Allocator &mAllocator;
};

}

#endif
