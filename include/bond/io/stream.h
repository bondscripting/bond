#ifndef BOND_IO_STREAM_H
#define BOND_IO_STREAM_H

#include "bond/types/types.h"

namespace Bond
{

/// \brief A base interface for performing IO operations.
/// \sa InputStream, OutputStream
/// \ingroup io
class Stream
{
public:
	static const int Eof = -1;
	typedef long pos_t;
	typedef long off_t;

	virtual ~Stream() {}

	virtual void Close() {}

	// Stream position.
	virtual pos_t GetPosition() const = 0;
	virtual pos_t GetEndPosition() const = 0;
	virtual void SetPosition(off_t offset) = 0;
	virtual void SetPositionFromEnd(off_t offset) = 0;
	virtual void AddOffset(off_t offset) = 0;

	// Status
	virtual bool IsEof() const = 0;
};

}

#endif
