#ifndef BOND_BUFFERED_TEXTWRITER_H
#define BOND_BUFFERED_TEXTWRITER_H

#include "bond/textwriter.h"

namespace Bond
{

class BufferedTextWriter: public TextWriter
{
public:
	BufferedTextWriter(char *buffer, int length);
	virtual ~BufferedTextWriter() {}

	virtual void Write(const char *format, ...);
	const char *GetBuffer() const { return mBuffer; }

private:
	char *mBuffer;
	int mLength;
	int mIndex;
};

}

#endif
