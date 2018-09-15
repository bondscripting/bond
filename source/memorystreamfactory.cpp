#include "bond/io/memoryinputstream.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/assert.h"
#include "bond/types/dataview.h"

namespace Bond
{

InputStreamHandle MemoryStreamFactory::CreateInputStream(const char *fileName)
{
	InputStreamHandle handle;
	const StringView name(fileName);
	const StringView *firstName = mIndex.GetDataViewNames();
	const StringView *lastName = firstName + mIndex.GetNumDataViews();
	const StringView *result = lower_bound(firstName, lastName, name);

	if ((result != lastName) && (*result == name))
	{
		const auto dataView = mIndex.GetDataViews()[result - firstName];
		const auto data = dataView.GetData();
		const auto size = Stream::pos_t(dataView.GetLength());
		handle = mAllocator.AllocOwnedObject<MemoryInputStream>(data, size);
	}
	else if (mDelegateFactory != nullptr)
	{
		handle = mDelegateFactory->CreateInputStream(fileName);
	}

	if (!handle && mThrowOnFailure)
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for reading.", fileName));
	}

	return handle;
}


OutputStreamHandle MemoryStreamFactory::CreateOutputStream(const char *fileName, bool append)
{
	OutputStreamHandle handle;
	if (mDelegateFactory != nullptr)
	{
		handle = mDelegateFactory->CreateOutputStream(fileName, append);
	}

	if (!handle && mThrowOnFailure)
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for writing.", fileName));
	}

	return handle;
}

}
