#pragma once
/**
 * @file providers/steam_provider.h
 * @brief Патерн Adapter — Steam реалізація IProvider.
 *
 * Адаптує функції steam_api.h до інтерфейсу IProvider.
 * Виконує роль і Adapter (адаптує steam_api) і
 * ConcreteProduct для Abstract Factory (IProvider).
 *
 * @par Патерни: Adapter + Abstract Factory (Конкретна фабрика)
 *
 * @see IProvider, steam_api.h, ProfileBuilder
 */

#include "i_provider.h"
#include <string>
#include <optional>

/**
 * @brief Steam реалізація IProvider.
 *
 * Делегує завантаження даних функціям steam_api.h
 * та конвертує UserProfile → ProviderProfile.
 */
class SteamProvider final : public IProvider {
public:
    std::string name() const override { return "Steam"; }

    /// @return true якщо API_KEY встановлено в config.h
    bool isConfigured() const override;

    /**
     * @brief Завантажує Steam профіль гравця.
     * @param playerId SteamID64 або Custom URL
     * @param region   Ігнорується (Steam глобальний)
     * @return         Профіль або nullopt при помилці/закритому профілі
     */
    std::optional<ProviderProfile> fetchProfile(
        const std::string& playerId,
        const std::string& region = "") const override;

    std::vector<std::string> supportedRegions() const override { return {}; }

private:
    /**
     * @brief Конвертує UserProfile у ProviderProfile.
     * @param user Заповнений UserProfile
     * @return     Відповідний ProviderProfile
     */
    static ProviderProfile convertProfile(const struct UserProfile& user);
};
