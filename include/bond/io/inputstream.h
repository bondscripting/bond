#ifndef BOND_IO_INPUTSTREAM_H
#define BOND_IO_INPUTSTREAM_H

#include "bond/io/stream.h"
#include <cstdarg>

namespace Bond
{

/// \brief An abstract interface for performing both unformatted and formatted input operations.
///
/// InputStream extends Stream with functions for reading data either as raw bytes or according to
/// a scanf-style format string.
///
/// \sa OutputStream, Stream
/// \ingroup io
class InputStream: public Stream
{
public:
	/// \brief Destroys an InputStream object.
	virtual ~InputStream() {}

	/// \brief Reads formatted input from the stream.
	/// \param format A scanf-style format string describing the values to be read.
	/// \param ... Pointers to locations where the parsed values will be stored.
	virtual void Scan(const char *format, ...);

	/// \brief Reads formatted input from the stream using a variable argument list.
	/// \param format A scanf-style format string describing the values to be read.
	/// \param argList A variable argument list containing pointers to storage for the parsed values.
	virtual void VScan(const char *format, va_list argList) = 0;

	/// \brief Reads a sequence of bytes from the stream into a character buffer.
	/// \param bytes The destination buffer where the bytes will be written.
	/// \param numBytes The maximum number of bytes to read.
	/// \returns The number of bytes actually read from the stream.
	size_t Read(char *bytes, size_t numBytes) { return Read(reinterpret_cast<uint8_t *>(bytes), numBytes); }

	/// \brief Reads a sequence of bytes from the stream.
	/// \param bytes The destination buffer where the bytes will be written.
	/// \param numBytes The maximum number of bytes to read.
	/// \returns The number of bytes actually read from the stream.
	virtual size_t Read(uint8_t *bytes, size_t numBytes);

	/// \brief Reads a single byte from the stream.
	/// \returns The next byte in the stream, or Stream::Eof if the end of the stream is reached.
	virtual int Read() = 0;
};

}

#endif
