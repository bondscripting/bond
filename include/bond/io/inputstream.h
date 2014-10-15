#ifndef BOND_IO_INPUTSTREAM_H
#define BOND_IO_INPUTSTREAM_H

#include "bond/io/stream.h"
#include <cstdarg>

namespace Bond
{

class InputStream: public Stream
{
public:
	virtual ~InputStream() {}
	virtual void Scan(const char *format, ...);
	virtual void VScan(const char *format, va_list argList) = 0;
	virtual size_t Read(uint8_t *bytes, size_t numBytes);
	virtual int Read() = 0;
};

}

#endif
