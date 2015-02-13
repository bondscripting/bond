#include "bond/io/outputstream.h"
#include "bond/types/qualifiedname.h"
#include <cstring>

namespace Bond
{

void QualifiedName::PrintTo(OutputStream &stream) const
{
	const char *const *elements = mElements;
	while (*elements != nullptr)
	{
		stream.Print(*elements);
		++elements;
		if (*elements != nullptr)
		{
			stream.Print(".");
		}
	}
}


int QualifiedName::Compare(const QualifiedName &other) const
{
	const char *const *elements1 = mElements;
	const char *const *elements2 = other.mElements;
	int result = 0;

	if (elements1 != elements2)
	{
		const bool name1Empty = (elements1 == nullptr) || (*elements1 == nullptr);
		const bool name2Empty = (elements2 == nullptr) || (*elements2 == nullptr);
		if (name1Empty)
		{
			result = name2Empty ? 0 : -1;
		}
		else if (name2Empty)
		{
			result = 1;
		}
		else
		{
			while ((result == 0) && (*elements1 != nullptr) && (*elements2 != nullptr))
			{
				result = strcmp(*elements1++, *elements2++);
			}
			if (result == 0)
			{
				result = (*elements1 == *elements2) ? 0 : (*elements1 == nullptr) ? -1 : 1;
			}
		}
	}
	return result;
}


int QualifiedName::Compare(const char *other) const
{
	const char *const *elements = mElements;
	const bool name1Empty = (elements == nullptr) || (*elements == nullptr);
	const bool name2Empty = (other == nullptr) || (*other == '\0');
	int result = 0;

	if (name1Empty)
	{
		result = name2Empty ? 0 : -1;
	}
	else if (name2Empty)
	{
		result = 1;
	}
	else
	{
		const char *name1 = *elements++;
		const char *name2 = other;

		while ((result == 0) && (name1 != nullptr) && (*name2 != '\0'))
		{
			if ((*name1 == '\0') && (*name2 == '.'))
			{
				name1 = *elements++;
			}
			else
			{
				result = *name1 - *name2;
				++name1;
			}
			++name2;
		}
		if (result == 0)
		{
			result = ((*elements == nullptr) && (*name1 == '\0') && (*name2 == '\0')) ? 0 : (*name2 == '\0') ? 1 : -1;
		}
	}
	return result;
}

}
