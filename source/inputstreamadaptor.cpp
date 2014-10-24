#include "bond/api/libio.h"
#include "bond/io/inputstream.h"
#include "bond/io/inputstreamadaptor.h"
#include <cstdio>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Bond
{

void InputStreamAdaptor::ScanStr(char *str, uint32_t bufferSize)
{
	if (bufferSize > 0)
	{
		--bufferSize;
	}
	char format[32];
	snprintf(format, sizeof(format), "%%%" BOND_PRIu32 "s", bufferSize);
	mStream->Scan(format, str);
}


bool InputStreamAdaptor::ScanBool()
{
	// TODO: Handle IO::BoolAlpha
	int32_t value = 0;
	mStream->Scan("%" BOND_SCNi32, &value);
	return value != 0;
}


int8_t InputStreamAdaptor::ScanChar()
{
	char value = 0;
	mStream->Scan("%c", &value);
	return int8_t(value);
}


int32_t InputStreamAdaptor::ScanInt()
{
	int32_t value = 0;
	const char *format =
		((mFlags & IO::Dec) != 0) ? "%" BOND_SCNd32 :
		((mFlags & IO::Hex) != 0) ? "%" BOND_SCNx32 :
		((mFlags & IO::Oct) != 0) ? "%" BOND_SCNo32 :
		"%" BOND_SCNi32;
	mStream->Scan(format, &value);
	return value;
}


uint32_t InputStreamAdaptor::ScanUInt()
{
	uint32_t value = 0;
	const char *format =
		((mFlags & IO::Dec) != 0) ? "%" BOND_SCNu32 :
		((mFlags & IO::Hex) != 0) ? "%" BOND_SCNx32 :
		((mFlags & IO::Oct) != 0) ? "%" BOND_SCNo32 :
		"%" BOND_SCNi32;
	mStream->Scan(format, &value);
	return value;
}


int64_t InputStreamAdaptor::ScanLong()
{
	int64_t value = 0;
	const char *format =
		((mFlags & IO::Dec) != 0) ? "%" BOND_SCNd64 :
		((mFlags & IO::Hex) != 0) ? "%" BOND_SCNx64 :
		((mFlags & IO::Oct) != 0) ? "%" BOND_SCNo64 :
		"%" BOND_SCNi64;
	mStream->Scan(format, &value);
	return value;
}


uint64_t InputStreamAdaptor::ScanULong()
{
	uint64_t value = 0;
	const char *format =
		((mFlags & IO::Dec) != 0) ? "%" BOND_SCNu64 :
		((mFlags & IO::Hex) != 0) ? "%" BOND_SCNx64 :
		((mFlags & IO::Oct) != 0) ? "%" BOND_SCNo64 :
		"%" BOND_SCNi64;
	mStream->Scan(format, &value);
	return value;
}


double InputStreamAdaptor::ScanDouble()
{
	double value = 0.0;
	mStream->Scan("%lf", &value);
	return value;
}

}
