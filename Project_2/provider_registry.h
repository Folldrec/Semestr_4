#pragma once
/**
 * @file providers/provider_registry.h
 * @brief Патерн Singleton — реєстр ігрових провайдерів.
 *
 * Зберігає всі зареєстровані IProvider та надає доступ до них.
 * Гарантує існування єдиного реєстру в програмі.
 *
 * @par Патерн: Singleton (Створення)
 *
 * @par Використання:
 * @code
 * // Реєстрація
 * ProviderRegistry::instance().add(std::make_unique<SteamProvider>());
 *
 * // Отримання всіх сконфігурованих
 * for (auto* p : ProviderRegistry::instance().configured()) {
 *     auto profile = p->fetchProfile(id);
 * }
 * @endcode
 *
 * @see IProvider, SteamProvider
 */

#include <memory>
#include <vector>
#include <string>
#include "i_provider.h"

/**
 * @brief Singleton реєстр провайдерів.
 *
 * @par Патерн: Singleton
 */
class ProviderRegistry {
public:
    /// @brief Повертає єдиний екземпляр реєстру
    static ProviderRegistry& instance();

    /**
     * @brief Додає провайдер до реєстру.
     * @param provider Унікальний вказівник на провайдер
     */
    void add(std::unique_ptr<IProvider> provider);

    /**
     * @brief Знаходить провайдер за назвою.
     * @param name Назва провайдера (наприклад "Steam")
     * @return Вказівник або nullptr якщо не знайдено
     */
    IProvider* get(const std::string& name) const;

    /// @brief Всі зареєстровані провайдери
    const std::vector<std::unique_ptr<IProvider>>& all() const;

    /// @brief Тільки провайдери з isConfigured() == true
    std::vector<IProvider*> configured() const;

    /// @brief Список назв всіх провайдерів
    std::vector<std::string> names() const;

private:
    ProviderRegistry() = default; ///< Приватний конструктор (Singleton)
    std::vector<std::unique_ptr<IProvider>> providers_;
};
