/**
 * @file test_sorters.cpp
 * @brief Unit tests for SequentialMergeSort and ParallelMergeSort.
 *
 * Uses a lightweight hand-rolled test framework so no external
 * dependency (GoogleTest, Catch2, …) is required.
 *
 * Covers:
 *  - Correct sorting: typical, empty, single element, two elements,
 *    already-sorted, reverse-sorted, duplicates, all-equal, negatives,
 *    mixed positive/negative, large random input.
 *  - Input not mutated.
 *  - Sequential and parallel results are identical.
 *  - Invalid constructor arguments throw std::invalid_argument.
 */

#include "../sort/SequentialMergeSort.hpp"
#include "../sort/ParallelMergeSort.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <cassert>
#include <functional>
#include <string>
#include <vector>

static int g_passed = 0;
static int g_failed = 0;

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            std::cerr << "  FAIL  " << __FILE__ << ":" << __LINE__ \
                      << "  expected equal\n"; \
            ++g_failed; return; \
        } \
    } while(0)

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "  FAIL  " << __FILE__ << ":" << __LINE__ \
                      << "  expected true: " #expr "\n"; \
            ++g_failed; return; \
        } \
    } while(0)

#define ASSERT_THROWS(expr) \
    do { \
        bool caught = false; \
        try { expr; } catch (const std::invalid_argument&) { caught = true; } \
        if (!caught) { \
            std::cerr << "  FAIL  " << __FILE__ << ":" << __LINE__ \
                      << "  expected std::invalid_argument\n"; \
            ++g_failed; return; \
        } \
    } while(0)

static void run_test(const std::string& name, std::function<void()> fn) {
    std::cout << "[ TEST ] " << name << "\n";
    int failed_before = g_failed;
    fn();
    if (g_failed == failed_before) {
        std::cout << "[  OK  ] " << name << "\n";
        ++g_passed;
    }
}

static std::vector<int> reference_sort(std::vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
}

