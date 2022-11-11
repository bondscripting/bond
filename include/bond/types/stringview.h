#ifndef BOND_TYPES_STRINGVIEW_H
#define BOND_TYPES_STRINGVIEW_H

#include "bond/types/stringutil.h"

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
/// \param str The StringView whose characters are written.
/// \param stream The stream to which the characters of the StringView are written.
void PrintTo(const StringView &str, OutputStream &stream);

}

#endif
