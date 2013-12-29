#ifndef BOND_IO_TEXTWRITER_H
#define BOND_IO_TEXTWRITER_H

#include <stdarg.h>

namespace Bond
{

class TextWriter
{
public:
	virtual ~TextWriter() {}
	void Write(const char *format, ...);
	virtual void VWrite(const char *format, va_list argList) = 0;
};

}

#endif
