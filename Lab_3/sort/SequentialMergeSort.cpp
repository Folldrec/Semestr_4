/**
 * @file SequentialMergeSort.cpp
 * @brief Implementation of SequentialMergeSort.
 */

#include "SequentialMergeSort.hpp"
#include <algorithm>

std::vector<int> SequentialMergeSort::sort(const std::vector<int>& data) const {
    if (data.size() <= 1) return data;
    return mergeSort(data);
}

std::vector<int> SequentialMergeSort::mergeSort(std::vector<int> data) const {
    if (data.size() <= 1) return data;

    std::size_t mid = data.size() / 2;
    auto left  = mergeSort({data.begin(), data.begin() + mid});
    auto right = mergeSort({data.begin() + mid, data.end()});
    return merge(left, right);
}

std::vector<int> SequentialMergeSort::merge(const std::vector<int>& left,
                                             const std::vector<int>& right) {
    std::vector<int> result;
    result.reserve(left.size() + right.size());

    std::size_t i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j])
            result.push_back(left[i++]);
        else
            result.push_back(right[j++]);
    }
    while (i < left.size())  result.push_back(left[i++]);
    while (j < right.size()) result.push_back(right[j++]);
    return result;
}
