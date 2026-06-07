#include "profile_builder.h"
#include "steam_api.h"
#include <iostream>


ProfileBuilder& ProfileBuilder::steamId(const std::string& id) {
    steamId_ = id;
    return *this;
}

ProfileBuilder& ProfileBuilder::withGenres(int limit) {
    genreLimit_ = limit;
    return *this;
}

ProfileBuilder& ProfileBuilder::withAchievements(int limit) {
    achievementLimit_ = limit;
    return *this;
}


std::optional<UserProfile> ProfileBuilder::build() const {
    if (steamId_.empty()) {
        std::cerr << "[Builder] SteamID не вказано\n";
        return std::nullopt;
    }

    UserProfile user;

    std::cout << "[Builder] Крок 1/4: Resolve SteamID...\n";
    user.steamid = resolveSteamID(steamId_);
    if (user.steamid.empty()) {
        std::cerr << "[Builder] SteamID не знайдено: " << steamId_ << "\n";
        return std::nullopt;
    }

    std::cout << "[Builder] Крок 2/4: Завантаження профілю...\n";
    if (!fetchPlayerSummary(user)) {
        std::cerr << "[Builder] Профіль недоступний\n";
        return std::nullopt;
    }
    std::cout << "[Builder] Знайдено: " << user.personaname << "\n";

    std::cout << "[Builder] Крок 3/4: Завантаження бібліотеки...\n";
    if (!fetchOwnedGames(user)) {
        std::cerr << "[Builder] Бібліотека недоступна\n";
        return std::nullopt;
    }
    std::cout << "[Builder] Ігор: " << user.games.size() << "\n";

    if (genreLimit_ > 0) {
        std::cout << "[Builder] Крок 4/4: Жанри (топ "
                  << genreLimit_ << ")...\n";
        int count = 0;
        for (auto& g : user.games) {
            if (count++ >= genreLimit_) break;
            fetchGenres(g);
        }
    }

    if (achievementLimit_ > 0) {
        std::cout << "[Builder] Крок 4/4: Досягнення (топ "
                  << achievementLimit_ << ")...\n";
        int count = 0;
        for (auto& g : user.games) {
            if (count++ >= achievementLimit_) break;
            if (g.playtime_forever == 0) continue;
            fetchAchievements(g, user.steamid);
        }
    }

    std::cout << "[Builder] Профіль зібрано: "
              << user.personaname << "\n";
    return user;
}
