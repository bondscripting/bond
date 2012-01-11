#ifndef BOND_STDOUTTEXTWRITER_H
#define BOND_STDOUTTEXTWRITER_H

#include "bond/textwriter.h"

namespace Bond
{

class StdOutTextWriter: public TextWriter
{
public:
	virtual ~StdOutTextWriter() {}
	virtual void Write(const char *format, ...);
};

}

#endif
