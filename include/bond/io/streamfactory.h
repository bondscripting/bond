#ifndef BOND_IO_STREAMFACTORY_H
#define BOND_IO_STREAMFACTORY_H

#include "bond/systems/allocator.h"

namespace Bond
{

class InputStream;

/// \addtogroup io
/// @{

/// \brief An owning pointer to a dynamically allocated InputStream.
typedef Allocator::Handle<InputStream> InputStreamHandle;

/// \brief An abstract interface for creating file streams without being coupled to any particular
/// concrete type of stream.
class StreamFactory
{
public:
	virtual ~StreamFactory() {}

	/// \brief Returns an owning pointer to a dynamically allocated InputStream.
	/// \param fileName The name of the file for which an input stream is created.
	virtual InputStreamHandle CreateInputStream(const char *fileName) = 0;
};

/// @}

}

#endif
