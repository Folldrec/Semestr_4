#include "steam_provider.h"
#include "../steam_api.h"
#include "../config.h"
#include "../types.h"
#include <iostream>

bool SteamProvider::isConfigured() const {
    return !API_KEY.empty() &&
           API_KEY != "YOUR_STEAM_API_KEY_HERE";
}

ProviderProfile SteamProvider::convertProfile(const UserProfile& user) {
    ProviderProfile profile;
    profile.provider_name = "Steam";
    profile.player_id     = user.steamid;
    profile.display_name  = user.personaname;
    profile.avatar_url    = user.avatar;

    for (const auto& g : user.games) {
        if (g.excluded) continue;

        ProviderGame pg;
        pg.provider_id     = std::to_string(g.appid);
        pg.name            = g.name;
        pg.hours_played    = g.hours();
        pg.achievement_pct = g.achievement_pct();
        pg.genres          = g.genres;

        // Додаткові дані у extra
        pg.extra["playtime_2weeks"] =
            std::to_string(g.playtime_2weeks / 60) + "h";
        pg.extra["ach_done"]  = std::to_string(g.achieved_count());
        pg.extra["ach_total"] = std::to_string(g.total_achievements());

        profile.games.push_back(std::move(pg));
    }

    return profile;
}

std::optional<ProviderProfile> SteamProvider::fetchProfile(
    const std::string& playerId,
    const std::string&) const
{
    std::cout << "  [Steam] Завантаження: " << playerId << "\n";

    UserProfile user;

    user.steamid = resolveSteamID(playerId);
    if (user.steamid.empty()) {
        std::cerr << "  [Steam] SteamID не знайдено: " << playerId << "\n";
        return std::nullopt;
    }

    if (!fetchPlayerSummary(user)) {
        std::cerr << "  [Steam] Профіль недоступний.\n";
        return std::nullopt;
    }
    std::cout << "  [Steam] Знайдено: " << user.personaname << "\n";

    if (!fetchOwnedGames(user)) {
        std::cerr << "  [Steam] Бібліотека закрита.\n";
        return std::nullopt;
    }
    std::cout << "  [Steam] Ігор: " << user.games.size() << "\n";

    int gf = 0;
    for (auto& g : user.games) {
        if (gf++ >= MAX_GENRE_FETCH) break;
        fetchGenres(g);
    }

    int af = 0;
    for (auto& g : user.games) {
        if (af++ >= MAX_ACHIEVEMENT_FETCH) break;
        if (g.playtime_forever == 0) continue;
        fetchAchievements(g, user.steamid);
    }

    return convertProfile(user);
}
