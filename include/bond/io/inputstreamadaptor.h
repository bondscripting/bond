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
	/// \brief Constructs an InputStreamAdaptor object.
	/// \param stream The stream from which formatted input is read.
	explicit InputStreamAdaptor(InputStream *stream): mStream(stream), mFlags() {}

	/// \brief Returns the wrapped InputStream.
	InputStream *GetStream() const { return mStream; }

	/// \brief Scans a string from the wrapped InputStream.
	/// \param str Buffer where the scanned string is written.
	/// \param bufferSize The size of the destination buffer, in bytes.
	void ScanStr(char *str, uint32_t bufferSize);

	/// \brief Scans a boolean value from the wrapped InputStream.
	/// \returns The scanned boolean value.
	bool ScanBool();

	/// \brief Scans a signed character value from the wrapped InputStream.
	/// \returns The scanned character value.
	int8_t ScanChar();

	/// \brief Scans a signed 32-bit integer value from the wrapped InputStream.
	/// \returns The scanned integer value.
	int32_t ScanInt();

	/// \brief Scans an unsigned 32-bit integer value from the wrapped InputStream.
	/// \returns The scanned unsigned integer value.
	uint32_t ScanUInt();

	/// \brief Scans a signed 64-bit integer value from the wrapped InputStream.
	/// \returns The scanned long integer value.
	int64_t ScanLong();

	/// \brief Scans an unsigned 64-bit integer value from the wrapped InputStream.
	/// \returns The scanned unsigned long integer value.
	uint64_t ScanULong();

	/// \brief Scans a floating-point value from the wrapped InputStream.
	/// \returns The scanned floating-point value.
	double ScanDouble();

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

private:
	void Format(char *format, const char *specifier) const;

	InputStream *mStream;
	int mFlags;
};

}

#endif
