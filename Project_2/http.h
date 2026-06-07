#pragma once
/**
 * @file http.h
 * @brief Легковісний HTTP GET клієнт на основі libcurl.
 *
 * Частина патерну Decorator — BasicHttp реалізує IHttp,
 * CachedHttp обгортає його додаючи кешування.
 *
 * @see IHttp, CachedHttp
 */

#include <string>
#include <optional>

/**
 * @brief Виконує HTTP GET запит до вказаного URL.
 *
 * @param url     Повний URL з параметрами запиту
 * @param timeout Таймаут у секундах (за замовчуванням 15)
 * @return        Тіло відповіді або std::nullopt при помилці мережі
 */
std::optional<std::string> httpGet(const std::string& url, long timeout = 15);
