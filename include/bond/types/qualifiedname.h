#ifndef BOND_TYPES_QUALIFIEDNAME_H
#define BOND_TYPES_QUALIFIEDNAME_H

#include "bond/types/types.h"

namespace Bond
{

class OutputStream;

class QualifiedName
{
public:
	QualifiedName(): mElements(nullptr) {}
	explicit QualifiedName(const char *const *elements): mElements(elements) {}

	const char *GetElement(size_t i) const { return mElements[i]; }

	void PrintTo(OutputStream &stream) const;

	int Compare(const QualifiedName &other) const;
	int Compare(const char *other) const;

	bool operator==(const QualifiedName &other) const { return Compare(other) == 0; }
	bool operator==(const char *other) const { return Compare(other) == 0; }
	bool operator<(const QualifiedName &other) const { return Compare(other) < 0; }
	bool operator<(const char *other) const { return Compare(other) < 0; }

private:
	const char *const *mElements;
};

}

#endif
