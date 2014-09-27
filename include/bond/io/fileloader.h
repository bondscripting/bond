#ifndef BOND_IO_FILELOADER_H
#define BOND_IO_FILELOADER_H

#include "bond/io/filedata.h"

namespace Bond
{

class FileLoader
{
public:
	class Handle
	{
	public:
		Handle():
			mFileData(),
			mFileLoader(nullptr)
		{}

		Handle(const FileData &fileData, FileLoader *fileLoader):
			mFileData(fileData),
			mFileLoader(fileLoader)
		{}

		Handle(Handle &&other):
			mFileData(other.mFileData),
			mFileLoader(other.mFileLoader)
		{
			other.mFileData = FileData();
			other.mFileLoader = nullptr;
		}

		~Handle()
		{
			if (mFileLoader != nullptr)
			{
				mFileLoader->DisposeFile(mFileData);
			}
		}

		Handle &operator=(Handle &&other)
		{
			FileData tempData = mFileData;
			mFileData = other.mFileData;
			other.mFileData = tempData;
			FileLoader *tempLoader = mFileLoader;
			mFileLoader = other.mFileLoader;
			other.mFileLoader = tempLoader;
			return *this;
		}

		FileData &Get() { return mFileData; }
		const FileData &Get() const { return mFileData; }

	private:
		Handle(const Handle &other);
		Handle &operator=(const Handle &other);

		FileData mFileData;
		FileLoader *mFileLoader;
	};


	virtual ~FileLoader() {}

	virtual Handle LoadFile(const char *fileName) = 0;
	virtual void DisposeFile(FileData &fileData) = 0;
};

}

#endif
