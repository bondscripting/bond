#ifndef BOND_TYPES_STRINGVIEW_H
#define BOND_TYPES_STRINGVIEW_H

/// \file
/// \brief The non-owning string type used by Bond.
///
/// By default, Bond::StringView is an alias of std::string_view. It can be replaced by defining
/// USER_BOND_STRINGVIEW_HEADER. A replacement must provide a Bond::StringView class with the
/// interface of std::string_view.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_STRINGVIEW_HEADER

#include USER_BOND_STRINGVIEW_HEADER

#else

#include "bond/private/stringview.h"

#endif

namespace Bond
{

class OutputStream;

/// \brief Writes the characters of a StringView to the provided output stream.
/// \ingroup stl
/// \param str The StringView whose characters are written.
/// \param stream The stream to which the characters of the StringView are written.
BOND_API void PrintTo(const StringView &str, OutputStream &stream);

}

#endif
