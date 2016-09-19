#include "bond/io/stdioinputstream.h"
#include "bond/systems/assert.h"

namespace Bond
{

StdioInputStream::StdioInputStream(const char *fileName):
		mHandle(fileName, "rb"),
		mFile(mHandle.GetFile())
{
	if (!IsBound())
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for reading.", fileName));
	}
}


void StdioInputStream::Close()
{
	// If we have a file, but not handle, close it explicitly, otherwise let the handle take care of it.
	if (IsBound() && !mHandle.IsBound())
	{
		fclose(mFile);
	}
	mHandle = nullptr;
	mFile = nullptr;
}


Stream::pos_t StdioInputStream::GetEndPosition() const
{
	const auto pos = ftell(mFile);
	fseek(mFile, 0, SEEK_END);
	const auto end = ftell(mFile);
	fseek(mFile, pos, SEEK_SET);
	return pos_t(end);
}


StdioInputStream &StdioInputStream::operator=(StdioInputStream &&other)
{
	if (this != &other)
	{
		mHandle = move(other.mHandle);
		mFile = other.mFile;
		other.mFile = nullptr;
	}
	return *this;
}

}
