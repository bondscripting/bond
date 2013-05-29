#ifndef BOND_MEMORYFILELOADER_H
#define BOND_MEMORYFILELOADER_H

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

	explicit MemoryFileLoader(const Index &index, FileLoader *delegateLoader = NULL):
		mIndex(index),
		mDelegateLoader(delegateLoader)
	{}

	virtual ~MemoryFileLoader() {}

	virtual Handle LoadFile(const char *fileName);
	virtual void DisposeFile(FileData &fileData) {}

	void SetDelegateLoader(FileLoader *delegateLoader) { mDelegateLoader = delegateLoader; }

private:
	const Index &mIndex;
	FileLoader *mDelegateLoader;
};

}

#endif
