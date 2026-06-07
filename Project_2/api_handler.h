#pragma once
/**
 * @file api_handler.h
 * @brief Патерн Facade — реєстрація REST API маршрутів.
 *
 * Єдина точка входу для веб-інтерфейсу.
 * Координує ProfileBuilder, ISortStrategy та steam_api
 * приховуючи внутрішню складність від браузера.
 *
 * @par Патерн: Facade (Структурний)
 *
 * @par Маршрути:
 * - GET /api/status
 * - GET /api/steam?steamid=...&achievements=0|1&sort=hours|name|achievements|recent
 * - GET /api/compare?steamids=id1,id2,...
 *
 * @see HttpServer, ProfileBuilder, ISortStrategy
 */

#include "server.h"

/**
 * @brief Реєструє всі /api/* маршрути на сервері.
 * @param server HTTP сервер для реєстрації маршрутів
 */
void registerApiRoutes(HttpServer& server);
