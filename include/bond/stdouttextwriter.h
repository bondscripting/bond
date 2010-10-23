#ifndef BOND_STDOUTTEXTWRITER_H
#define BOND_STDOUTTEXTWRITER_H

#include "bond/textwriter.h"

namespace Bond
{

class StdoutTextWriter: public TextWriter
{
public:
	virtual ~StdoutTextWriter() {}
	virtual void Write(const char *format, ...);
};

}

#endif
