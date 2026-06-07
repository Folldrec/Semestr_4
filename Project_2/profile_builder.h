#pragma once
/**
 * @file profile_builder.h
 * @brief Патерн Builder для покрокової побудови UserProfile.
 *
 * Замість послідовного виклику кількох функцій steam_api,
 * ProfileBuilder дозволяє налаштувати які дані завантажувати
 * і отримати готовий UserProfile одним викликом build().
 *
 * @par Патерн: Builder (Створення)
 *
 * @par Приклад використання:
 * @code
 * // Повний профіль
 * auto profile = ProfileBuilder()
 *     .steamId("gaben")
 *     .withGenres(50)
 *     .withAchievements(30)
 *     .build();
 *
 * // Швидкий профіль без досягнень
 * auto quick = ProfileBuilder()
 *     .steamId(id)
 *     .withGenres(15)
 *     .build();
 * @endcode
 *
 * @see SteamProvider, steam_api.h
 */

#include "types.h"
#include <string>
#include <optional>

/**
 * @brief Будівник профілю Steam гравця.
 *
 * Реалізує патерн Builder — дозволяє покроково налаштувати
 * які дані завантажувати та отримати UserProfile через build().
 */
class ProfileBuilder {
public:
    ProfileBuilder() = default;

    /**
     * @brief Встановлює SteamID або Custom URL гравця.
     * @param id SteamID64 або vanity name (наприклад "gaben")
     * @return Посилання на this для fluent API
     */
    ProfileBuilder& steamId(const std::string& id);

    /**
     * @brief Налаштовує завантаження жанрів.
     * @param limit Кількість ігор для яких завантажувати жанри (0 = вимкнено)
     * @return Посилання на this для fluent API
     */
    ProfileBuilder& withGenres(int limit = 50);

    /**
     * @brief Налаштовує завантаження досягнень.
     * @param limit Кількість ігор для яких завантажувати досягнення (0 = вимкнено)
     * @return Посилання на this для fluent API
     */
    ProfileBuilder& withAchievements(int limit = 30);

    /**
     * @brief Виконує всі налаштовані кроки і повертає UserProfile.
     *
     * Кроки виконуються в порядку:
     * 1. resolveSteamID
     * 2. fetchPlayerSummary
     * 3. fetchOwnedGames
     * 4a. fetchGenres (якщо withGenres > 0)
     * 4b. fetchAchievements (якщо withAchievements > 0)
     *
     * @return Заповнений профіль або std::nullopt при помилці
     */
    std::optional<UserProfile> build() const;

private:
    std::string steamId_;
    int         genreLimit_       = 0;
    int         achievementLimit_ = 0;
};
