#ifndef BOND_IO_STDIOSTREAMFACTORY_H
#define BOND_IO_STDIOSTREAMFACTORY_H

#include "bond/conf.h"
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
class BOND_API StdioStreamFactory: public StreamFactory
{
public:
	/// \brief Constructs a StdioStreamFactory object.
	/// \param allocator The memory allocator from which StdioInputStreams and
	///        StdioOutputStreams are allocated.
	/// \param rootPath Optional root path that is prepended to each file name, followed by
	///        BOND_PATH_SEPARATOR_CHAR, when opening files. The string is not copied and must
	///        remain valid for the lifetime of the factory.
	/// \param delegateFactory If this factory cannot provide a stream, this factory is used as a
	///        fallback. May be null.
	/// \param throwOnFailure If true, stream creation failures are reported by throwing an
	///        Exception. Otherwise, stream creation failures return a null handle.
	explicit StdioStreamFactory(Allocator &allocator, const char *rootPath = nullptr,
			StreamFactory *delegateFactory = nullptr, bool throwOnFailure = true):
		mAllocator(allocator),
		mRootPath(rootPath),
		mDelegateFactory(delegateFactory),
		mThrowOnFailure(throwOnFailure)
	{}

	/// \brief Destroys a StdioStreamFactory object.
	virtual ~StdioStreamFactory() {}

	/// \brief Opens the given file for reading in binary mode and returns a StdioInputStream that
	/// owns it, or the stream created by the delegate factory if the file fails to open.
	///
	/// The opened path is BOND_FILESYSTEM_PREFIX followed by the root path, if any, and the file
	/// name. The file fails to open if the path is longer than BOND_MAX_PATH_LENGTH bytes,
	/// including the terminating null.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) override;

	/// \brief Opens the given file for writing in binary mode and returns a StdioOutputStream that
	/// owns it, or the stream created by the delegate factory if the file fails to open.
	///
	/// The opened path is formed as described for CreateInputStream().
	/// \param fileName The name of the file for which an output stream is created.
	/// \param append Whether bytes are written to the end of an existing file. Otherwise, an
	///        existing file is truncated.
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append = false) override;

	/// \brief Sets the root path prepended to file names when opening files.
	/// \param rootPath The new root path, or null to use file names as given. The string is not
	///        copied and must remain valid for the lifetime of the factory.
	void SetRootPath(const char *rootPath) { mRootPath = rootPath; }

	/// \brief Sets the fallback StreamFactory used when this factory cannot provide a stream.
	/// \param delegateFactory The fallback StreamFactory, or null.
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
