#ifndef BOND_IO_STDIOSTREAMFACTORY_H
#define BOND_IO_STDIOSTREAMFACTORY_H

#include "bond/io/streamfactory.h"
#include "bond/io/stdiofilehandle.h"

namespace Bond
{

class StdioInputStream;

/// \addtogroup io
/// @{

/// \brief A concreate implementation of StreamFactory that instantiates StdioInputStreams.
/// \ingroup io
class StdioStreamFactory: public StreamFactory
{
public:
	explicit StdioStreamFactory(Allocator &allocator, const char *rootPath = nullptr, StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mRootPath(rootPath),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	virtual ~StdioStreamFactory() {}

	/// \brief Returns an owning pointer to a dynamically allocated StdioInputStream.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) override;

	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append = false) override;

	void SetRootPath(const char *rootPath) { mRootPath = rootPath; }
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
