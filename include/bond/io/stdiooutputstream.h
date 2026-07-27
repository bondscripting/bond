#ifndef BOND_IO_STDIOOUTPUTSTREAM_H
#define BOND_IO_STDIOOUTPUTSTREAM_H

#include "bond/io/outputstream.h"
#include "bond/io/stdiofilehandle.h"
#include "bond/stl/utility.h"

namespace Bond
{

/// \brief A concrete implementation of OutputStream that performs output operations to a stdio
/// FILE*.
///
/// StdioOutputStream adapts a C stdio FILE object to the OutputStream interface. The stream may be
/// bound either to an existing FILE pointer or to a file opened and owned through a
/// StdioFileHandle.
///
/// \sa OutputStream, StdioInputStream
/// \ingroup io
class StdioOutputStream: public OutputStream
{
public:
	/// \brief Constructs a StdioOutputStream object.
	/// \param file The stdio FILE object to which output will be written. The file is not owned by
	///        the StdioOutputStream unless it is later replaced by an owning handle.
	explicit StdioOutputStream(FILE *file):
		mFile(file)
	{}

	/// \brief Constructs a StdioOutputStream object.
	/// \param fileName The name of the file to open for writing.
	explicit StdioOutputStream(const char *fileName);

	/// \brief Constructs a StdioOutputStream object.
	/// \param handle An owning file handle whose FILE object will be used for output operations.
	explicit StdioOutputStream(StdioFileHandle &&handle):
		mHandle(Bond::move(handle)),
		mFile(mHandle.GetFile())
	{}

	/// \brief Move-constructs a StdioOutputStream object.
	/// \param other The StdioOutputStream object from which resources will be moved.
	StdioOutputStream(StdioOutputStream &&other):
		mHandle(Bond::move(other.mHandle)),
		mFile(other.mFile)
	{
		other.mFile = nullptr;
	}

	virtual ~StdioOutputStream() {}

	StdioOutputStream(const StdioOutputStream &other) = delete;
	StdioOutputStream &operator=(const StdioOutputStream &other) = delete;

	StdioOutputStream &operator=(StdioOutputStream &&other);

	/// \brief Tests whether this stream is currently bound to a FILE object.
	/// \returns True if this stream is bound to a FILE object.
	bool IsBound() const { return (mFile != nullptr); }

	virtual void Close() override;
	virtual void Flush() override { fflush(mFile); }

	virtual void VPrint(const char *format, va_list argList) override { vfprintf(mFile, format, argList); }
	virtual void Write(uint8_t c) override { fputc(c, mFile); }
	virtual void Write(const uint8_t *bytes, size_t numBytes) override { fwrite(bytes, 1, numBytes, mFile); }

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


/// \brief A concrete implementation of StdioOutputStream bound to stdout.
/// \sa StdioOutputStream
class StdOutOutputStream: public StdioOutputStream
{
public:
	/// \brief Constructs a StdOutOutputStream object.
	StdOutOutputStream(): StdioOutputStream(stdout) {}
	virtual ~StdOutOutputStream() {}
};


/// \brief A concrete implementation of StdioOutputStream bound to stderr.
/// \sa StdioOutputStream
class StdErrOutputStream: public StdioOutputStream
{
public:
	/// \brief Constructs a StdErrOutputStream object.
	StdErrOutputStream(): StdioOutputStream(stderr) {}
	virtual ~StdErrOutputStream() {}
};

}

#endif
