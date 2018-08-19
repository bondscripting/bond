#ifndef BOND_TYPES_DATAVIEW_H
#define BOND_TYPES_DATAVIEW_H

#include "bond/types/stringview.h"

namespace Bond
{

struct DataView
{
	DataView(): mData(nullptr), mLength(0) {}

	DataView(const void *data, size_t length):
		mData(data),
		mLength(length)
	{}

	DataView(const DataView &other) = default;

	bool operator==(const DataView &other) const
	{
		return (mData == other.mData) && (mLength == other.mLength);
	}

	const void *mData;
	size_t mLength;
};


struct DataViewIndex
{
	DataViewIndex(const StringView *dataViewNames, const DataView *dataViews, size_t numDataViews):
		mDataViewNames(dataViewNames),
		mDataViews(dataViews),
		mNumDataViews(numDataViews)
	{}

	const StringView *mDataViewNames;
	const DataView *mDataViews;
	size_t mNumDataViews;
};

}

#endif
