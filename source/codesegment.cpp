#include "bond/stl/algorithm.h"
#include "bond/vm/codesegment.h"

namespace Bond
{

const Function *CodeSegment::GetFunction(uint32_t functionHash) const
{
	const uint32_t *first = mFunctionLookup;
	const uint32_t *last = first + mFunctionCount;
	const uint32_t *target = lower_bound(first, last, functionHash);
	return ((target < last) && (*target == functionHash)) ? &mFunctionTable[target - first] : nullptr;
}


int32_t CodeSegment::GetFunctionIndex(uint32_t functionHash) const
{
	const uint32_t *first = mFunctionLookup;
	const uint32_t *last = first + mFunctionCount;
	const uint32_t *target = lower_bound(first, last, functionHash);
	return ((target < last) && (*target == functionHash)) ? int32_t(target - first) : int32_t(-1);
}


const DataEntry *CodeSegment::GetDataEntry(uint32_t dataHash) const
{
	const uint32_t *first = mDataLookup;
	const uint32_t *last = first + mDataCount;
	const uint32_t *target = lower_bound(first, last, dataHash);
	return ((target < last) && (*target == dataHash)) ? &mDataTable[target - first] : nullptr;
}


int32_t CodeSegment::GetDataEntryIndex(uint32_t dataHash) const
{
	const uint32_t *first = mDataLookup;
	const uint32_t *last = first + mDataCount;
	const uint32_t *target = lower_bound(first, last, dataHash);
	return ((target < last) && (*target == dataHash)) ? int32_t(target - first) : int32_t(-1);
}

}
