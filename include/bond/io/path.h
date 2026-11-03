#ifndef BOND_IO_PATH_H
#define BOND_IO_PATH_H

#include "bond/conf.h"
#include "bond/stl/stringview.h"
#include "bond/stl/vector.h"

namespace Bond
{

/// \addtogroup io
/// @{

/// \brief Returns whether a path is absolute, that is whether it starts with a '/' or '\'
/// separator, or with a drive letter prefix such as "C:".
/// \param path The path to be tested.
BOND_API bool IsAbsolutePath(const StringView &path);

/// \brief Returns the directory part of a path, up to and including its last '/' or '\'
/// separator, or an empty StringView if the path has no separator.
/// \param path The path whose directory is returned.
BOND_API StringView GetDirectory(const StringView &path);

/// \brief Lexically normalizes the path formed by joining a directory and a path.
///
/// The result uses '/' separators, contains no "." segments or repeated separators, and has
/// "segment/.." pairs collapsed. Leading ".." segments are kept on relative paths and dropped at
/// the root of absolute paths. A drive letter prefix, a root separator and a UNC "//" prefix are
/// preserved. An empty result is written as ".". No file system calls are made, so the names
/// need not refer to files, and symbolic links are not resolved.
/// \param directory A directory to prepend to path, with an implied separator between them. It
///        may be empty.
/// \param path The path to be normalized.
/// \param result Receives the null terminated normalized path. Its previous contents are
///        discarded. No memory is allocated other than by result.
BOND_API void NormalizePath(const StringView &directory, const StringView &path, Vector<char> &result);

/// @}

}

#endif
