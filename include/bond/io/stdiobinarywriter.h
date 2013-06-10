#ifndef BOND_IO_STDIOBINARYWRITER_H
#define BOND_IO_STDIOBINARYWRITER_H

#include "bond/io/binarywriter.h"
#include <stdio.h>

namespace Bond
{

class StdioBinaryWriter: public BinaryWriter
{
public:
	StdioBinaryWriter(FILE *file): mFile(file) {}
	virtual ~StdioBinaryWriter() {}
	virtual void Write(bu8_t c) { fputc(c, mFile); }
	virtual int GetPosition() const { return ftell(mFile); }
	virtual void SetPosition(int offset) { fseek(mFile, offset, SEEK_SET); }
	virtual void SetPositionFromEnd(int offset) { fseek(mFile, offset, SEEK_END); }
	virtual void AddOffset(int offset) { fseek(mFile, offset, SEEK_CUR); }

private:
	FILE *mFile;
};

}

#endif
