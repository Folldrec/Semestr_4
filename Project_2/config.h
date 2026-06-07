#pragma once
/**
 * @file config.h
 * @brief Глобальна конфігурація: API ключі, маппінг жанрів, ліміти.
 *
 * Єдине місце для налаштування застосунку.
 * Перед збіркою замініть API_KEY на реальний ключ.
 *
 * @see https://steamcommunity.com/dev/apikey
 */

#include <string>
#include <vector>
#include <utility>

/// @brief Steam Web API ключ.
inline const std::string API_KEY = "D83A8B40B5C979F0876040B67E256B13";

/**
 * @brief Маппінг ключових слів жанрів Steam → українські назви категорій.
 *
 * Порядок важливий: використовується перший збіг.
 * Нові жанри можна додати до кінця списку.
 */
inline const std::vector<std::pair<std::string,std::string>> GENRE_MAP = {
    {"Action",       "Шутери/Екшн"},
    {"Shooter",      "Шутери/Екшн"},
    {"FPS",          "Шутери/Екшн"},
    {"Strategy",     "Стратегії"},
    {"RTS",          "Стратегії"},
    {"RPG",          "РПГ"},
    {"Role-Playing", "РПГ"},
    {"Adventure",    "Пригоди"},
    {"Simulation",   "Симулятори"},
    {"Sports",       "Спорт"},
    {"Racing",       "Гонки"},
    {"Puzzle",       "Пазли"},
    {"Horror",       "Хорор"},
    {"Indie",        "Інді"},
    {"MMO",          "ММО"},
    {"Multiplayer",  "Мультиплеєр"},
};

/// @brief Максимальна кількість ігор для завантаження жанрів
inline constexpr int MAX_GENRE_FETCH       = 50;

/// @brief Максимальна кількість ігор для завантаження досягнень
inline constexpr int MAX_ACHIEVEMENT_FETCH = 30;

/// @brief Кількість ігор у термінальному звіті
inline constexpr int TOP_GAMES_DISPLAY     = 20;
