#pragma once
/**
 * @file strategies/sort_strategies.h
 * @brief Конкретні реалізації ISortStrategy.
 *
 * @par Патерн: Strategy (Поведінковий)
 *
 * Доступні алгоритми сортування ігор:
 * - SortByHours        — за загальним часом гри (URL: sort=hours)
 * - SortByName         — за назвою A-Z (URL: sort=name)
 * - SortByAchievements — за % досягнень (URL: sort=achievements)
 * - SortByRecentPlay   — за часом за останні 2 тижні (URL: sort=recent)
 *
 * @see ISortStrategy, api_handler.cpp
 */

#include "i_sort_strategy.h"

/**
 * @brief Сортування за загальним часом гри (спадання).
 * Ігри з найбільшою кількістю годин — вгорі.
 */
class SortByHours final : public ISortStrategy {
public:
    void sort(std::vector<Game>& games) override;
    std::string name() const override { return "За часом"; }
};

/**
 * @brief Сортування за назвою гри (алфавіт A-Z).
 */
class SortByName final : public ISortStrategy {
public:
    void sort(std::vector<Game>& games) override;
    std::string name() const override { return "За назвою"; }
};

/**
 * @brief Сортування за відсотком досягнень (спадання).
 * Ігри без досягнень — в кінці списку.
 */
class SortByAchievements final : public ISortStrategy {
public:
    void sort(std::vector<Game>& games) override;
    std::string name() const override { return "За досягненнями"; }
};

/**
 * @brief Сортування за часом гри за останні 2 тижні (спадання).
 * Нещодавно зіграні ігри — вгорі.
 */
class SortByRecentPlay final : public ISortStrategy {
public:
    void sort(std::vector<Game>& games) override;
    std::string name() const override { return "Нещодавно зіграні"; }
};
