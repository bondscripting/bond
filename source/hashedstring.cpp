#include "bond/hashedstring.h"

namespace Bond
{

HashedString::HashedString(const char *str):
	mStr(str)
{
	int length = 0;
	if (str != NULL)
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
		((mStr == other.mStr) || StringEqual(mStr, other.mStr, mLength));
}


bool HashedString::operator<(const HashedString &other) const
{
	return
		(mHashCode != other.mHashCode) ?
		(mHashCode < other.mHashCode) :
		((mLength != other.mLength) ?
		 (mLength < other.mLength) :
		 (StringCompare(mStr, other.mStr, mLength) < 0));
}

}