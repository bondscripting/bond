#ifndef BOND_PRIVATE_ALGORITHM_H
#define BOND_PRIVATE_ALGORITHM_H

#include <algorithm>

namespace Bond
{

template <typename RandomAccessIterator, typename Comparator>
void Sort(RandomAccessIterator first, RandomAccessIterator last, Comparator comparator)
{
	std::sort(first, last, comparator);
}


template <typename ForwardIterator, typename ValueType>
ForwardIterator LowerBound(ForwardIterator first, ForwardIterator last, const ValueType &value)
{
	return std::lower_bound(first, last, value);
}


template <typename InputIterator, typename ValueType>
InputIterator Find(InputIterator first, InputIterator last, const ValueType &value)
{
	return std::find(first, last, value);
}

}

#endif
