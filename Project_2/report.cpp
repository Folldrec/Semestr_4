#include "report.h"
#include "config.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace Color;

void printHeader(const std::string& title) {
    std::cout << "\n" << BOLD << CYAN
              << "══════════════════════════════════════════\n"
              << "  " << title << "\n"
              << "══════════════════════════════════════════\n"
              << RESET;
}

void printBar(const std::string& label, double value, double maxVal, int width) {
    const int filled = (maxVal > 0)
        ? static_cast<int>(width * value / maxVal)
        : 0;

    std::cout << std::left << std::setw(22) << label << " │";
    std::cout << GREEN;
    for (int i = 0; i < filled;  i++) std::cout << "█";
    std::cout << RESET;
    for (int i = filled; i < width; i++) std::cout << "░";
    std::cout << "│ "
              << YELLOW << std::fixed << std::setprecision(1) << value << "h"
              << RESET << "\n";
}

void reportUser(UserProfile& user) {
    printHeader("Профіль: " + user.personaname + "  [" + user.steamid + "]");

    std::cout << BOLD << "\n  Загальний ігровий час: "
              << YELLOW << std::fixed << std::setprecision(1)
              << user.total_hours() << " год."
              << RESET << "\n";
    std::cout << "  Ігор у бібліотеці: " << user.games.size() << "\n\n";

    printHeader("Час за категоріями");
    const auto cats = user.hours_by_category();

    double maxH = 0.0;
    for (const auto& [k, v] : cats) maxH = std::max(maxH, v);
    for (const auto& [k, v] : cats) printBar(k, v, maxH);

    printHeader("Топ ігри за часом");
    std::cout << BOLD
              << std::left  << std::setw(5)  << "#"
              << std::left  << std::setw(40) << "Назва гри"
              << std::right << std::setw(10) << "Год."
              << std::right << std::setw(8)  << "Ачів %"
              << "\n" << RESET;
    std::cout << std::string(65, '─') << "\n";

    int idx = 1;
    for (const auto& g : user.games) {
        if (g.excluded || idx > TOP_GAMES_DISPLAY) break;
        const std::string shortName =
            (g.name.size() > 37) ? g.name.substr(0, 37) + "..." : g.name;

        std::cout << std::left  << std::setw(5)  << idx
                  << std::left  << std::setw(40) << shortName
                  << std::right << std::setw(9)  << std::fixed
                                << std::setprecision(1) << g.hours() << "h"
                  << std::right << std::setw(7)  << std::fixed
                                << std::setprecision(1) << g.achievement_pct() << "%"
                  << "\n";
        idx++;
    }

    printHeader("🗑  Видалення гри (м'яке виключення)");
    std::cout << "Введіть AppID або частину назви гри для виключення\n"
              << "(або 0 щоб пропустити): ";

    std::string query;
    std::cin >> query;

    if (query != "0" && !query.empty()) {
        int removed = 0;
        for (auto& g : user.games) {
            if (g.excluded) continue;
            const bool byId   = (std::to_string(g.appid) == query);
            const bool byName = (g.name.find(query) != std::string::npos);
            if (byId || byName) {
                g.excluded = true;
                std::cout << GREEN << "  ✓ Виключено: " << g.name
                          << " (" << std::fixed << std::setprecision(1)
                          << g.hours() << "h)" << RESET << "\n";
                removed++;
            }
        }

        if (removed == 0) {
            std::cout << RED << "  Нічого не знайдено за запитом: "
                      << query << RESET << "\n";
        } else {
            std::cout << YELLOW << "\n  Оновлений загальний час: "
                      << std::fixed << std::setprecision(1)
                      << user.total_hours() << " год." << RESET << "\n";
        }
    }
}

void reportComparison(std::vector<UserProfile>& users) {
    printHeader("⚔  ПОРІВНЯННЯ ГРАВЦІВ");

    std::cout << BOLD << "\n  Відсоток пройдених досягнень:\n" << RESET;

    UserProfile* bestAch = nullptr;
    for (auto& u : users) {
        const double pct = u.overall_achievement_pct();
        std::cout << "    " << std::left << std::setw(20) << u.personaname
                  << YELLOW << std::fixed << std::setprecision(2)
                  << pct << "%" << RESET << "\n";
        if (!bestAch || pct > bestAch->overall_achievement_pct())
            bestAch = &u;
    }
    if (bestAch)
        std::cout << GREEN << "  Лідер досягнень: "
                  << bestAch->personaname << RESET << "\n";

    const std::vector<std::pair<std::string,std::string>> cmpCats = {
        {"Шутери/Екшн", "Шутери/Екшн"},
        {"Стратегії",   "Стратегії"},
        {"РПГ",         "РПГ"},
        {"Симулятори",  "Симулятори"},
        {"Пригоди",     "Пригоди"},
    };

    for (const auto& [catKey, label] : cmpCats) {
        bool hasAny = false;
        for (const auto& u : users) {
            const auto cats = u.hours_by_category();
            if (cats.count(catKey) && cats.at(catKey) > 0) { hasAny = true; break; }
        }
        if (!hasAny) continue;

        std::cout << BOLD << "\n  " << label << " — час по гравцях:\n" << RESET;

        double maxH = 0.0;
        for (const auto& u : users) {
            const auto cats = u.hours_by_category();
            const double h = cats.count(catKey) ? cats.at(catKey) : 0.0;
            maxH = std::max(maxH, h);
        }

        UserProfile* winner = nullptr;
        double winH = -1.0;
        for (auto& u : users) {
            const auto cats = u.hours_by_category();
            const double h = cats.count(catKey) ? cats.at(catKey) : 0.0;
            printBar("    " + u.personaname, h, maxH > 0 ? maxH : 1.0);
            if (h > winH) { winH = h; winner = &u; }
        }
        if (winner && winH > 0)
            std::cout << GREEN << " Більше в " << label << ": "
                      << winner->personaname << " ("
                      << std::fixed << std::setprecision(1) << winH << "h)"
                      << RESET << "\n";
    }

    std::cout << BOLD << "\n Загальний ігровий час:\n" << RESET;
    double maxTot = 0.0;
    for (const auto& u : users) maxTot = std::max(maxTot, u.total_hours());
    for (auto& u : users)
        printBar("    " + u.personaname, u.total_hours(), maxTot > 0 ? maxTot : 1.0);
}
