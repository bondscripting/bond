#include "bond/io/stdiooutputstream.h"
#include "bond/systems/assert.h"

namespace Bond
{

 StdioOutputStream::StdioOutputStream(const char *fileName, const char *mode):
		mHandle(fileName, mode),
		mFile(mHandle.GetFile())
{
	if (!IsBound())
	{
		BOND_FAIL_FORMAT(("Failed to load file '%s'.", fileName));
	}
}


Stream::pos_t StdioOutputStream::GetEndPosition() const
{
	const auto pos = ftell(mFile);
	fseek(mFile, 0, SEEK_END);
	const auto end = ftell(mFile);
	fseek(mFile, pos, SEEK_SET);
	return pos_t(end);
}


StdioOutputStream &StdioOutputStream::operator=(StdioOutputStream &&other)
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
