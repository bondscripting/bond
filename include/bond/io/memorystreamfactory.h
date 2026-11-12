#ifndef BOND_IO_MEMORYSTREAMFACTORY_H
#define BOND_IO_MEMORYSTREAMFACTORY_H

#include "bond/conf.h"
#include "bond/io/streamfactory.h"

namespace Bond
{

class DataViewIndex;
class MemoryInputStream;

/// \addtogroup io
/// @{

/// \brief A concrete implementation of StreamFactory that instantiates MemoryInputStreams
/// for a fixed collection of DataViews.
///
/// A file name is looked up by exact match against the names in the DataViewIndex. The factory
/// cannot create output streams itself, so it always forwards CreateOutputStream to its delegate
/// factory.
/// \ingroup io
class BOND_API MemoryStreamFactory: public StreamFactory
{
public:
	/// \brief Constructs a MemoryStreamFactory object.
	/// \param allocator The memory allocator from which MemoryInputStreams are allocated.
	/// \param index The index of DataViews from which MemoryInputStreams are instantiated. The
	///        names in the index must be sorted in ascending order, since they are binary
	///        searched. The index is not copied and must remain valid for the lifetime of the
	///        factory.
	/// \param delegateFactory If this factory cannot provide a stream, this factory is used as a
	///        fallback. May be null.
	/// \param throwOnFailure If true, stream creation failures are reported by throwing an
	///        Exception. Otherwise, stream creation failures return a null handle.
	MemoryStreamFactory(Allocator &allocator, const DataViewIndex &index, StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mIndex(index),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	/// \brief Destroys a MemoryStreamFactory object.
	virtual ~MemoryStreamFactory() {}

	/// \brief Returns a MemoryInputStream over the DataView with the given name, or the stream
	/// created by the delegate factory if there is no such DataView.
	/// \param fileName The name of the DataView for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) override;

	/// \brief Returns the stream created by the delegate factory.
	/// \param fileName The name of the file for which an output stream is created.
	/// \param append Whether bytes are written to the end of an existing file.
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append) override;

	/// \brief Sets the fallback StreamFactory used when this factory cannot provide a stream.
	/// \param delegateFactory The fallback StreamFactory, or null.
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
