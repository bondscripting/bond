#ifndef BOND_IO_INPUTSTREAMADAPTOR_H
#define BOND_IO_INPUTSTREAMADAPTOR_H

#include "bond/types/types.h"

namespace Bond
{

class InputStream;

/// \brief A wrapper around an InputStream that adds formatted input operations used within
/// the Bond Standard Library.
/// \ingroup io
class InputStreamAdaptor
{
public:
	explicit InputStreamAdaptor(InputStream *stream): mStream(stream), mFlags() {}

	InputStream *GetStream() const { return mStream; }

	void ScanStr(char *str, uint32_t bufferSize);
	bool ScanBool();
	int8_t ScanChar();
	int32_t ScanInt();
	uint32_t ScanUInt();
	int64_t ScanLong();
	uint64_t ScanULong();
	double ScanDouble();

	int GetFlags() const { return mFlags; }
	void SetFlags(int flags) { mFlags |= flags; }
	void UnsetFlags(int flags) { mFlags &= ~flags; }
	void ClearFlags() { mFlags = 0; }

private:
	void Format(char *format, const char *specifier) const;

	InputStream *mStream;
	int mFlags;
};

}

#endif
