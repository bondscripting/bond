#ifndef BOND_IO_STDIOINPUTSTREAM_H
#define BOND_IO_STDIOINPUTSTREAM_H

#include "bond/io/inputstream.h"
#include <cstdio>

namespace Bond
{

class StdioInputStream: public InputStream
{
public:
	StdioInputStream(FILE *file): mFile(file) {}
	virtual ~StdioInputStream() {}
	virtual void VScan(const char *format, va_list argList) override { vfscanf(mFile, format, argList); }
	virtual size_t Read(uint8_t *bytes, size_t numBytes) override { return fread(bytes, 1, numBytes, mFile); }
	virtual int Read() override { const int value = fgetc(mFile); return (value == EOF) ? Eof : value; }
	virtual pos_t GetPosition() const override { return ftell(mFile); }
	virtual void SetPosition(off_t offset) override { fseek(mFile, offset, SEEK_SET); }
	virtual void SetPositionFromEnd(off_t offset) override { fseek(mFile, offset, SEEK_END); }
	virtual void AddOffset(off_t offset) override { fseek(mFile, offset, SEEK_CUR); }

private:
	FILE *mFile;
};


class StdInInputStream: public StdioInputStream
{
public:
	StdInInputStream(): StdioInputStream(stdin) {}
	virtual ~StdInInputStream() {}
};

}

#endif
