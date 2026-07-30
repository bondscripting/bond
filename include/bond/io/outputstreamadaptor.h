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

	/// \brief Constructs an OutputStreamAdaptor object.
	/// \param stream The stream to which formatted output is written.
	explicit OutputStreamAdaptor(OutputStream *stream):
		mStream(stream),
		mFlags(0),
		mPrecision(DEFAULT_PRECISION),
		mWidth(0)
	{}

	/// \brief Returns the wrapped OutputStream.
	OutputStream *GetStream() const { return mStream; }

	/// \brief Prints a null-terminated string to the wrapped OutputStream.
	/// \param str The string to print.
	void Print(const char *str);

	/// \brief Prints a boolean value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(bool value);

	/// \brief Prints a character value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(char value);

	/// \brief Prints a signed 32-bit integer value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(int32_t value);

	/// \brief Prints an unsigned 32-bit integer value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(uint32_t value);

	/// \brief Prints a signed 64-bit integer value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(int64_t value);

	/// \brief Prints an unsigned 64-bit integer value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(uint64_t value);

	/// \brief Prints a floating-point value to the wrapped OutputStream.
	/// \param value The value to print.
	void Print(double value);

	/// \brief Returns the current formatting flags.
	///
	/// The flags are bitfields defined by the IO::Flags enum.
	int GetFlags() const { return mFlags; }

	/// \brief Sets the specified formatting flags.
	///
	/// The flags are bitfields defined by the IO::Flags enum.
	/// \param flags The flags to set.
	void SetFlags(int flags) { mFlags |= flags; }

	/// \brief Unsets the specified formatting flags.
	///
	/// The flags are bitfields defined by the IO::Flags enum.
	/// \param flags The flags to unset.
	void UnsetFlags(int flags) { mFlags &= ~flags; }

	/// \brief Clears all formatting flags.
	///
	/// This clears any bitfield values previously set from the IO::Flags enum.
	void ClearFlags() { mFlags = 0; }

	/// \brief Returns the current floating-point precision.
	int GetPrecision() const { return mPrecision; }

	/// \brief Sets the floating-point precision.
	/// \param precision The precision to set.
	void SetPrecision(int precision) { mPrecision = precision; }

	/// \brief Returns the current field width.
	int GetWidth() const { return mWidth; }

	/// \brief Sets the field width.
	/// \param width The width to set.
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
