#ifndef BOND_IO_OUTPUTSTREAM_H
#define BOND_IO_OUTPUTSTREAM_H

#include "bond/io/stream.h"
#include <cstdarg>

namespace Bond
{

class OutputStream: public Stream
{
public:
	virtual ~OutputStream() {}
	virtual void Print(const char *format, ...);
	virtual void VPrint(const char *format, va_list argList) = 0;
	virtual void Write(bu8_t c) = 0;
};

}

#endif
