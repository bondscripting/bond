#ifndef BOND_IO_STDIOFILEHANDLE_H
#define BOND_IO_STDIOFILEHANDLE_H

#include <cstdio>

namespace Bond
{

/// \brief A handle to a stdio FILE* to ensure that the file is properly closed using RAII.
/// \ingroup io
class StdioFileHandle
{
public:
	StdioFileHandle():
		mFile(nullptr)
	{}

	StdioFileHandle(FILE *file):
		mFile(file)
	{}

	StdioFileHandle(const char *fileName, const char *mode):
		mFile(fopen(fileName, mode))
	{}

	StdioFileHandle(StdioFileHandle &&other):
		mFile(other.mFile)
	{
		other.mFile = nullptr;
	}

	~StdioFileHandle()
	{
		if (mFile != nullptr)
		{
			fclose(mFile);
		}
	}

	StdioFileHandle &operator=(StdioFileHandle &&other)
	{
		if (this != &other)
		{
			if ((mFile != nullptr) && (mFile != other.mFile))
			{
				fclose(mFile);
			}
			mFile = other.mFile;
			other.mFile = nullptr;
		}
		return *this;
	}

	StdioFileHandle &operator=(FILE *file)
	{
		if ((mFile != nullptr) && (mFile != file))
		{
			fclose(mFile);
		}
		mFile = file;
		return *this;
	}

	StdioFileHandle(const StdioFileHandle &other) = delete;
	StdioFileHandle &operator=(const StdioFileHandle &other) = delete;

	FILE *GetFile() { return mFile; }

	bool IsBound() const { return mFile != nullptr; }

private:
	FILE *mFile;
};

}

#endif
