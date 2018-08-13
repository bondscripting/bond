#ifndef BOND_TYPES_DATACHUNK_H
#define BOND_TYPES_DATACHUNK_H

#include "bond/types/stringview.h"

namespace Bond
{

struct DataChunk
{
	DataChunk(): mData(nullptr), mLength(0) {}

	DataChunk(const void *data, size_t length):
		mData(data),
		mLength(length)
	{}

	DataChunk(const DataChunk &other) = default;

	bool operator==(const DataChunk &other) const
	{
		return (mData == other.mData) && (mLength == other.mLength);
	}

	const void *mData;
	size_t mLength;
};


struct DataChunkIndex
{
	DataChunkIndex(const StringView *chunkNames, const DataChunk *chunks, size_t numChunks):
		mChunkNames(chunkNames),
		mChunks(chunks),
		mNumChunks(numChunks)
	{}

	const StringView *mChunkNames;
	const DataChunk *mChunks;
	size_t mNumChunks;
};

}

#endif
