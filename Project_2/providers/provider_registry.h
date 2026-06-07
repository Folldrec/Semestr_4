#pragma once
#include <memory>
#include <vector>
#include <string>
#include "i_provider.h"

class ProviderRegistry {
public:
    static ProviderRegistry& instance();

    /// Додає провайдер до реєстру
    void add(std::unique_ptr<IProvider> provider);

    /// Знайти провайдер за назвою (nullptr якщо не знайдено)
    IProvider* get(const std::string& name) const;

    /// Усі зареєстровані провайдери
    const std::vector<std::unique_ptr<IProvider>>& all() const;

    /// Тільки провайдери, де isConfigured() == true
    std::vector<IProvider*> configured() const;

    /// Список назв усіх провайдерів (для меню)
    std::vector<std::string> names() const;

private:
    ProviderRegistry() = default;
    std::vector<std::unique_ptr<IProvider>> providers_;
};
