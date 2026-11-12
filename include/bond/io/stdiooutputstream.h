#ifndef BOND_IO_STDIOOUTPUTSTREAM_H
#define BOND_IO_STDIOOUTPUTSTREAM_H

#include "bond/conf.h"
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
class BOND_API StdioOutputStream: public OutputStream
{
public:
	/// \brief Constructs a StdioOutputStream object.
	/// \param file The stdio FILE object to which output will be written. The file is not owned by
	///        the StdioOutputStream, so it is not closed when the StdioOutputStream is destroyed,
	///        but it is closed by Close().
	explicit StdioOutputStream(FILE *file):
		mFile(file)
	{}

	/// \brief Constructs a StdioOutputStream object that opens and owns the given file.
	///
	/// An existing file is truncated. Throws a Bond::Exception if the file fails to open.
	/// \param fileName The name of the file to open for writing in binary mode, encoded in UTF-8.
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

	/// \brief Destroys a StdioOutputStream object, closing the file if it is owned.
	virtual ~StdioOutputStream() {}

	StdioOutputStream(const StdioOutputStream &other) = delete;
	StdioOutputStream &operator=(const StdioOutputStream &other) = delete;

	/// \brief Closes the file if it is owned and takes over the file of another
	/// StdioOutputStream.
	/// \param other The StdioOutputStream object from which resources will be moved.
	/// \returns A reference to this StdioOutputStream.
	StdioOutputStream &operator=(StdioOutputStream &&other);

	/// \brief Tests whether this stream is currently bound to a FILE object.
	/// \returns True if this stream is bound to a FILE object.
	bool IsBound() const { return (mFile != nullptr); }

	/// \brief Closes the FILE object, whether or not it is owned, and unbinds the stream.
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
class BOND_API StdOutOutputStream: public StdioOutputStream
{
public:
	/// \brief Constructs a StdOutOutputStream object.
	StdOutOutputStream(): StdioOutputStream(stdout) {}

	/// \brief Destroys a StdOutOutputStream object. stdout is not closed.
	virtual ~StdOutOutputStream() {}
};


/// \brief A concrete implementation of StdioOutputStream bound to stderr.
/// \sa StdioOutputStream
class BOND_API StdErrOutputStream: public StdioOutputStream
{
public:
	/// \brief Constructs a StdErrOutputStream object.
	StdErrOutputStream(): StdioOutputStream(stderr) {}

	/// \brief Destroys a StdErrOutputStream object. stderr is not closed.
	virtual ~StdErrOutputStream() {}
};

}

#endif
