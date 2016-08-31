#ifndef BOND_IO_OUTPUTSTREAMADAPTOR_H
#define BOND_IO_OUTPUTSTREAMADAPTOR_H

#include "bond/types/types.h"

namespace Bond
{

class OutputStream;

/// \brief A wrapper around an OuputStream that adds formatted output operations used within
/// the Bond Standard Library.
/// \ingroup io
class OutputStreamAdaptor
{
public:
	static const int DEFAULT_PRECISION = 6;

	OutputStreamAdaptor(OutputStream *stream):
		mStream(stream),
		mFlags(0),
		mPrecision(DEFAULT_PRECISION),
		mWidth(0)
	{}

	OutputStream *GetStream() const { return mStream; }

	void Print(const char *str);
	void Print(bool value);
	void Print(char value);
	void Print(int32_t value);
	void Print(uint32_t value);
	void Print(int64_t value);
	void Print(uint64_t value);
	void Print(double value);

	int GetFlags() const { return mFlags; }
	void SetFlags(int flags) { mFlags |= flags; }
	void UnsetFlags(int flags) { mFlags &= ~flags; }
	void ClearFlags() { mFlags = 0; }

	int GetPrecision() const { return mPrecision; }
	void SetPrecision(int precision) { mPrecision = precision; }

	int GetWidth() const { return mWidth; }
	void SetWidth(int width) { mWidth = width; }

private:
	void FormatInteger(char *format, const char *dec, const char *hex, const char *oct) const;
	void FormatFloat(char *format) const;

	OutputStream *mStream;
	int mFlags;
	int mPrecision;
	int mWidth;
};

}

#endif
