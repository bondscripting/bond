#ifndef BOND_EXCEPTION
#define BOND_EXCEPTION

#include <string.h>

namespace Bond
{

class Exception
{
public:
	static const size_t MESSAGE_BUFFER_LENGTH = 1024;

	Exception() { mMessage[0] = '\0'; }
	Exception(const char *message);
	Exception(const char *format, ...);

	const char *GetMessage() const { return mMessage; }

private:
	char mMessage[MESSAGE_BUFFER_LENGTH];
};

}

#endif
