#pragma once
/**
 * @file types.h
 * @brief Основні структури даних застосунку.
 *
 * Містить Achievement, Game та UserProfile — три ключові
 * доменні об'єкти з якими працює весь проект.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cstdint>

#include "config.h"


/**
 * @brief Одне досягнення гравця у грі.
 */
struct Achievement {
    std::string apiname;     ///< Внутрішня назва досягнення (Steam API)
    bool        achieved    = false; ///< true якщо досягнення розблоковано
    int         unlocktime  = 0;     ///< Unix timestamp розблокування (0 якщо не розблоковано)
};


/**
 * @brief Гра у бібліотеці Steam гравця.
 *
 * Зберігає час гри, жанри та досягнення.
 * Поле @c excluded дозволяє м'яко видалити гру
 * з підрахунку без втрати даних (патерн Command).
 */
struct Game {
    uint64_t    appid            = 0;     ///< Унікальний ідентифікатор гри у Steam
    std::string name;                     ///< Назва гри
    int         playtime_forever = 0;     ///< Загальний час гри у хвилинах
    int         playtime_2weeks  = 0;     ///< Час гри за останні 2 тижні (хвилини)
    bool        excluded         = false; ///< true якщо гра виключена з підрахунку (soft-delete)

    std::vector<std::string> genres;       ///< Список жанрів (з Steam Store API)
    std::vector<Achievement> achievements; ///< Список досягнень

    /**
     * @brief Повертає загальний час гри у годинах.
     * @return Години (дробове число)
     */
    double hours() const {
        return playtime_forever / 60.0;
    }

    /**
     * @brief Кількість розблокованих досягнень.
     */
    int achieved_count() const {
        return static_cast<int>(
            std::count_if(achievements.begin(), achievements.end(),
                [](const Achievement& a){ return a.achieved; }));
    }

    /**
     * @brief Загальна кількість досягнень у грі.
     */
    int total_achievements() const {
        return static_cast<int>(achievements.size());
    }

    /**
     * @brief Відсоток пройдених досягнень (0..100).
     * @return 0 якщо досягнень немає, інакше (achieved/total)*100
     */
    double achievement_pct() const {
        if (total_achievements() == 0) return 0.0;
        return 100.0 * achieved_count() / total_achievements();
    }
};


/**
 * @brief Профіль Steam гравця з повною бібліотекою ігор.
 *
 * Агрегує всі ігри гравця та надає методи для
 * підрахунку статистики.
 */
struct UserProfile {
    std::string steamid;      ///< SteamID64 гравця
    std::string personaname;  ///< Нікнейм гравця
    std::string avatar;       ///< URL повного аватара
    std::vector<Game> games;  ///< Список ігор (відсортований за часом)

    /**
     * @brief Сумарний ігровий час по всіх НЕвиключених іграх.
     * @return Години
     */
    double total_hours() const {
        double h = 0.0;
        for (const auto& g : games)
            if (!g.excluded) h += g.hours();
        return h;
    }

    /**
     * @brief Час гри по категоріях (жанрах).
     *
     * Кожен жанр гри зіставляється з GENRE_MAP.
     * Ігри без жанрів потрапляють у категорію "Інше".
     *
     * @return Словник { назва категорії → сумарні години }
     */
    std::map<std::string,double> hours_by_category() const {
        std::map<std::string,double> result;
        for (const auto& g : games) {
            if (g.excluded) continue;
            std::set<std::string> seen;
            for (const auto& genre : g.genres) {
                std::string label = "Інше";
                for (const auto& [kw, lbl] : GENRE_MAP)
                    if (genre.find(kw) != std::string::npos) { label = lbl; break; }
                if (!seen.count(label)) {
                    result[label] += g.hours();
                    seen.insert(label);
                }
            }
            if (g.genres.empty()) result["Інше"] += g.hours();
        }
        return result;
    }

    /**
     * @brief Загальний відсоток досягнень по всій бібліотеці.
     *
     * Вагується по кількості досягнень у кожній грі.
     * @return Відсоток (0..100), 0 якщо досягнень немає
     */
    double overall_achievement_pct() const {
        int total = 0, done = 0;
        for (const auto& g : games) {
            if (g.excluded) continue;
            total += g.total_achievements();
            done  += g.achieved_count();
        }
        if (total == 0) return 0.0;
        return 100.0 * done / total;
    }
};
