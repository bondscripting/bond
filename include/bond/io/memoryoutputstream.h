#ifndef BOND_IO_MEMORYOUTPUTSTREAM_H
#define BOND_IO_MEMORYOUTPUTSTREAM_H

#include "bond/conf.h"
#include "bond/io/outputstream.h"
#include "bond/types/types.h"

namespace Bond
{

/// \brief A concrete implementation of OutputStream that performs output operations to an
/// in-memory buffer.
///
/// MemoryOutputStream provides a non-owning view over a writable memory buffer and writes data to
/// that buffer without reallocating it. Output that does not fit in the buffer is silently
/// discarded, positions are clamped to the bounds of the buffer, and a MemoryOutputStream never
/// enters an error state.
///
/// Formatted output written with Print() or VPrint() is null terminated, so a MemoryOutputStream
/// can be used to format a C-style string. Unformatted output written with Write() is not.
///
/// \sa MemoryInputStream, OutputStream
/// \ingroup io
class BOND_API MemoryOutputStream: public OutputStream
{
public:
	/// \brief Constructs a MemoryOutputStream object and writes a null terminator at the start of
	/// the buffer.
	/// \param buffer A pointer to the memory buffer to which bytes will be written. The buffer is
	///        not copied and must remain valid for the lifetime of the MemoryOutputStream.
	/// \param size The size of the memory buffer, in bytes. It must be at least 1.
	MemoryOutputStream(void *buffer, pos_t size);

	/// \brief Destroys a MemoryOutputStream object.
	virtual ~MemoryOutputStream() {}

	/// \brief Writes formatted output to the buffer followed by a null terminator, which is not
	/// counted in the current position. The output is truncated if it does not fit.
	/// \param format A printf-style format string describing the text to be written.
	/// \param argList A variable argument list containing the values to be formatted.
	virtual void VPrint(const char *format, va_list argList) override;

	virtual void Write(const uint8_t *bytes, size_t numBytes) override;
	virtual void Write(uint8_t c) override;

	virtual pos_t GetPosition() const override { return mCurrent; }

	/// \brief Returns the highest position that has been written to or moved to.
	virtual pos_t GetEndPosition() const override { return mEnd; }

	/// \brief Sets the current position within the stream, clamped to the range [0, size].
	/// \param offset The absolute position to which the stream should be moved.
	virtual void SetPosition(off_t offset) override;

	virtual void SetPositionFromEnd(off_t offset) override;
	virtual void AddOffset(off_t offset) override;

	/// \brief Returns whether the buffer is full.
	virtual bool IsEof() const override { return mCurrent >= mSize; }

	/// \brief Returns false, since a MemoryOutputStream never enters an error state.
	virtual bool HasError() const override { return false; }

	/// \brief Does nothing, since a MemoryOutputStream never enters an error state.
	virtual void ClearError() override {}

private:
	uint8_t *mBuffer;
	pos_t mSize;
	pos_t mCurrent;
	pos_t mEnd;
};

}

#endif
