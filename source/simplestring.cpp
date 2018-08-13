#include "bond/io/outputstream.h"
#include "bond/types/simplestring.h"

namespace Bond
{

void StringView::PrintTo(Bond::OutputStream &stream) const
{
	for (size_t i = 0; i < mLength; ++i)
	{
		stream.Print("%c", mStr[i]);
	}
}

}
