#ifndef BOND_IO_STREAM_H
#define BOND_IO_STREAM_H

#include "bond/types/types.h"

namespace Bond
{

/// \brief A base interface for performing IO operations.
///
/// Stream provides the common positioning and status operations shared by all Bond input and
/// output streams. Concrete implementations may be backed by memory buffers, stdio FILE objects,
/// or other storage mechanisms.
///
/// \sa InputStream, OutputStream
/// \ingroup io
class Stream
{
public:
	/// \brief A special value returned by single-byte input functions to indicate end of stream.
	static const int Eof = -1;

	/// \brief A type used to represent absolute positions within a stream.
	typedef long pos_t;

	/// \brief A type used to represent relative offsets within a stream.
	typedef long off_t;

	/// \brief Destroys a Stream object.
	virtual ~Stream() {}

	/// \brief Closes the stream.
	///
	/// The default implementation does nothing. Concrete stream implementations may override this
	/// function to release owned resources or detach from the underlying data source or sink.
	virtual void Close() {}

	/// \brief Returns the current position within the stream.
	/// \returns The current absolute stream position.
	virtual pos_t GetPosition() const = 0;

	/// \brief Returns the end position of the stream.
	/// \returns The absolute position one past the last byte in the stream.
	virtual pos_t GetEndPosition() const = 0;

	/// \brief Sets the current position within the stream.
	/// \param offset The absolute position to which the stream should be moved.
	virtual void SetPosition(off_t offset) = 0;

	/// \brief Sets the current position relative to the end of the stream.
	/// \param offset The offset from the end position to which the stream should be moved.
	virtual void SetPositionFromEnd(off_t offset) = 0;

	/// \brief Advances the current position by the given offset.
	/// \param offset The signed amount by which the current position should be adjusted.
	virtual void AddOffset(off_t offset) = 0;

	/// \brief Tests whether the current position is at or beyond the end of the stream.
	/// \returns True if no more data can be read or written at the current position.
	virtual bool IsEof() const = 0;

	/// \brief Tests whether the stream is currently in an error state.
	/// \returns True if an error has occurred on the underlying stream.
	virtual bool HasError() const = 0;

	/// \brief Clears the current error state of the stream.
	virtual void ClearError() = 0;
};

}

#endif
