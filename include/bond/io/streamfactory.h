#ifndef BOND_IO_STREAMFACTORY_H
#define BOND_IO_STREAMFACTORY_H

#include "bond/io/inputstream.h"
#include "bond/io/outputstream.h"
#include "bond/systems/allocator.h"

namespace Bond
{

/// \addtogroup io
/// @{

/// \brief An owning pointer to a dynamically allocated InputStream.
typedef Allocator::ObjectHandle<InputStream> InputStreamHandle;

/// \brief An owning pointer to a dynamically allocated OutputStream.
typedef Allocator::ObjectHandle<OutputStream> OutputStreamHandle;

/// \brief An abstract interface for creating file streams without being coupled to any particular
/// concrete type of stream.
class StreamFactory
{
public:
	virtual ~StreamFactory() {}

	/// \brief Returns an owning pointer to a dynamically allocated InputStream.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) = 0;

	/// \brief Returns an owning pointer to a dynamically allocated OutputStream.
	/// \param fileName The name of the file for which an Output stream is created.
	/// \param append Whether bytes are written to the end of an existing file.
	virtual OutputStreamHandle CreateOutputStream(const char *fileName, bool append) = 0;
};

/// @}

}

#endif
