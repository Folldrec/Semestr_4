/**
 * @file Sorter.hpp
 * @brief Abstract base class (interface) for all sorter implementations.
 */

#pragma once
#include <vector>
#include <string>

/**
 * @brief Abstract interface for sorting algorithms.
 *
 * Defines the contract that both sequential and parallel
 * implementations must fulfil.  Encapsulates the sorting
 * strategy so callers are decoupled from the concrete algorithm.
 */
class Sorter {
public:
    virtual ~Sorter() = default;

    /**
     * @brief Sort a vector of integers.
     * @param data  Input vector (not modified).
     * @return      New sorted vector.
     */
    virtual std::vector<int> sort(const std::vector<int>& data) const = 0;

    /**
     * @brief Human-readable name of this sorter.
     * @return Name string.
     */
    virtual std::string getName() const = 0;
};
