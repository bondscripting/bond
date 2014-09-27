#include "bond/stl/algorithm.h"
#include "bond/vm/codesegment.h"

namespace Bond
{

const Function *CodeSegment::GetFunction(bu32_t functionHash) const
{
	const bu32_t *first = mFunctionLookup;
	const bu32_t *last = first + mFunctionCount;
	const bu32_t *target = lower_bound(first, last, functionHash);
	return ((target < last) && (*target == functionHash)) ? &mFunctionTable[target - first] : nullptr;
}


bi32_t CodeSegment::GetFunctionIndex(bu32_t functionHash) const
{
	const bu32_t *first = mFunctionLookup;
	const bu32_t *last = first + mFunctionCount;
	const bu32_t *target = lower_bound(first, last, functionHash);
	return ((target < last) && (*target == functionHash)) ? bi32_t(target - first) : bi32_t(-1);
}

}
