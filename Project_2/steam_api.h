#pragma once
/**
 * @file steam_api.h
 * @brief Функції для роботи зі Steam Web API та Steam Store API.
 *
 * Містить низькорівневі функції завантаження даних.
 * Для зручнішого використання рекомендується ProfileBuilder.
 *
 * @see ProfileBuilder, SteamProvider
 */

#include <string>
#include <vector>
#include "types.h"

/**
 * @brief Перетворює Vanity URL або SteamID64 рядок на SteamID64.
 *
 * Якщо переданий рядок вже є 17-значним числом — повертає без API запиту.
 *
 * @param input Vanity URL (наприклад "gaben") або SteamID64
 * @return SteamID64 рядок або порожній рядок при помилці
 */
std::string resolveSteamID(const std::string& input);

/**
 * @brief Завантажує ім'я та URL аватара гравця.
 *
 * @param[out] user Заповнює user.personaname та user.avatar
 * @return true при успіху
 */
bool fetchPlayerSummary(UserProfile& user);

/**
 * @brief Завантажує список усіх ігор гравця з часом гри.
 *
 * Заповнює user.games та сортує за playtime_forever DESC.
 *
 * @param[out] user Профіль з встановленим steamid
 * @return true при успіху, false якщо бібліотека закрита
 */
bool fetchOwnedGames(UserProfile& user);

/**
 * @brief Завантажує жанри для однієї гри через Steam Store API.
 *
 * @param[out] game Гра — заповнює game.genres
 */
void fetchGenres(Game& game);

/**
 * @brief Завантажує список досягнень гравця для однієї гри.
 *
 * @param[out] game    Гра — заповнює game.achievements
 * @param      steamid SteamID64 гравця
 */
void fetchAchievements(Game& game, const std::string& steamid);

/**
 * @brief Зберігає дані профілів у JSON файл.
 *
 * Генерує steam_report.json для використання в HTML дашборді.
 *
 * @param users    Список профілів для збереження
 * @param filename Шлях до файлу (за замовчуванням "steam_report.json")
 * @return true при успішному записі
 */
bool exportJson(const std::vector<UserProfile>& users,
                const std::string& filename = "steam_report.json");
