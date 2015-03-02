#include "bond/api/libio.h"
#include "bond/io/outputstream.h"
#include "bond/io/outputstreamadaptor.h"
#include <cstdio>

namespace Bond
{

void OutputStreamAdaptor::Print(const char *str)
{
	const char *format = ((mFlags & IO::Left) != 0) ? "%-*s" : "%*s";
	mStream->Print(format, mWidth, str);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(bool value)
{
	if ((mFlags & IO::BoolAlpha) != 0)
	{
		Print(value ? "true" : "false");
	}
	else
	{
		Print(value ? int32_t(1) : int32_t(0));
	}
}


void OutputStreamAdaptor::Print(char value)
{
	const char *format = ((mFlags & IO::Left) != 0) ? "%-*c" : "%*c";
	mStream->Print(format, mWidth, value);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(int32_t value)
{
	char format[16];
	FormatInteger(format, BOND_PRId32, BOND_PRIx32, BOND_PRIo32);
	mStream->Print(format, mWidth, value);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(uint32_t value)
{
	char format[16];
	FormatInteger(format, BOND_PRIu32, BOND_PRIx32, BOND_PRIo32);
	mStream->Print(format, mWidth, value);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(int64_t value)
{
	char format[16];
	FormatInteger(format, BOND_PRId64, BOND_PRIx64, BOND_PRIo64);
	mStream->Print(format, mWidth, value);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(uint64_t value)
{
	char format[16];
	FormatInteger(format, BOND_PRIu64, BOND_PRIx64, BOND_PRIo64);
	mStream->Print(format, mWidth, value);
	mWidth = 0;
}


void OutputStreamAdaptor::Print(double value)
{
	char format[16];
	FormatFloat(format);
	mStream->Print(format, mWidth, mPrecision, value);
	mWidth = 0;
}


void OutputStreamAdaptor::FormatInteger(char *format, const char *dec, const char *hex, const char *oct) const
{
	*format++ = '%';
	if ((mFlags & IO::Left) != 0)
	{
		*format++ = '-';
	}
	if (((mFlags & IO::ShowBase) != 0) && ((mFlags & (IO::Hex | IO::Oct)) != 0))
	{
		*format++ = '#';
	}
	if ((mFlags & IO::Zero) != 0)
	{
		*format++ = '0';
	}

	*format++ = '*';

	const char *specifier = ((mFlags & IO::Hex) != 0) ? hex : ((mFlags & IO::Oct) != 0) ? oct : dec;
	while (*specifier)
	{
		*format++ = *specifier++;
	}

	*format++ = '\0';
}


void OutputStreamAdaptor::FormatFloat(char *format) const
{
	*format++ = '%';
	if ((mFlags & IO::Left) != 0)
	{
		*format++ = '-';
	}
	if ((mFlags & IO::ShowPoint) != 0)
	{
		*format++ = '#';
	}
	if ((mFlags & IO::Zero) != 0)
	{
		*format++ = '0';
	}

	*format++ = '*';
	*format++ = '.';
	*format++ = '*';

	if ((mFlags & IO::Fixed) != 0)
	{
		*format++ = 'f';
	}
	else if ((mFlags & IO::Scientific) != 0)
	{
		*format++ = 'e';
	}
	else
	{
		*format++ = 'g';
	}

	*format++ = '\0';
}

}
