#ifndef BOND_IO_MEMORYSTREAMFACTORY_H
#define BOND_IO_MEMORYSTREAMFACTORY_H

#include "bond/io/streamfactory.h"

namespace Bond
{

struct DataViewIndex;
class MemoryInputStream;

/// \addtogroup io
/// @{

/// \brief A concrete implementation of StreamFactory that instantiates MemoryInputStreams
/// for a fixed collection of DataViews.
/// \ingroup io
class MemoryStreamFactory: public StreamFactory
{
public:
	MemoryStreamFactory(Allocator &allocator, const DataViewIndex &index, StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mIndex(index),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	virtual ~MemoryStreamFactory() {}

	virtual InputStreamHandle CreateInputStream(const char *fileName) override;
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append) override;

	void SetDelegateFactory(StreamFactory *delegateFactory) { mDelegateFactory = delegateFactory; }

private:
	Allocator &mAllocator;
	const DataViewIndex &mIndex;
	StreamFactory *mDelegateFactory;
	bool mThrowOnFailure;
};

/// @}

}

#endif
