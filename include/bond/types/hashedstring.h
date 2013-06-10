#ifndef BOND_TYPES_HASHEDSTRING_H
#define BOND_TYPES_HASHEDSTRING_H

#include "bond/types/simplestring.h"

namespace Bond
{

// String that stores its hash code so that it does not need to be recomputed.
class HashedString: public SimpleString
{
public:
	HashedString(): mHashCode(STRING_HASH_SEED) {}

	HashedString(const char *str):
		SimpleString(str)
	{
		mHashCode = StringHash(GetLength(), str);
	}

	HashedString(const char *str, size_t length):
		SimpleString(str, length)
	{
		mHashCode = StringHash(length, str);
	}

	bu32_t GetHashCode() const { return mHashCode; }

	bool operator==(const HashedString &other) const
	{
		return
			(mHashCode == other.mHashCode) &&
			SimpleString::operator==(other);
	}

	bool operator<(const HashedString &other) const
	{
		return
			(mHashCode != other.mHashCode) ?
			(mHashCode < other.mHashCode) :
			SimpleString::operator<(other);
	}

private:
	bu32_t mHashCode;
};

}


#endif
