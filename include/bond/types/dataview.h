#ifndef BOND_TYPES_DATAVIEW_H
#define BOND_TYPES_DATAVIEW_H

#include "bond/stl/stringview.h"

namespace Bond
{

/// \addtogroup types
/// @{

/// \brief An immutable object that encapsulates a pointer to a constant contiguous block of
/// memory representing arbitrary binary data and a count of the number of bytes of data.
///
/// DataView does not perform any allocations; it contains a pointer to an existing block of
/// memory whose lifetime must supersede that of the DataView.
class DataView
{
public:
	/// \brief Constructs an empty DataView. After construction, GetData() returns nullptr and
	/// GetLength() returns 0.
	DataView(): mData(nullptr), mLength(0) {}

	/// \brief Constructs a DataView over the given pointer to a block of memory.
	/// \param data The pointer to a block of memory over which the view is created.
	/// \param length The number of bytes of data.
	DataView(const void *data, size_t length):
		mData(data),
		mLength(length)
	{}

	DataView(const DataView &other) = default;

	/// \brief Returns whether this DataView points to the same number of bytes at the same
	/// memory address as another DataView.
	/// \param other The DataView against which this one is compared.
	bool operator==(const DataView &other) const
	{
		return (mData == other.mData) && (mLength == other.mLength);
	}

	/// \brief Returns the pointer to the block of memory over which the view has been created.
	const void *GetData() const { return mData; }

	/// \brief Returns the number of bytes of data in the view.
	size_t GetLength() const { return mLength; }

private:
	const void  *mData;
	size_t mLength;
};


/// \brief An immutable object that encapsulates an array of named constant DataView objects.
///
/// DataViewIndex does not perform any allocations; it contains a pointer to an existing array
/// of DataView objects and an existing array of StringView objects whose lifetimes must
/// supersede that of the DataView.
class DataViewIndex
{
public:
	/// \brief Constructs a DataViewIndex over the given array of DataViews. dataViewNames and
	/// dataViews must both point to arrays of numDataView elements.
	/// \param dataViewNames The array of names for the DataViews in the index.
	/// \param dataViews The array of DataViews over which the index is created.
	/// \param numDataViews The number of DataViews in the index.
	DataViewIndex(const StringView *dataViewNames, const DataView *dataViews, size_t numDataViews):
		mDataViewNames(dataViewNames),
		mDataViews(dataViews),
		mNumDataViews(numDataViews)
	{}

	/// \brief Returns the array of StringViews representing the names of the DataViews in the
	/// index.
	const StringView *GetDataViewNames() const { return mDataViewNames; }

	/// \brief Returns the array of DataViews over which the index has been created.
	const DataView *GetDataViews() const { return mDataViews; }

	/// \brief Returns the number of DataViews in the index.
	const size_t GetNumDataViews() const { return mNumDataViews; }

private:
	const StringView *mDataViewNames;
	const DataView *mDataViews;
	size_t mNumDataViews;
};

/// @}

}

#endif
