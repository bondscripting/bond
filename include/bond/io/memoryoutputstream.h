#ifndef BOND_IO_MEMORYOUTPUTSTREAM_H
#define BOND_IO_MEMORYOUTPUTSTREAM_H

#include "bond/io/outputstream.h"
#include "bond/types/types.h"

namespace Bond
{

class MemoryOutputStream: public OutputStream
{
public:
	MemoryOutputStream(char *buffer, pos_t size);
	MemoryOutputStream(bu8_t *buffer, pos_t size);
	virtual ~MemoryOutputStream() {}
	virtual void VPrint(const char *format, va_list argList);
	virtual void Write(bu8_t c);
	virtual pos_t GetPosition() const { return mCurrent; }
	virtual void SetPosition(off_t offset);
	virtual void SetPositionFromEnd(off_t offset);
	virtual void AddOffset(off_t offset);
	const bu8_t *GetBuffer() const { return mBuffer; }

private:
	bu8_t *mBuffer;
	pos_t mSize;
	pos_t mCurrent;
	pos_t mEnd;
};

}

#endif
