#ifndef BOND_IO_STDIOTEXTWRITER_H
#define BOND_IO_STDIOTEXTWRITER_H

#include "bond/io/textwriter.h"
#include <cstdio>

namespace Bond
{

class StdioTextWriter: public TextWriter
{
public:
	StdioTextWriter(FILE *file): mFile(file) {}
	virtual ~StdioTextWriter() {}
	virtual void VWrite(const char *format, va_list argList);

private:
	FILE *mFile;
};


class StdOutTextWriter: public StdioTextWriter
{
public:
	StdOutTextWriter(): StdioTextWriter(stdout) {}
	virtual ~StdOutTextWriter() {}
};


class StdErrTextWriter: public StdioTextWriter
{
public:
	StdErrTextWriter(): StdioTextWriter(stderr) {}
	virtual ~StdErrTextWriter() {}
};

}

#endif
