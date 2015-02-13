#include "bond/stl/algorithm.h"
#include "bond/vm/codesegment.h"
#include "bond/vm/vm.h"

namespace Bond
{

const Function *CodeSegment::GetFunction(const QualifiedName &qualifiedName) const
{
	auto comparator = [](const Function &a, const QualifiedName &b) { return a.mName < b; };
	const Function *first = mFunctionTable;
	const Function *last = first + mFunctionCount;
	const Function *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? &mFunctionTable[target - first] : nullptr;
}


const Function *CodeSegment::GetFunction(const char *qualifiedName) const
{
	auto comparator = [](const Function &a, const char *b) { return a.mName < b; };
	const Function *first = mFunctionTable;
	const Function *last = first + mFunctionCount;
	const Function *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? &mFunctionTable[target - first] : nullptr;
}


int32_t CodeSegment::GetFunctionIndex(const QualifiedName &qualifiedName) const
{
	auto comparator = [](const Function &a, const QualifiedName &b) { return a.mName < b; };
	const Function *first = mFunctionTable;
	const Function *last = first + mFunctionCount;
	const Function *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? int32_t(target - first) : int32_t(-1);
}


int32_t CodeSegment::GetFunctionIndex(const char *qualifiedName) const
{
	auto comparator = [](const Function &a, const char *b) { return a.mName < b; };
	const Function *first = mFunctionTable;
	const Function *last = first + mFunctionCount;
	const Function *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? int32_t(target - first) : int32_t(-1);
}


const DataEntry *CodeSegment::GetDataEntry(const QualifiedName &qualifiedName) const
{
	auto comparator = [](const DataEntry &a, const QualifiedName &b) { return a.mName < b; };
	const DataEntry *first = mDataTable;
	const DataEntry *last = first + mDataCount;
	const DataEntry *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? &mDataTable[target - first] : nullptr;
}


const DataEntry *CodeSegment::GetDataEntry(const char *qualifiedName) const
{
	auto comparator = [](const DataEntry &a, const char *b) { return a.mName < b; };
	const DataEntry *first = mDataTable;
	const DataEntry *last = first + mDataCount;
	const DataEntry *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? &mDataTable[target - first] : nullptr;
}


int32_t CodeSegment::GetDataEntryIndex(const QualifiedName &qualifiedName) const
{
	auto comparator = [](const DataEntry &a, const QualifiedName &b) { return a.mName < b; };
	const DataEntry *first = mDataTable;
	const DataEntry *last = first + mDataCount;
	const DataEntry *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? int32_t(target - first) : int32_t(-1);
}


int32_t CodeSegment::GetDataEntryIndex(const char *qualifiedName) const
{
	auto comparator = [](const DataEntry &a, const char *b) { return a.mName < b; };
	const DataEntry *first = mDataTable;
	const DataEntry *last = first + mDataCount;
	const DataEntry *target = lower_bound(first, last, qualifiedName, comparator);
	return ((target < last) && (target->mName == qualifiedName)) ? int32_t(target - first) : int32_t(-1);
}


void CodeSegment::CallStaticInitializers(VM &vm) const
{
	for (size_t i = 0; i < mStaticInitializerCount; ++i)
	{
		vm.CallVoidFunction(mStaticInitializerTable[i]);
	}
}

}
