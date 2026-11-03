#include "bond/io/path.h"

namespace Bond
{

static bool IsSeparator(char c)
{
	return (c == '/') || (c == '\\');
}


static bool IsDriveLetterPrefix(const StringView &path)
{
	if (path.size() < 2)
	{
		return false;
	}
	const char c = char(path[0] | 0x20);
	return (c >= 'a') && (c <= 'z') && (path[1] == ':');
}


namespace
{

// Appends the segments of a path to a normalized result, collapsing "." and ".." segments.
class SegmentAppender
{
public:
	SegmentAppender(Vector<char> &result, bool isRooted):
		mResult(result),
		mRootLength(result.size()),
		mNumSegments(0),
		mNumParentSegments(0),
		mIsRooted(isRooted)
	{}

	void Append(const StringView &str)
	{
		const size_t length = str.size();
		size_t pos = 0;
		while (pos < length)
		{
			size_t end = pos;
			while ((end < length) && !IsSeparator(str[end]))
			{
				++end;
			}

			const StringView segment = str.substr(pos, end - pos);
			if (segment.empty() || (segment == "."))
			{
				// Skip repeated separators and "." segments.
			}
			else if ((segment == "..") && (mNumSegments > mNumParentSegments))
			{
				PopSegment();
			}
			else if ((segment == "..") && mIsRooted)
			{
				// Cannot go above the root.
			}
			else
			{
				if (mNumSegments > 0)
				{
					mResult.push_back('/');
				}
				mResult.insert(mResult.end(), segment.begin(), segment.end());
				++mNumSegments;
				if (segment == "..")
				{
					++mNumParentSegments;
				}
			}

			pos = end + 1;
		}
	}

private:
	void PopSegment()
	{
		size_t newSize = mRootLength;
		for (size_t i = mResult.size(); i > mRootLength; --i)
		{
			if (mResult[i - 1] == '/')
			{
				newSize = i - 1;
				break;
			}
		}
		mResult.resize(newSize);
		--mNumSegments;
	}

	Vector<char> &mResult;
	size_t mRootLength;
	size_t mNumSegments;
	size_t mNumParentSegments;
	bool mIsRooted;
};

}


bool IsAbsolutePath(const StringView &path)
{
	return (!path.empty() && IsSeparator(path[0])) || IsDriveLetterPrefix(path);
}


StringView GetDirectory(const StringView &path)
{
	const size_t pos = path.find_last_of("/\\");
	return (pos == StringView::npos) ? StringView() : path.substr(0, pos + 1);
}


void NormalizePath(const StringView &directory, const StringView &path, Vector<char> &result)
{
	result.clear();

	// Preserve a drive letter, a root separator and a UNC prefix of the first non-empty part.
	const StringView first = directory.empty() ? path : directory;
	const size_t length = first.size();
	size_t prefixLength = 0;

	if (IsDriveLetterPrefix(first))
	{
		result.push_back(first[0]);
		result.push_back(':');
		prefixLength = 2;
	}

	const bool isRooted = (prefixLength < length) && IsSeparator(first[prefixLength]);
	if (isRooted)
	{
		result.push_back('/');
		++prefixLength;
		if ((prefixLength == 1) && (prefixLength < length) && IsSeparator(first[prefixLength]))
		{
			result.push_back('/');
			++prefixLength;
		}
	}

	SegmentAppender appender(result, isRooted);
	appender.Append(first.substr(prefixLength));
	if (!directory.empty())
	{
		appender.Append(path);
	}

	if (result.empty())
	{
		result.push_back('.');
	}

	result.push_back('\0');
}

}
