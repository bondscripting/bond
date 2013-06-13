#ifndef BOND_IO_FILEDATA_H
#define BOND_IO_FILEDATA_H

#include "bond/types/types.h"

namespace Bond
{

struct FileData
{
	FileData(): mData(NULL), mLength(0) {}

	FileData(const void *data, size_t length):
		mData(data),
		mLength(length)
	{}

	bool operator==(const FileData &other) const
	{
		return (mData == other.mData) && (mLength == other.mLength);
	}

	const void *mData;
	size_t mLength;
};

}

#endif
