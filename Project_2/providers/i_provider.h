#pragma once
/**
 * @file providers/i_provider.h
 * @brief Патерн Abstract Factory — інтерфейс ігрового провайдера.
 *
 * Єдиний інтерфейс для будь-якого ігрового сервісу.
 * Нові провайдери додаються успадкуванням від IProvider
 * без зміни існуючого коду.
 *
 * @par Патерн: Abstract Factory (Створення)
 *
 * @par Додавання нового провайдера:
 * 1. Успадкувати IProvider
 * 2. Реалізувати name(), isConfigured(), fetchProfile()
 * 3. Зареєструвати: ProviderRegistry::instance().add(make_unique<NewProvider>())
 *
 * @see SteamProvider, ProviderRegistry
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>

/**
 * @brief Уніфікована гра з будь-якого провайдера.
 */
struct ProviderGame {
    std::string  provider_id;         ///< ID гри у провайдері
    std::string  name;                ///< Назва гри
    double       hours_played    = 0; ///< Загальний ігровий час (години)
    double       achievement_pct = 0; ///< Відсоток досягнень (0 якщо немає)
    std::vector<std::string> genres;  ///< Список жанрів
    std::map<std::string,std::string> extra; ///< Додаткові метадані (rank, mastery тощо)
};

/**
 * @brief Уніфікований профіль гравця з будь-якого провайдера.
 */
struct ProviderProfile {
    std::string provider_name; ///< Назва провайдера ("Steam", "Riot" тощо)
    std::string player_id;     ///< ID гравця у провайдері
    std::string display_name;  ///< Нікнейм гравця
    std::string avatar_url;    ///< URL аватара

    std::vector<ProviderGame> games; ///< Список ігор/активностей

    double total_hours() const;              ///< Сумарний час
    std::map<std::string,double> hours_by_category() const; ///< Час по категоріях
    double overall_achievement_pct() const;  ///< Середній % досягнень
};

/**
 * @brief Абстрактний інтерфейс ігрового провайдера.
 *
 * @par Патерн: Abstract Factory
 */
class IProvider {
public:
    virtual ~IProvider() = default;

    /// @brief Назва провайдера для відображення
    virtual std::string name() const = 0;

    /**
     * @brief Перевіряє чи налаштований провайдер (є API ключ).
     * @return false якщо ключ не вказаний у config.h
     */
    virtual bool isConfigured() const = 0;

    /**
     * @brief Завантажує профіль та ігри гравця.
     * @param playerId ID або нікнейм гравця
     * @param region   Регіон (ігнорується якщо провайдер глобальний)
     * @return         Заповнений профіль або nullopt при помилці
     */
    virtual std::optional<ProviderProfile> fetchProfile(
        const std::string& playerId,
        const std::string& region = "") const = 0;

    /// @brief Список підтримуваних регіонів (порожній = регіон не потрібен)
    virtual std::vector<std::string> supportedRegions() const { return {}; }
};
