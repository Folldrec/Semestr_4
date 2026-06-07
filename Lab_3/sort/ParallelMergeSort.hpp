/**
 * @file ParallelMergeSort.hpp
 * @brief Parallel merge sort using std::thread.
 */

#pragma once
#include "Sorter.hpp"
#include "SequentialMergeSort.hpp"
#include <stdexcept>

/**
 * @brief Parallel merge sort backed by std::thread.
 *
 * The list is recursively split in half.  Each half is sorted
 * concurrently on a new thread as long as the current recursion
 * depth is below @c maxDepth.  Once the depth limit is reached
 * (or the sub-array is small enough) the work falls through to
 * SequentialMergeSort to avoid spawning too many threads on tiny
 * inputs.
 *
 * The effective thread count is at most 2^maxDepth.  With the
 * default @c numThreads=4 the depth is set to log2(4)=2, so at
 * most 4 threads run concurrently.
 *
 * Example usage:
 * @code
 *   ParallelMergeSort sorter(4);          // up to 4 threads
 *   auto result = sorter.sort({5,3,8,1});
 * @endcode
 *
 * @param numThreads          Desired parallelism (must be >= 1).
 * @param sequentialThreshold Sub-array length below which the
 *                            sequential fallback is used (default 1000).
 */
class ParallelMergeSort : public Sorter {
public:
    explicit ParallelMergeSort(int numThreads = 4,
                               std::size_t sequentialThreshold = 1000);

    /**
     * @brief Sort a vector using parallel merge sort.
     * @param data  Input vector (not modified).
     * @return      New sorted vector.
     * @throws      std::invalid_argument if construction params are bad.
     */
    std::vector<int> sort(const std::vector<int>& data) const override;

    std::string getName() const override;

    /// @brief Number of threads configured for this sorter.
    int getNumThreads() const { return numThreads_; }

    /// @brief Sequential-fallback threshold.
    std::size_t getSequentialThreshold() const { return seqThreshold_; }

private:
    int         numThreads_;
    std::size_t seqThreshold_;
    int         maxDepth_;           ///< log2(numThreads)
    SequentialMergeSort seqSorter_;

    std::vector<int> parallelSort(std::vector<int> data, int depth) const;
};
