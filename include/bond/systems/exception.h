#ifndef BOND_SYSTEMS_EXCEPTION_H
#define BOND_SYSTEMS_EXCEPTION_H

#include "bond/conf.h"

namespace Bond
{

/// \brief Exception thrown for all types of errors in the Bond Runtime Library.
///
/// Exception does not derive from std::exception. It is typically thrown with the macros in
/// bond/systems/error.h. Compiler errors in Bond source code are not reported with exceptions;
/// they are pushed to a CompilerErrorBuffer instead.
/// \ingroup systems
class BOND_API Exception
{
public:
	/// \brief Length of the buffer for the encapsulated error message.
	static const size_t MESSAGE_BUFFER_LENGTH = 2048;

	/// \brief Constructs an Exception with an empty message.
	Exception() { mMessage[0] = '\0'; }

	/// \brief Constructs an Exception with a formatted message. The arguments are forwarded
	/// to vsnprintf to write the message to the encapsulated error message buffer.
	/// \param format A printf-style format string. The formatted message is truncated if it does
	///        not fit in MESSAGE_BUFFER_LENGTH bytes, including the terminating null.
	Exception(const char *format, ...);

	/// \brief Returns the encapsulated error message.
	const char *GetMessage() const { return mMessage; }

private:
	char mMessage[MESSAGE_BUFFER_LENGTH];
};

}

#endif
