#ifndef BOND_IO_MEMORYFILELOADER_H
#define BOND_IO_MEMORYFILELOADER_H

#include "bond/io/fileloader.h"
#include "bond/types/simplestring.h"

namespace Bond
{

class MemoryFileLoader: public FileLoader
{
public:
	struct Index
	{
		Index(const SimpleString *fileNames, const FileData *files, size_t numFiles):
			mFileNames(fileNames),
			mFiles(files),
			mNumFiles(numFiles)
		{}

		const SimpleString *mFileNames;
		const FileData *mFiles;
		size_t mNumFiles;
	};

	explicit MemoryFileLoader(const Index &index, FileLoader *delegateLoader = nullptr):
		mIndex(index),
		mDelegateLoader(delegateLoader)
	{}

	virtual ~MemoryFileLoader() {}

	virtual Handle LoadFile(const char *fileName) override;
	virtual void DisposeFile(FileData &fileData) override {}

	void SetDelegateLoader(FileLoader *delegateLoader) { mDelegateLoader = delegateLoader; }

	MemoryFileLoader &operator=(const MemoryFileLoader &other) = delete;

private:
	const Index &mIndex;
	FileLoader *mDelegateLoader;
};

}

#endif
