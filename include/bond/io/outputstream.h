#ifndef BOND_IO_OUTPUTSTREAM_H
#define BOND_IO_OUTPUTSTREAM_H

#include "bond/io/stream.h"
#include <cstdarg>

namespace Bond
{

/// \brief An abstract interface for performing both unformatted and formatted output operations.
///
/// OutputStream extends Stream with functions for writing data either as raw bytes or according to
/// a printf-style format string.
///
/// \sa InputStream, Stream
/// \ingroup io
class OutputStream: public Stream
{
public:
	/// \brief Destroys an OutputStream object.
	virtual ~OutputStream() {}

	/// \brief Flushes any buffered output.
	///
	/// The default implementation does nothing. Concrete stream implementations may override this
	/// function when they buffer output internally or depend on an underlying buffered API.
	virtual void Flush() {}

	/// \brief Writes formatted output to the stream.
	/// \param format A printf-style format string describing the text to be written.
	/// \param ... Values to be formatted and written to the stream.
	virtual void Print(const char *format, ...);

	/// \brief Writes formatted output to the stream using a variable argument list.
	/// \param format A printf-style format string describing the text to be written.
	/// \param argList A variable argument list containing the values to be formatted.
	virtual void VPrint(const char *format, va_list argList) = 0;

	/// \brief Writes a sequence of bytes to the stream.
	/// \param bytes The buffer containing the bytes to be written.
	/// \param numBytes The number of bytes to write.
	virtual void Write(const uint8_t *bytes, size_t numBytes);

	/// \brief Writes a single byte to the stream.
	/// \param byte The byte to write.
	virtual void Write(uint8_t byte) = 0;
};

}

#endif
