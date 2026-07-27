#ifndef BOND_IO_STDIOINPUTSTREAM_H
#define BOND_IO_STDIOINPUTSTREAM_H

#include "bond/io/inputstream.h"
#include "bond/io/stdiofilehandle.h"
#include "bond/stl/utility.h"

namespace Bond
{

/// \brief A concrete implementation of InputStream that performs input operations from a stdio
/// FILE*.
///
/// StdioInputStream adapts a C stdio FILE object to the InputStream interface. The stream may be
/// bound either to an existing FILE pointer or to a file opened and owned through a
/// StdioFileHandle.
///
/// \sa InputStream, StdioOutputStream
/// \ingroup io
class StdioInputStream: public InputStream
{
public:
	/// \brief Constructs a StdioInputStream object.
	/// \param file The stdio FILE object from which input will be read. The file is not owned by the
	///        StdioInputStream unless it is later replaced by an owning handle.
	explicit StdioInputStream(FILE *file):
		mFile(file)
	{}

	/// \brief Constructs a StdioInputStream object.
	/// \param fileName The name of the file to open for reading.
	explicit StdioInputStream(const char *fileName);

	/// \brief Constructs a StdioInputStream object.
	/// \param handle An owning file handle whose FILE object will be used for input operations.
	explicit StdioInputStream(StdioFileHandle &&handle):
		mHandle(Bond::move(handle)),
		mFile(mHandle.GetFile())
	{}

	/// \brief Move-constructs a StdioInputStream object.
	/// \param other The StdioInputStream object from which resources will be moved.
	StdioInputStream(StdioInputStream &&other):
		mHandle(Bond::move(other.mHandle)),
		mFile(other.mFile)
	{
		other.mFile = nullptr;
	}

	virtual ~StdioInputStream() {}

	StdioInputStream(const StdioInputStream &other) = delete;
	StdioInputStream &operator=(const StdioInputStream &other) = delete;

	StdioInputStream &operator=(StdioInputStream &&other);

	/// \brief Tests whether this stream is currently bound to a FILE object.
	/// \returns True if this stream is bound to a FILE object.
	bool IsBound() const { return (mFile != nullptr); }

	virtual void Close() override;

	virtual void VScan(const char *format, va_list argList) override { vfscanf(mFile, format, argList); }
	virtual size_t Read(uint8_t *bytes, size_t numBytes) override { return fread(bytes, 1, numBytes, mFile); }
	virtual int Read() override { const int value = fgetc(mFile); return (value == EOF) ? Eof : value; }

	virtual pos_t GetPosition() const override { return ftell(mFile); }
	virtual pos_t GetEndPosition() const override;
	virtual void SetPosition(off_t offset) override { fseek(mFile, offset, SEEK_SET); }
	virtual void SetPositionFromEnd(off_t offset) override { fseek(mFile, offset, SEEK_END); }
	virtual void AddOffset(off_t offset) override { fseek(mFile, offset, SEEK_CUR); }

	virtual bool IsEof() const override { return feof(mFile) != 0; }
	virtual bool HasError() const override { return ferror(mFile) != 0; }
	virtual void ClearError() override { clearerr(mFile); }

private:
	StdioFileHandle mHandle;
	FILE *mFile;
};


/// \brief A concrete implementation of StdioInputStream bound to stdin.
/// \sa StdioInputStream
class StdInInputStream: public StdioInputStream
{
public:
	/// \brief Constructs a StdInInputStream object.
	StdInInputStream(): StdioInputStream(stdin) {}
	virtual ~StdInInputStream() {}
};

}

#endif
