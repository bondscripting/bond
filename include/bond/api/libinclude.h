#ifndef BOND_API_LIBINCLUDE_H
#define BOND_API_LIBINCLUDE_H

#include "bond/conf.h"
#include "bond/types/dataview.h"

namespace Bond
{

#include "bond/private/libinclude_embedded.h"

/// \addtogroup api
/// @{

/// \brief An index of the source of the include files of the Bond Standard Library, which are
/// embedded in the Bond library.
///
/// The files are named "io.bond", "math.bond", "memory.bond", "string.bond" and "type.bond". The
/// index can be used to resolve include directives in Bond scripts without accessing the file
/// system, e.g. with a MemoryStreamFactory.
extern BOND_API const DataViewIndex INCLUDE_FILE_INDEX;

/// @}

}

#endif
