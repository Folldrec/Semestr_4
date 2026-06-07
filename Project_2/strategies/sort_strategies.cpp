#include "sort_strategies.h"
#include <algorithm>

void SortByHours::sort(std::vector<Game>& games) {
    std::sort(games.begin(), games.end(),
        [](const Game& a, const Game& b) {
            return a.playtime_forever > b.playtime_forever;
        });
}

void SortByName::sort(std::vector<Game>& games) {
    std::sort(games.begin(), games.end(),
        [](const Game& a, const Game& b) {
            return a.name < b.name;
        });
}

void SortByAchievements::sort(std::vector<Game>& games) {
    std::sort(games.begin(), games.end(),
        [](const Game& a, const Game& b) {
            if (a.total_achievements() == 0) return false;
            if (b.total_achievements() == 0) return true;
            return a.achievement_pct() > b.achievement_pct();
        });
}

void SortByRecentPlay::sort(std::vector<Game>& games) {
    std::sort(games.begin(), games.end(),
        [](const Game& a, const Game& b) {
            return a.playtime_2weeks > b.playtime_2weeks;
        });
}
