#include "bond/algorithm.h"
#include "bond/codesegment.h"

namespace Bond
{

const Function *CodeSegment::GetFunction(bu32_t functionHash) const
{
	const bu32_t *first = mFunctionLookup;
	const bu32_t *last = first + mFunctionCount;
	const bu32_t *target = LowerBound(first, last, functionHash);
	return ((target < last) && (*target == functionHash)) ? &mFunctionList[target - first] : static_cast<const Function *>(NULL);
}

}
