#ifndef BOND_BUFFEREDBINARYWRITER_H
#define BOND_BUFFEREDBINARYWRITER_H

#include "bond/binarywriter.h"

namespace Bond
{

class BufferedBinaryWriter: public BinaryWriter
{
public:
	BufferedBinaryWriter(bu8_t *buffer, int length);
	virtual ~BufferedBinaryWriter() {}

	virtual void Write(bu8_t c);
	virtual int GetPosition() const { return mPosition; }
	virtual void SetPosition(int offset);
	virtual void SetPositionFromEnd(int offset);
	virtual void AddOffset(int offset);

private:
	bu8_t *mBuffer;
	int mLength;
	int mPosition;
	int mEnd;
};

}

#endif