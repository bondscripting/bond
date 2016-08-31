#include "bond/io/memoryinputstream.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/stl/algorithm.h"
#include "bond/systems/assert.h"
#include "bond/types/datachunk.h"

namespace Bond
{

InputStreamHandle MemoryStreamFactory::CreateInputStream(const char *fileName)
{
	InputStreamHandle handle;
	const SimpleString name(fileName);
	const SimpleString *firstName = mIndex.mChunkNames;
	const SimpleString *lastName = firstName + mIndex.mNumChunks;
	const SimpleString *result = lower_bound(firstName, lastName, name);

	if ((result != lastName) && (*result == name))
	{
		const auto chunk = mIndex.mChunks[result - firstName];
		const auto data = chunk.mData;
		const auto size = Stream::pos_t(chunk.mLength);
		handle = InputStreamHandle(mAllocator, mAllocator.AllocObject<MemoryInputStream>(data, size));
	}
	else if (mDelegateFactory != nullptr)
	{
		handle = mDelegateFactory->CreateInputStream(fileName);
	}
	else
	{
		BOND_FAIL_FORMAT(("Failed to open file '%s' for reading.", fileName));
	}

	return handle;
}

}
