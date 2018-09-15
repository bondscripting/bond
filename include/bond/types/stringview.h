#ifndef BOND_TYPES_STRINGVIEW_H
#define BOND_TYPES_STRINGVIEW_H

#include "bond/types/stringutil.h"

namespace Bond
{

class OutputStream;

/// \brief An immutable wrapper around a constant array of chars (a.k.a. a constant C-style string).
///
/// StringView can handle strings with embeded null characters as well as non-null terminated
/// strings. It also provides some comparison operators, making it useful for storing in
/// containers. StringView does not perform any allocations; it references an existing string
/// whose lifetime must superseed that of the StringView.
///
/// This class is a candidate for replacement by std::string_view.
///
/// \ingroup types
class StringView
{
public:
	/// \brief Constructs an empty StringView object. After construction, GetString() returns
	/// nullptr and GetLength() returns 0.
	StringView():
		mStr(nullptr),
		mLength(0)
	{}

	/// \brief Constructs a StringView over the given null-terminated C-style string.
	/// \param str The C-style string over which the view is created.
	StringView(const char *str):
		mStr(str)
	{
		mLength = StringLength(str);
	}

	/// \brief Constructs a StringView over the given array of characters that may contain null
	/// characters.
	/// \param str The array of characters over which the view is created.
	/// \param length The number of characters in the array.
	StringView(const char *str, size_t length):
		mStr(str),
		mLength(length)
	{}

	/// \brief Returns the pointer to the array of characters over which the view has been created.
	const char *GetString() const { return mStr; }

	/// \brief Returns the number of characters in the view.
	size_t GetLength() const { return mLength; }

	/// \brief Returns whether the view contains
	bool IsEmpty() const { return mLength == 0; }

	/// \brief Writes the characters of the view to the provided output stream.
	/// \param stream The stream to which the characters of the StringView are written.
	void PrintTo(OutputStream &stream) const;

	/// \brief Returns whether the StringView is lexicographically equal to the one passed in.
	/// \param other The StringView against which this one is compared.
	bool operator==(const StringView &other) const
	{
		return StringEqual(mStr, mLength, other.mStr, other.mLength);
	}

	/// \brief Returns whether the StringView is lexicographically less than the one passed in.
	/// \param other The StringView against which this one is compared.
	bool operator<(const StringView &other) const
	{
		return StringCompare(mStr, mLength, other.mStr, other.mLength) < 0;
	}

private:
	const char *mStr;
	size_t mLength;
};

}

#endif
