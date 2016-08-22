#ifndef BOND_IO_MEMORYOUTPUTSTREAM_H
#define BOND_IO_MEMORYOUTPUTSTREAM_H

#include "bond/io/outputstream.h"
#include "bond/types/types.h"

namespace Bond
{

class MemoryOutputStream: public OutputStream
{
public:
	MemoryOutputStream(void *buffer, pos_t size);
	virtual ~MemoryOutputStream() {}

	virtual void VPrint(const char *format, va_list argList) override;
	virtual void Write(const uint8_t *bytes, size_t numBytes);
	virtual void Write(uint8_t c) override;

	virtual pos_t GetPosition() const override { return mCurrent; }
	virtual pos_t GetEndPosition() const override { return mEnd; }
	virtual void SetPosition(off_t offset) override;
	virtual void SetPositionFromEnd(off_t offset) override;
	virtual void AddOffset(off_t offset) override;

	virtual bool IsEof() const override { return mCurrent >= mSize; }

private:
	uint8_t *mBuffer;
	pos_t mSize;
	pos_t mCurrent;
	pos_t mEnd;
};

}

#endif
