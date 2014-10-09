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

}
