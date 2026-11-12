#ifndef BOND_STL_CSTDIO_H
#define BOND_STL_CSTDIO_H

/// \file
/// \brief String formatting functions used by Bond.
///
/// By default, snprintf and vsnprintf are brought into the Bond namespace from the C++ standard
/// library. They can be replaced, e.g. with stb_sprintf, by defining USER_BOND_CSTDIO_HEADER. A
/// replacement must declare both functions in the Bond namespace with the standard semantics. Bond
/// passes the format strings of OutputStream::Print through to vsnprintf, so a replacement must
/// support every conversion that Bond and its embedders use, including the BOND_PRI* macros.
///
/// File I/O is not covered here. It is abstracted by the InputStream, OutputStream and
/// StreamFactory interfaces instead.
///
/// \ingroup stl

#include "bond/conf.h"

#ifdef USER_BOND_CSTDIO_HEADER

#include USER_BOND_CSTDIO_HEADER

#else

#include "bond/private/cstdio.h"

#endif

#endif
