    /**
 * @file ParallelMergeSort.cpp
 * @brief Implementation of ParallelMergeSort.
 */

#include "ParallelMergeSort.hpp"
#include <thread>
#include <future>
#include <cmath>
#include <sstream>
#include <stdexcept>

ParallelMergeSort::ParallelMergeSort(int numThreads,
                                     std::size_t sequentialThreshold)
    : numThreads_(numThreads),
      seqThreshold_(sequentialThreshold)
{
    if (numThreads < 1)
        throw std::invalid_argument("numThreads must be >= 1");
    if (sequentialThreshold < 1)
        throw std::invalid_argument("sequentialThreshold must be >= 1");

    maxDepth_ = (numThreads > 1)
                ? static_cast<int>(std::floor(std::log2(numThreads)))
                : 0;
}

std::vector<int> ParallelMergeSort::sort(const std::vector<int>& data) const {
    if (data.size() <= 1) return data;
    return parallelSort(data, 0);
}

std::vector<int> ParallelMergeSort::parallelSort(std::vector<int> data,
                                                  int depth) const {
    if (data.size() <= seqThreshold_ || depth >= maxDepth_) {
        return seqSorter_.sort(data);
    }

    std::size_t mid = data.size() / 2;
    std::vector<int> leftHalf(data.begin(), data.begin() + mid);
    std::vector<int> rightHalf(data.begin() + mid, data.end());

    auto leftFuture = std::async(std::launch::async,
                                 [this, &leftHalf, depth]() {
                                     return parallelSort(std::move(leftHalf), depth + 1);
                                 });

    auto rightSorted = parallelSort(std::move(rightHalf), depth + 1);

    auto leftSorted = leftFuture.get();
    return SequentialMergeSort::merge(leftSorted, rightSorted);
}

std::string ParallelMergeSort::getName() const {
    std::ostringstream oss;
    oss << "Parallel Merge Sort (threads=" << numThreads_
        << ", threshold=" << seqThreshold_ << ")";
    return oss.str();
}
