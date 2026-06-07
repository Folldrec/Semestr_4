/**
 * @file SequentialMergeSort.hpp
 * @brief Sequential (single-threaded) merge sort.
 */

#pragma once
#include "Sorter.hpp"

/**
 * @brief Classic sequential merge sort.
 *
 * Time complexity:  O(n log n)
 * Space complexity: O(n)
 *
 * Runs entirely on the calling thread — no parallelism.
 * Used as the baseline for benchmarks and as the fallback
 * inside ParallelMergeSort for small sub-arrays.
 */
class SequentialMergeSort : public Sorter {
public:
    /**
     * @brief Sort a vector using sequential merge sort.
     * @param data  Input vector (not modified).
     * @return      New sorted vector.
     */
    std::vector<int> sort(const std::vector<int>& data) const override;

    std::string getName() const override { return "Sequential Merge Sort"; }

    /**
     * @brief Merge two sorted vectors into one sorted vector.
     *
     * Static so it can be reused by ParallelMergeSort without
     * creating an extra object.
     *
     * @param left   Left sorted sub-vector.
     * @param right  Right sorted sub-vector.
     * @return       Merged sorted vector.
     */
    static std::vector<int> merge(const std::vector<int>& left,
                                  const std::vector<int>& right);

private:
    std::vector<int> mergeSort(std::vector<int> data) const;
};
