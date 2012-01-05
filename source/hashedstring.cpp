#include "bond/hashedstring.h"

namespace Bond
{

HashedString::HashedString(const char *str):
	mStr(str)
{
	int length = 0;
	if (str != 0)
	{
		const char *s = str;
		while (*s != '\0')
		{
			++s;
			++length;
		}
	}
	mLength = length;
	mHashCode = StringHash(length, str);
}


HashedString::HashedString(const char *str, int length):
	mStr(str),
	mLength(length)
{
	mHashCode = StringHash(length, str);
}


bool HashedString::operator==(const HashedString &other) const
{
	return
		(mHashCode == other.mHashCode) &&
		(mLength == other.mLength) &&
		StringEqual(mStr, other.mStr, mLength);
}

}
