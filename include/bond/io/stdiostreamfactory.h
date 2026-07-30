#ifndef BOND_IO_STDIOSTREAMFACTORY_H
#define BOND_IO_STDIOSTREAMFACTORY_H

#include "bond/io/streamfactory.h"
#include "bond/io/stdiofilehandle.h"

namespace Bond
{

class StdioInputStream;

/// \addtogroup io
/// @{

/// \brief A concrete implementation of StreamFactory that creates instances of StdioInputStream
/// and StdioOutputStream.
/// \ingroup io
class StdioStreamFactory: public StreamFactory
{
public:
	/// \brief Constructs a StdioStreamFactory object.
	/// \param allocator The memory allocator from which StdioInputStreams and
	///        StdioOutputStreams are allocated.
	/// \param rootPath Optional root path that is prepended to each file name when opening files.
	/// \param delegateFactory If this factory cannot provide an InputStream, this factory is
	///        used as a fallback.
	/// \param throwOnFailure If true, stream creation failures are reported by throwing an
	///        Exception. Otherwise, stream creation failures return a null handle.
	explicit StdioStreamFactory(Allocator &allocator, const char *rootPath = nullptr,
			StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mRootPath(rootPath),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	virtual ~StdioStreamFactory() {}

	virtual InputStreamHandle CreateInputStream(const char *fileName) override;
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append = false) override;

	/// \brief Sets the root path prepended to file names when opening files.
	/// \param rootPath The new root path, or null to use file names as given.
	void SetRootPath(const char *rootPath) { mRootPath = rootPath; }

	/// \brief Sets the fallback StreamFactory used when this factory cannot provide a stream.
	/// \param delegateFactory The fallback StreamFactory.
	void SetDelegateFactory(StreamFactory *delegateFactory) { mDelegateFactory = delegateFactory; }

private:
	StdioFileHandle OpenFileHandle(const char *fileName, const char *mode);

	Allocator &mAllocator;
	const char *mRootPath;
	StreamFactory *mDelegateFactory;
	bool mThrowOnFailure;
};

/// @}

}

#endif
