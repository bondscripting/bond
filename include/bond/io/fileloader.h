#ifndef BOND_IO_FILELOADER_H
#define BOND_IO_FILELOADER_H

#include "bond/types/datachunk.h"

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

		Handle(const DataChunk &fileData, FileLoader *fileLoader):
			mFileData(fileData),
			mFileLoader(fileLoader)
		{}

		Handle(Handle &&other):
			mFileData(other.mFileData),
			mFileLoader(other.mFileLoader)
		{
			other.mFileData = DataChunk();
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
			DataChunk tempData = mFileData;
			mFileData = other.mFileData;
			other.mFileData = tempData;
			FileLoader *tempLoader = mFileLoader;
			mFileLoader = other.mFileLoader;
			other.mFileLoader = tempLoader;
			return *this;
		}

		DataChunk &Get() { return mFileData; }
		const DataChunk &Get() const { return mFileData; }

	private:
		Handle(const Handle &other);
		Handle &operator=(const Handle &other);

		DataChunk mFileData;
		FileLoader *mFileLoader;
	};


	virtual ~FileLoader() {}

	virtual Handle LoadFile(const char *fileName) = 0;
	virtual void DisposeFile(DataChunk &fileData) = 0;
};

}

#endif
