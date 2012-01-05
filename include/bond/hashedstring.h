#ifndef BOND_HASHEDSTRING_H
#define BOND_HASHEDSTRING_H

#include "bond/stringutil.h"

namespace Bond
{

class HashedString
{
public:
	HashedString():
		mStr(0),
		mLength(0),
		mHashCode(STRING_HASH_SEED)
	{}

	HashedString(const char *str);
	HashedString(const char *str, int length);

	const char *GetString() const { return mStr; }
	int GetLength() const { return mLength; }
	bu32_t GetHashCode() const { return mHashCode; }

	bool operator==(const HashedString &other) const;

private:
	const char *mStr;
	int mLength;
	bu32_t mHashCode;
};

}


#endif