static std::vector<int> random_vec(std::size_t n, int seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(-100000, 100000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

void test_seq_typical() {
    SequentialMergeSort s;
    std::vector<int> data = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_seq_empty() {
    SequentialMergeSort s;
    ASSERT_EQ(s.sort({}), std::vector<int>{});
}

void test_seq_single() {
    SequentialMergeSort s;
    ASSERT_EQ(s.sort({42}), (std::vector<int>{42}));
}

void test_seq_two_sorted() {
    SequentialMergeSort s;
    ASSERT_EQ(s.sort({1, 2}), (std::vector<int>{1, 2}));
}

void test_seq_two_reversed() {
    SequentialMergeSort s;
    ASSERT_EQ(s.sort({2, 1}), (std::vector<int>{1, 2}));
}

void test_seq_already_sorted() {
    SequentialMergeSort s;
    std::vector<int> data = {1,2,3,4,5,6,7,8,9,10};
    ASSERT_EQ(s.sort(data), data);
}

void test_seq_reverse_sorted() {
    SequentialMergeSort s;
    std::vector<int> data = {10,9,8,7,6,5,4,3,2,1};
    ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_seq_duplicates() {
    SequentialMergeSort s;
    std::vector<int> data = {3,1,4,1,5,9,2,6,5,3,5};
    ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_seq_all_equal() {
    SequentialMergeSort s;
    std::vector<int> data(10, 7);
    ASSERT_EQ(s.sort(data), data);
}

void test_seq_negatives() {
    SequentialMergeSort s;
    std::vector<int> data = {-5,-1,-10,0,3};
    ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_seq_large_random() {
    SequentialMergeSort s;
    auto data = random_vec(20000);
    ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_seq_no_mutation() {
    SequentialMergeSort s;
    std::vector<int> data = {3, 1, 2};
    auto copy = data;
    s.sort(data);
    ASSERT_EQ(data, copy);
}

void test_seq_get_name() {
    SequentialMergeSort s;
    ASSERT_TRUE(!s.getName().empty());
}

static std::vector<ParallelMergeSort> make_par_sorters() {
    return {
        ParallelMergeSort(1),
        ParallelMergeSort(2),
        ParallelMergeSort(4),
        ParallelMergeSort(8, 500),
    };
}

void test_par_typical() {
    std::vector<int> data = {5,3,8,1,9,2,7,4,6};
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_par_empty() {
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort({}), std::vector<int>{});
}

void test_par_single() {
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort({99}), (std::vector<int>{99}));
}

void test_par_already_sorted() {
    std::vector<int> data = {1,2,3,4,5,6,7,8,9,10};
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), data);
}

void test_par_reverse_sorted() {
    std::vector<int> data = {10,9,8,7,6,5,4,3,2,1};
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_par_duplicates() {
    std::vector<int> data = {3,1,4,1,5,9,2,6,5,3,5};
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_par_negatives() {
    std::vector<int> data = {-5,-1,-10,0,3};
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_par_large_random() {
    auto data = random_vec(20000);
    for (auto& s : make_par_sorters())
        ASSERT_EQ(s.sort(data), reference_sort(data));
}

void test_par_no_mutation() {
    ParallelMergeSort s;
    std::vector<int> data = {3,1,2};
    auto copy = data;
    s.sort(data);
    ASSERT_EQ(data, copy);
}

void test_par_invalid_threads_zero() {
    ASSERT_THROWS(ParallelMergeSort(0));
}

void test_par_invalid_threads_neg() {
    ASSERT_THROWS(ParallelMergeSort(-1));
}

void test_par_invalid_threshold_zero() {
    ASSERT_THROWS(ParallelMergeSort(4, 0));
}

void test_par_properties() {
    ParallelMergeSort s(3, 500);
    ASSERT_EQ(s.getNumThreads(), 3);
    ASSERT_EQ(s.getSequentialThreshold(), (std::size_t)500);
}

void test_par_get_name_contains_thread_count() {
    ParallelMergeSort s(4);
    ASSERT_TRUE(s.getName().find("4") != std::string::npos);
}

void test_equivalence() {
    SequentialMergeSort seq;
    ParallelMergeSort   par(4);
    std::mt19937 rng(123);
    for (int i = 0; i < 10; ++i) {
        std::size_t sz = rng() % 10000;
        auto data = random_vec(sz, static_cast<int>(rng()));
        ASSERT_EQ(seq.sort(data), par.sort(data));
    }
}

int main() {
    run_test("seq_typical",         test_seq_typical);
    run_test("seq_empty",           test_seq_empty);
    run_test("seq_single",          test_seq_single);
    run_test("seq_two_sorted",      test_seq_two_sorted);
    run_test("seq_two_reversed",    test_seq_two_reversed);
    run_test("seq_already_sorted",  test_seq_already_sorted);
    run_test("seq_reverse_sorted",  test_seq_reverse_sorted);
    run_test("seq_duplicates",      test_seq_duplicates);
    run_test("seq_all_equal",       test_seq_all_equal);
    run_test("seq_negatives",       test_seq_negatives);
    run_test("seq_large_random",    test_seq_large_random);
    run_test("seq_no_mutation",     test_seq_no_mutation);
    run_test("seq_get_name",        test_seq_get_name);

    run_test("par_typical",                      test_par_typical);
    run_test("par_empty",                        test_par_empty);
    run_test("par_single",                       test_par_single);
    run_test("par_already_sorted",               test_par_already_sorted);
    run_test("par_reverse_sorted",               test_par_reverse_sorted);
    run_test("par_duplicates",                   test_par_duplicates);
    run_test("par_negatives",                    test_par_negatives);
    run_test("par_large_random",                 test_par_large_random);
    run_test("par_no_mutation",                  test_par_no_mutation);
    run_test("par_invalid_threads_zero",         test_par_invalid_threads_zero);
    run_test("par_invalid_threads_neg",          test_par_invalid_threads_neg);
    run_test("par_invalid_threshold_zero",       test_par_invalid_threshold_zero);
    run_test("par_properties",                   test_par_properties);
    run_test("par_get_name_contains_thread_count", test_par_get_name_contains_thread_count);

    run_test("equivalence_seq_vs_par", test_equivalence);

    std::cout << "\n==============================\n";
    std::cout << "Passed: " << g_passed << "\n";
    std::cout << "Failed: " << g_failed << "\n";
    std::cout << "==============================\n";
    return g_failed == 0 ? 0 : 1;
}
