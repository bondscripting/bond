#ifndef BOND_BUFFERED_TEXTWRITER_H
#define BOND_BUFFERED_TEXTWRITER_H

#include "bond/textwriter.h"
#include "bond/types.h"

namespace Bond
{

class BufferedTextWriter: public TextWriter
{
public:
	BufferedTextWriter(char *buffer, size_t length);
	virtual ~BufferedTextWriter() {}

	virtual void Write(const char *format, ...);
	const char *GetBuffer() const { return mBuffer; }

private:
	char *mBuffer;
	size_t mLength;
	size_t mIndex;
};

}

#endif
