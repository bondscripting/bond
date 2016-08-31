#ifndef BOND_IO_OUTPUTSTREAM_H
#define BOND_IO_OUTPUTSTREAM_H

#include "bond/io/stream.h"
#include <cstdarg>

namespace Bond
{

/// \brief An abstract interface for performing both unformatted and formatted output operations.
/// \ingroup io
class OutputStream: public Stream
{
public:
	virtual ~OutputStream() {}
	virtual void Print(const char *format, ...);
	virtual void VPrint(const char *format, va_list argList) = 0;
	virtual void Write(const uint8_t *bytes, size_t numBytes);
	virtual void Write(uint8_t byte) = 0;
};

}

#endif
