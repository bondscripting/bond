#ifndef BOND_EXCEPTION_H
#define BOND_EXCEPTION_H

#include "bond/conf.h"

namespace Bond
{

class Exception
{
public:
	static const size_t MESSAGE_BUFFER_LENGTH = 2048;

	Exception() { mMessage[0] = '\0'; }
	Exception(const char *format, ...);

	const char *GetMessage() const { return mMessage; }

private:
	char mMessage[MESSAGE_BUFFER_LENGTH];
};

}

#endif
