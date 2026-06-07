#pragma once
#include <string>
#include <vector>
#include "types.h"

namespace Color {
    inline const std::string RESET  = "\033[0m";
    inline const std::string BOLD   = "\033[1m";
    inline const std::string CYAN   = "\033[36m";
    inline const std::string GREEN  = "\033[32m";
    inline const std::string YELLOW = "\033[33m";
    inline const std::string RED    = "\033[31m";
    inline const std::string BLUE   = "\033[34m";
    inline const std::string MAG    = "\033[35m";
}

/**
 * Виводить кольоровий заголовок секції з роздільниками.
 */
void printHeader(const std::string& title);

/**
 * Малює один горизонтальний рядок bar-chart'у.
 * @param label  Назва категорії (вирівнюється зліва).
 * @param value  Поточне значення.
 * @param maxVal Максимальне значення (100% ширина).
 * @param width  Ширина бару у символах (за замовчуванням 30).
 */
void printBar(const std::string& label,
              double value,
              double maxVal,
              int width = 30);

/**
 * Повний звіт одного гравця:
 *  – загальний час
 *  – bar-chart по категоріях
 *  – таблиця топ-20 ігор
 *  – інтерактивне виключення гри
 */
void reportUser(UserProfile& user);

/**
 * Порівняння двох або більше гравців:
 *  – % досягнень (хто кращий)
 *  – час у кожній категорії
 *  – загальний час
 */
void reportComparison(std::vector<UserProfile>& users);
