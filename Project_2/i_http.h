#pragma once
/**
 * @file i_http.h
 * @brief Абстрактний інтерфейс HTTP клієнта.
 *
 * Базовий інтерфейс для патерну Decorator.
 * BasicHttp реалізує реальні HTTP запити,
 * CachedHttp декорує будь-яку реалізацію IHttp.
 *
 * @par Патерн: Decorator (базовий інтерфейс)
 * @see CachedHttp, httpGet
 */

#include <string>
#include <optional>

/**
 * @brief Інтерфейс HTTP клієнта.
 */
class IHttp {
public:
    virtual ~IHttp() = default;

    /**
     * @brief Виконує HTTP GET запит.
     * @param url     Повний URL
     * @param timeout Таймаут у секундах
     * @return        Тіло відповіді або nullopt при помилці
     */
    virtual std::optional<std::string> get(const std::string& url,
                                            long timeout = 15) = 0;
};
