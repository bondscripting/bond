#ifndef BOND_TYPES_QUALIFIEDNAME_H
#define BOND_TYPES_QUALIFIEDNAME_H

#include "bond/types/types.h"

namespace Bond
{

class OutputStream;

/// \brief An immutable wrapper around a constant array of C-style strings to represent the
/// fully qualified name of a symbol in compiled Bond bytecode.
///
/// The number of elements in the wrapped array is not stored; instead, the array must be
/// terminated with a nullptr. QualifiedName does not perform any allocations; it contains a
/// pointer to an existing array of strings whose lifetime must superseed that of the
/// QualifiedName.
///
/// \ingroup types
class QualifiedName
{
public:
	/// \brief Constructs an empty QualifiedName. After construction, GetElement(0)
	/// returns nullptr.
	QualifiedName(): mElements(EMPTY_NAME) {}

	/// \brief Constructs a QualifiedName with the given array of C-style strings.
	/// \param elements Pointer to an array of string which are the elements of the qualified
	/// name. The last element of the array must be nullptr.
	explicit QualifiedName(const char *const *elements): mElements(elements) {}

	/// \brief Returns the string at the given index in the array of qualified name elements.
	///
	/// It it safe to call this function with the values in the range [0-n] where n is the first
	/// value for which this function returns nullptr.
	/// \param i The index of the element to be retrieved.
	const char *GetElement(size_t i) const { return mElements[i]; }

	/// \brief Writes the qualified name to the provided output stream with the elements
	/// separated by a '.' character.
	/// \param stream The stream to which the characters are written.
	void PrintTo(OutputStream &stream) const;

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with another one.
	///
	/// The algorithm iterates over each element of both QualifiedNames and calls strcmp() for each
	/// pair of elements. If strcmp() returns a non-zero value, that value is returned immediately.
	/// If the end of this QualifiedName is reached first, a negative value is returned. If the end
	/// of the other QualifiedName is reached first, a positive value is returned. If the end of
	/// both is reached at the same time, the value 0 is returned.
	/// \param other The QualifiedName against which this one is compared.
	/// \returns A negative value, positive value or 0 if this QualifiedName is lexicographically
	/// less than greater than or equal to the other one.
	int Compare(const QualifiedName &other) const;

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with a string.
	///
	/// This function returns the same value that the other overload of this function would return
	/// if the string passed in were to be treated as a QualifiedName whose elements are separated
	/// by the '.' character.
	/// \param other The string against which this QualifiedName is compared.
	/// \returns A negative value, positive value or 0 if this QualifiedName is lexicographically
	/// less than greater than or equal to the other one.
	int Compare(const char *other) const;

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with another one
	/// and returns whether they are equal.
	/// \param other The QualifiedName against which this one is compared.
	/// \returns The result of the expression: <code>Compare(other) == 0</code>.
	bool operator==(const QualifiedName &other) const { return Compare(other) == 0; }

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with a string.
	/// and returns whether they are equal.
	/// \param other The string against which this QualifiedName is compared.
	/// \returns The result of the expression: <code>Compare(other) == 0</code>.
	bool operator==(const char *other) const { return Compare(other) == 0; }

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with another one
	/// and returns whether this one is less.
	/// \param other The QualifiedName against which this one is compared.
	/// \returns The result of the expression: <code>Compare(other) < 0</code>.
	bool operator<(const QualifiedName &other) const { return Compare(other) < 0; }

	/// \brief Does an element-wise lexicographic comparison of this QualifiedName with a string.
	/// and returns whether this one is less.
	/// \param other The string against which this QualifiedName is compared.
	/// \returns The result of the expression: <code>Compare(other) < 0</code>.
	bool operator<(const char *other) const { return Compare(other) < 0; }

private:
	static const char *const EMPTY_NAME[1];
	const char *const *mElements;
};

}

#endif
