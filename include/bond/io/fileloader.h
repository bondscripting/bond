#ifndef BOND_IO_FILELOADER_H
#define BOND_IO_FILELOADER_H

#include "bond/io/filedata.h"
#include "bond/systems/resourcehandle.h"

namespace Bond
{

class FileLoader
{
public:
	class Deallocator
	{
	public:
		Deallocator(): mFileLoader(NULL) {}
		Deallocator(FileLoader *fileLoader): mFileLoader(fileLoader) {}

		void operator()(FileData &fileData) { if (mFileLoader != NULL) mFileLoader->DisposeFile(fileData); }

	private:
		FileLoader *mFileLoader;
	};


	class Handle: public ResourceHandle<FileData, Deallocator>
	{
	public:
		Handle(FileLoader &fileLoader, const FileData &fileData = FileData()):
			ResourceHandle<FileData, Deallocator>(fileData, Deallocator(&fileLoader))
		{}

		Handle(Handle &other):
			ResourceHandle<FileData, Deallocator>(other)
		{}

		Handle(const ResourceHandleProxy<FileData, Deallocator> &proxy):
			ResourceHandle<FileData, Deallocator>(proxy)
		{}

		Handle &operator=(const ResourceHandleProxy<FileData, Deallocator> &proxy)
		{
			ResourceHandle<FileData, Deallocator>::operator=(proxy);
			return *this;
		}
	};


	virtual ~FileLoader() {}

	virtual Handle LoadFile(const char *fileName) = 0;
	virtual void DisposeFile(FileData &fileData) = 0;
};

}

#endif
