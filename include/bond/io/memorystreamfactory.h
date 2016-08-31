#ifndef BOND_IO_MEMORYSTREAMFACTORY_H
#define BOND_IO_MEMORYSTREAMFACTORY_H

#include "bond/io/streamfactory.h"

namespace Bond
{

struct DataChunkIndex;
class MemoryInputStream;

/// \addtogroup io
/// @{

/// \brief A concreate implementation of StreamFactory that instantiates MemoryInputStreams
/// for a collection of DataChunks.
/// \ingroup io
class MemoryStreamFactory: public StreamFactory
{
public:
	MemoryStreamFactory(Allocator &allocator, const DataChunkIndex &index, StreamFactory *delegateFactory = nullptr):
		mAllocator(allocator),
		mIndex(index),
		mDelegateFactory(delegateFactory)
	{}

	virtual ~MemoryStreamFactory() {}

	/// \brief Returns an owning pointer to a dynamically allocated MemoryInputStream.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) override;

private:
	Allocator &mAllocator;
	const DataChunkIndex &mIndex;
	StreamFactory *mDelegateFactory;
};

/// @}

}

#endif
