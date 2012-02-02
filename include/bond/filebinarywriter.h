#ifndef BOND_FILEBINARYWRITER_H
#define BOND_FILEBINARYWRITER_H

#include "bond/binarywriter.h"
#include <stdio.h>

namespace Bond
{

class FileBinaryWriter: public BinaryWriter
{
public:
	FileBinaryWriter(FILE *file): mFile(file) {}
	virtual ~FileBinaryWriter() {}
	virtual void Write(unsigned char c) { fputc(c, mFile); }
	virtual int GetPosition() const { return ftell(mFile); }
	virtual void SetPosition(int offset) { fseek(mFile, offset, SEEK_SET); }
	virtual void SetPositionFromEnd(int offset) { fseek(mFile, offset, SEEK_END); }
	virtual void AddOffset(int offset) { fseek(mFile, offset, SEEK_CUR); }

private:
	FILE *mFile;
};

}

#endif
