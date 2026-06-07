#include "provider_report.h"
#include "../report.h" 
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>

using namespace Color;

void reportProviderProfile(const ProviderProfile& profile) {
    printHeader("🎮 [" + profile.provider_name + "] " + profile.display_name);

    std::cout << BOLD << "\n  Загальний ігровий час: "
              << YELLOW << std::fixed << std::setprecision(1)
              << profile.total_hours() << " год." << RESET
              << "  |  Ігор: " << profile.games.size() << "\n\n";

    const auto cats = profile.hours_by_category();
    if (!cats.empty()) {
        printHeader("📊 Час за категоріями");
        double maxH = 0;
        for (const auto& [k,v] : cats) maxH = std::max(maxH, v);
        for (const auto& [k,v] : cats) printBar(k, v, maxH > 0 ? maxH : 1);
    }

    printHeader("🏆 Ігри / активності");
    std::cout << BOLD
              << std::left  << std::setw(40) << "Назва"
              << std::right << std::setw(10) << "Год."
              << std::right << std::setw(10) << "Ачів %"
              << "\n" << RESET
              << std::string(62, '─') << "\n";

    auto sorted = profile.games;
    std::sort(sorted.begin(), sorted.end(),
        [](const ProviderGame& a, const ProviderGame& b){
            return a.hours_played > b.hours_played; });

    int idx = 1;
    for (const auto& g : sorted) {
        if (idx > 25) break;
        const std::string n = g.name.size() > 37
                            ? g.name.substr(0,37) + "..." : g.name;
        std::cout << std::left  << std::setw(40) << n
                  << std::right << std::setw(9)
                                << std::fixed << std::setprecision(1)
                                << g.hours_played << "h"
                  << std::right << std::setw(9)
                                << std::fixed << std::setprecision(1)
                                << g.achievement_pct << "%";

        for (const auto& [k,v] : g.extra)
            std::cout << "  " << MAG << k << "=" << v << RESET;
        std::cout << "\n";
        idx++;
    }
}

void reportProviderComparison(const std::vector<ProviderProfile>& profiles) {
    printHeader("⚔  ПОРІВНЯННЯ ГРАВЦІВ (мульти-провайдер)");

    std::set<std::string> allCats;
    for (const auto& p : profiles)
        for (const auto& [k,v] : p.hours_by_category())
            allCats.insert(k);

    for (const auto& cat : allCats) {
        double maxH = 0;
        for (const auto& p : profiles) {
            const auto cats = p.hours_by_category();
            if (cats.count(cat)) maxH = std::max(maxH, cats.at(cat));
        }
        if (maxH == 0) continue;

        std::cout << BOLD << "\n  📂 " << cat << ":\n" << RESET;
        const ProviderProfile* winner = nullptr;
        double winH = -1;
        for (const auto& p : profiles) {
            const auto cats = p.hours_by_category();
            const double h = cats.count(cat) ? cats.at(cat) : 0.0;
            printBar("    [" + p.provider_name + "] " + p.display_name,
                     h, maxH);
            if (h > winH) { winH = h; winner = &p; }
        }
        if (winner && winH > 0)
            std::cout << GREEN << "  👑 Лідер: "
                      << winner->display_name
                      << " (" << winner->provider_name << ")"
                      << RESET << "\n";
    }

    std::cout << BOLD << "\n  ⏱ Загальний ігровий час:\n" << RESET;
    double maxTot = 0;
    for (const auto& p : profiles) maxTot = std::max(maxTot, p.total_hours());
    for (const auto& p : profiles)
        printBar("    [" + p.provider_name + "] " + p.display_name,
                 p.total_hours(), maxTot > 0 ? maxTot : 1.0);
}

void reportAllProviders(const std::vector<ProviderProfile>& allProfiles) {
    printHeader("📊 ЗВЕДЕННЯ ПО ВСІХ ПРОВАЙДЕРАХ");

    std::map<std::string, double> byProvider;
    double grand = 0;
    for (const auto& p : allProfiles) {
        byProvider[p.provider_name] += p.total_hours();
        grand += p.total_hours();
    }

    double maxH = 0;
    for (const auto& [k,v] : byProvider) maxH = std::max(maxH, v);

    for (const auto& [k,v] : byProvider) {
        printBar(k, v, maxH > 0 ? maxH : 1.0);
    }

    std::cout << BOLD << "\n  Сумарно по всіх платформах: "
              << YELLOW << std::fixed << std::setprecision(1)
              << grand << " год." << RESET << "\n";
}
