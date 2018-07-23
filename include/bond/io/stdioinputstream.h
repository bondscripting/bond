#ifndef BOND_IO_STDIOINPUTSTREAM_H
#define BOND_IO_STDIOINPUTSTREAM_H

#include "bond/io/inputstream.h"
#include "bond/io/stdiofilehandle.h"
#include "bond/stl/utility.h"

namespace Bond
{

/// \brief A concrete implementation of InputStream that performs input operations from a stdio
/// FILE*.
/// \ingroup io
class StdioInputStream: public InputStream
{
public:
	explicit StdioInputStream(FILE *file):
		mFile(file)
	{}

	explicit StdioInputStream(const char *fileName);

	explicit StdioInputStream(StdioFileHandle &&handle):
		mHandle(move(handle)),
		mFile(mHandle.GetFile())
	{}

	StdioInputStream(StdioInputStream &&other):
		mHandle(move(other.mHandle)),
		mFile(other.mFile)
	{
		other.mFile = nullptr;
	}

	virtual ~StdioInputStream() {}

	StdioInputStream(const StdioInputStream &other) = delete;
	StdioInputStream &operator=(const StdioInputStream &other) = delete;

	StdioInputStream &operator=(StdioInputStream &&other);

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


class StdInInputStream: public StdioInputStream
{
public:
	StdInInputStream(): StdioInputStream(stdin) {}
	virtual ~StdInInputStream() {}
};

}

#endif
