#include "bond/io/outputstream.h"
#include "bond/stl/stringview.h"

namespace Bond
{

void PrintTo(const StringView &str, OutputStream &stream)
{
	for (char c : str)
	{
		stream.Print("%c", c);
	}
}

}
