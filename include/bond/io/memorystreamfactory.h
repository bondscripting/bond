#ifndef BOND_IO_MEMORYSTREAMFACTORY_H
#define BOND_IO_MEMORYSTREAMFACTORY_H

#include "bond/io/streamfactory.h"

namespace Bond
{

class DataViewIndex;
class MemoryInputStream;

/// \addtogroup io
/// @{

/// \brief A concrete implementation of StreamFactory that instantiates MemoryInputStreams
/// for a fixed collection of DataViews.
/// \ingroup io
class MemoryStreamFactory: public StreamFactory
{
public:
	/// \brief Constructs a MemoryStreamFactory object.
	/// \param allocator The memory allocator from which MemoryInputStreams are allocated.
	/// \param index The index of DataViews from which MemoryInputStreams are instantiated.
	/// \param delegateFactory If this factory cannot provide an InputStream, this factory is
	///        used as a fallback.
	/// \param throwOnFailure If true, stream creation failures are reported by throwing an
	///        Exception. Otherwise, stream creation failures return a null handle.
	MemoryStreamFactory(Allocator &allocator, const DataViewIndex &index, StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mIndex(index),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	virtual ~MemoryStreamFactory() {}

	virtual InputStreamHandle CreateInputStream(const char *fileName) override;
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append) override;

	/// \brief Sets the fallback StreamFactory used when this factory cannot provide a stream.
	/// \param delegateFactory The fallback StreamFactory.
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
