#pragma once
/**
 * @file cached_http.h
 * @brief Патерн Decorator — кешуючий HTTP клієнт.
 *
 * Обгортає будь-яку реалізацію IHttp та додає кешування
 * відповідей в пам'яті. При повторному запиті того самого
 * URL повертає збережений результат без звернення до мережі.
 *
 * @par Патерн: Decorator (Структурний)
 * @par Переваги: прискорює завантаження жанрів при порівнянні гравців
 *
 * @par Приклад:
 * @code
 * auto http = std::make_shared<CachedHttp>(
 *     std::make_shared<BasicHttp>(), 500);
 * auto resp = http->get("https://store.steampowered.com/...");
 * std::cout << "Hits: " << http->hits(); // кількість cache hits
 * @endcode
 *
 * @see IHttp, i_http.h
 */

#include "i_http.h"
#include <map>
#include <mutex>
#include <memory>

/**
 * @brief Декоратор що додає кешування до IHttp.
 *
 * Thread-safe реалізація через std::mutex.
 * При переповненні кешу видаляє найстаріший запис.
 */
class CachedHttp final : public IHttp {
public:
    /**
     * @brief Конструктор.
     * @param inner   Реальний HTTP клієнт (декорується)
     * @param maxSize Максимальна кількість записів у кеші (за замовчуванням 500)
     */
    explicit CachedHttp(std::shared_ptr<IHttp> inner, size_t maxSize = 500);

    /**
     * @brief Виконує запит або повертає кешований результат.
     *
     * @param url     URL запиту
     * @param timeout Таймаут у секундах
     * @return        Тіло відповіді або nullopt при помилці
     */
    std::optional<std::string> get(const std::string& url,
                                    long timeout = 15) override;

    /// @brief Поточна кількість записів у кеші
    size_t cacheSize() const;

    /// @brief Очищає весь кеш
    void clearCache();

    /// @brief Кількість cache hits (запитів що повернули кешований результат)
    size_t hits()   const { return hits_;   }

    /// @brief Кількість cache misses (реальних мережевих запитів)
    size_t misses() const { return misses_; }

private:
    std::shared_ptr<IHttp>             inner_;    ///< Декорований HTTP клієнт
    std::map<std::string, std::string> cache_;    ///< Кеш: URL → тіло відповіді
    mutable std::mutex                 mutex_;    ///< Захист від race conditions
    size_t                             maxSize_;  ///< Максимальний розмір кешу
    size_t                             hits_   = 0;
    size_t                             misses_ = 0;
};
