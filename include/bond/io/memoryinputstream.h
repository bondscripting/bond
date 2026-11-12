#ifndef BOND_IO_MEMORYINPUTSTREAM_H
#define BOND_IO_MEMORYINPUTSTREAM_H

#include "bond/conf.h"
#include "bond/io/inputstream.h"
#include "bond/types/types.h"

namespace Bond
{

/// \brief A concrete implementation of InputStream that performs input operations from an
/// in-memory buffer.
///
/// MemoryInputStream provides a non-owning view over a contiguous block of memory and reads data
/// from that buffer without copying it. Positions are clamped to the bounds of the buffer, and a
/// MemoryInputStream never enters an error state.
///
/// \sa InputStream, MemoryOutputStream
/// \ingroup io
class BOND_API MemoryInputStream: public InputStream
{
public:
	/// \brief Constructs a MemoryInputStream object.
	/// \param buffer A pointer to the memory buffer from which bytes will be read. The buffer is not
	///        copied and must remain valid for the lifetime of the MemoryInputStream.
	/// \param size The size of the memory buffer, in bytes.
	MemoryInputStream(const void *buffer, pos_t size);

	/// \brief Destroys a MemoryInputStream object.
	virtual ~MemoryInputStream() {}

	/// \brief Not implemented. Always throws a Bond::Exception.
	virtual void VScan(const char *format, va_list argList) override;

	virtual size_t Read(uint8_t *bytes, size_t numBytes) override;
	virtual int Read() override;

	virtual pos_t GetPosition() const override { return mCurrent; }

	/// \brief Returns the size of the memory buffer.
	virtual pos_t GetEndPosition() const override { return mSize; }

	/// \brief Sets the current position within the stream, clamped to the range [0, size].
	/// \param offset The absolute position to which the stream should be moved.
	virtual void SetPosition(off_t offset) override;

	virtual void SetPositionFromEnd(off_t offset) override;
	virtual void AddOffset(off_t offset) override;

	virtual bool IsEof() const override { return mCurrent >= mSize; }

	/// \brief Returns false, since a MemoryInputStream never enters an error state.
	virtual bool HasError() const override { return false; }

	/// \brief Does nothing, since a MemoryInputStream never enters an error state.
	virtual void ClearError() override {}

private:
	const uint8_t *mBuffer;
	pos_t mSize;
	pos_t mCurrent;
};

}

#endif
