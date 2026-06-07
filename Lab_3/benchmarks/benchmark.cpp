/**
 * @file benchmark.cpp
 * @brief Benchmark sequential vs parallel merge sort.
 *
 * Measures wall-clock time for various input sizes and thread counts,
 * prints a table to stdout, and saves results to benchmarks/results.txt.
 *
 * Usage:
 * @code
 *   ./benchmark
 * @endcode
 */

#include "../sort/SequentialMergeSort.hpp"
#include "../sort/ParallelMergeSort.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <string>

static const std::vector<std::size_t> SIZES        = {1000, 5000, 10000, 50000, 100000};
static const std::vector<int>         THREAD_COUNTS = {1, 2, 4, 8};
static const int                      REPEAT        = 3;   ///< runs per config (median reported)

/** @brief Generate a vector of random integers. */
static std::vector<int> random_vec(std::size_t n, unsigned seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(-1'000'000, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

/** @brief Wall-clock time for one sort call (seconds). */
static double measure(const Sorter& sorter, const std::vector<int>& data) {
    auto copy = data;
    auto t0 = std::chrono::high_resolution_clock::now();
    sorter.sort(copy);
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(t1 - t0).count();
}

/** @brief Median wall-clock time over `repeat` runs. */
static double median_time(const Sorter& sorter,
                          const std::vector<int>& data,
                          int repeat) {
    std::vector<double> times;
    times.reserve(repeat);
    for (int i = 0; i < repeat; ++i)
        times.push_back(measure(sorter, data));
    std::sort(times.begin(), times.end());
    return times[repeat / 2];
}

struct Row {
    std::size_t size;
    std::string sorter_name;
    double      time_s;
    double      speedup;
};

int main() {
    std::vector<Row> rows;

    std::cout << std::left
              << std::setw(10) << "Size"
              << std::setw(55) << "Sorter"
              << std::right
              << std::setw(12) << "Time (s)"
              << std::setw(10) << "Speedup"
              << "\n"
              << std::string(90, '-') << "\n";

    SequentialMergeSort seqSorter;

    for (std::size_t sz : SIZES) {
        auto data = random_vec(sz);

        double seqTime = median_time(seqSorter, data, REPEAT);
        rows.push_back({sz, seqSorter.getName(), seqTime, 1.0});

        std::cout << std::left
                  << std::setw(10) << sz
                  << std::setw(55) << seqSorter.getName()
                  << std::right
                  << std::setw(12) << std::fixed << std::setprecision(4) << seqTime
                  << std::setw(9)  << std::fixed << std::setprecision(2) << 1.0 << "x"
                  << "\n";

        for (int t : THREAD_COUNTS) {
            ParallelMergeSort parSorter(t);
            double parTime = median_time(parSorter, data, REPEAT);
            double speedup = (parTime > 0) ? seqTime / parTime : 0.0;
            rows.push_back({sz, parSorter.getName(), parTime, speedup});

            std::cout << std::left
                      << std::setw(10) << ""
                      << std::setw(55) << parSorter.getName()
                      << std::right
                      << std::setw(12) << std::fixed << std::setprecision(4) << parTime
                      << std::setw(9)  << std::fixed << std::setprecision(2) << speedup << "x"
                      << "\n";
        }
        std::cout << "\n";
    }

    std::ofstream out("results.txt");
    if (out) {
        out << "# Benchmark Results – Parallel vs Sequential Merge Sort\n";
        out << "# Columns: input_size | sorter_name | median_time_s | speedup\n\n";
        out << std::left
            << std::setw(10) << "size"
            << std::setw(55) << "sorter"
            << std::right
            << std::setw(12) << "time_s"
            << std::setw(10) << "speedup"
            << "\n"
            << std::string(90, '-') << "\n";
        for (const auto& r : rows) {
            out << std::left
                << std::setw(10) << r.size
                << std::setw(55) << r.sorter_name
                << std::right
                << std::setw(12) << std::fixed << std::setprecision(4) << r.time_s
                << std::setw(9)  << std::fixed << std::setprecision(2) << r.speedup << "x"
                << "\n";
        }
        std::cout << "Results saved to results.txt\n";
    }

    return 0;
}
