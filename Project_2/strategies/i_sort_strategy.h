#pragma once
/**
 * @file strategies/i_sort_strategy.h
 * @brief Патерн Strategy — інтерфейс стратегії сортування ігор.
 *
 * Дозволяє вибирати алгоритм сортування в runtime
 * через URL параметр sort= без зміни коду клієнта.
 *
 * @par Патерн: Strategy (Поведінковий)
 *
 * @par Доступні стратегії:
 * - SortByHours        — за кількістю годин
 * - SortByName         — за назвою
 * - SortByAchievements — за % досягнень
 * - SortByRecentPlay   — за нещодавньою активністю
 *
 * @see sort_strategies.h, api_handler.cpp
 */

#include <vector>
#include <string>
#include "../types.h"

/**
 * @brief Абстрактна стратегія сортування ігор.
 *
 * @par Патерн: Strategy
 */
class ISortStrategy {
public:
    virtual ~ISortStrategy() = default;

    /**
     * @brief Сортує список ігор на місці.
     * @param[in,out] games Список ігор для сортування
     */
    virtual void sort(std::vector<Game>& games) = 0;

    /// @brief Назва стратегії для відображення в UI
    virtual std::string name() const = 0;
};
