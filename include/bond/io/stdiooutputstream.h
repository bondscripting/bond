#ifndef BOND_IO_STDIOOUTPUTSTREAM_H
#define BOND_IO_STDIOOUTPUTSTREAM_H

#include "bond/io/outputstream.h"
#include <cstdio>

namespace Bond
{

class StdioOutputStream: public OutputStream
{
public:
	StdioOutputStream(FILE *file): mFile(file) {}
	virtual ~StdioOutputStream() {}
	virtual void VPrint(const char *format, va_list argList) { vfprintf(mFile, format, argList); }
	virtual void Write(bu8_t c) { fputc(c, mFile); }
	virtual pos_t GetPosition() const { return ftell(mFile); }
	virtual void SetPosition(off_t offset) { fseek(mFile, offset, SEEK_SET); }
	virtual void SetPositionFromEnd(off_t offset) { fseek(mFile, offset, SEEK_END); }
	virtual void AddOffset(off_t offset) { fseek(mFile, offset, SEEK_CUR); }

private:
	FILE *mFile;
};


class StdOutOutputStream: public StdioOutputStream
{
public:
	StdOutOutputStream(): StdioOutputStream(stdout) {}
	virtual ~StdOutOutputStream() {}
};


class StdErrOutputStream: public StdioOutputStream
{
public:
	StdErrOutputStream(): StdioOutputStream(stderr) {}
	virtual ~StdErrOutputStream() {}
};

}

#endif
