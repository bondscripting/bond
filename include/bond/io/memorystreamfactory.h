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
	MemoryStreamFactory(Allocator &allocator, const DataChunkIndex &index, StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mIndex(index),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	virtual ~MemoryStreamFactory() {}

	/// \brief Returns an owning pointer to a dynamically allocated MemoryInputStream.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) override;

	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append) override;

	void SetDelegateFactory(StreamFactory *delegateFactory) { mDelegateFactory = delegateFactory; }

private:
	Allocator &mAllocator;
	const DataChunkIndex &mIndex;
	StreamFactory *mDelegateFactory;
	bool mThrowOnFailure;
};

/// @}

}

#endif
