#ifndef BOND_SYSTEMS_EXCEPTION_H
#define BOND_SYSTEMS_EXCEPTION_H

#include "bond/conf.h"

namespace Bond
{

/// \brief Exception thrown for all types of errors in the Bond Runtime Library.
/// \ingroup systems
class Exception
{
public:
	/// \brief Length of the buffer for the encapsulated error message.
	static const size_t MESSAGE_BUFFER_LENGTH = 2048;

	/// \brief Constructs an Exception with an empty message.
	Exception() { mMessage[0] = '\0'; }

	/// \brief Constructs an Exception with a formatted message. The arguments are forwarded
	/// to vsnprintf to write the message to the encapsulated error message buffer.
	Exception(const char *format, ...);

	/// \brief Returns the encapsulated error message.
	const char *GetMessage() const { return mMessage; }

private:
	char mMessage[MESSAGE_BUFFER_LENGTH];
};

}

#endif
