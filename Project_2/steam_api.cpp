
#include "steam_api.h"
#include "http.h"
#include "config.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string resolveSteamID(const std::string& input) {
    if (input.size() == 17 &&
        std::all_of(input.begin(), input.end(), ::isdigit))
        return input;

    const std::string url =
        "https://api.steampowered.com/ISteamUser/ResolveVanityURL/v1/"
        "?key=" + API_KEY + "&vanityurl=" + input;

    auto resp = httpGet(url);
    if (!resp) return {};

    try {
        auto j = json::parse(*resp);
        if (j["response"]["success"] == 1)
            return j["response"]["steamid"].get<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "[resolveSteamID] JSON parse error: " << e.what() << "\n";
    }
    return {};
}

bool fetchPlayerSummary(UserProfile& user) {
    const std::string url =
        "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/"
        "?key=" + API_KEY + "&steamids=" + user.steamid;

    auto resp = httpGet(url);
    if (!resp) return false;

    try {
        auto j = json::parse(*resp);
        const auto& players = j["response"]["players"];
        if (players.empty()) return false;

        user.personaname = players[0].value("personaname", "Unknown");
        user.avatar      = players[0].value("avatarfull",  "");
    } catch (const std::exception& e) {
        std::cerr << "[fetchPlayerSummary] " << e.what() << "\n";
        return false;
    }
    return true;
}

bool fetchOwnedGames(UserProfile& user) {
    const std::string url =
        "https://api.steampowered.com/IPlayerService/GetOwnedGames/v1/"
        "?key=" + API_KEY
        + "&steamid=" + user.steamid
        + "&include_appinfo=true"
        + "&include_played_free_games=true";

    auto resp = httpGet(url);
    if (!resp) return false;

    try {
        auto j = json::parse(*resp);
        for (const auto& g : j["response"]["games"]) {
            Game game;
            game.appid            = g.value("appid",            0ULL);
            game.name             = g.value("name",             "Unknown");
            game.playtime_forever = g.value("playtime_forever", 0);
            game.playtime_2weeks  = g.value("playtime_2weeks",  0);
            user.games.push_back(std::move(game));
        }
    } catch (const std::exception& e) {
        std::cerr << "[fetchOwnedGames] " << e.what() << "\n";
        return false;
    }

    std::sort(user.games.begin(), user.games.end(),
        [](const Game& a, const Game& b){
            return a.playtime_forever > b.playtime_forever;
        });
    return true;
}


void fetchGenres(Game& game) {
    const std::string url =
        "https://store.steampowered.com/api/appdetails"
        "?appids=" + std::to_string(game.appid) + "&filters=genres";

    auto resp = httpGet(url);
    if (!resp) return;

    try {
        auto j = json::parse(*resp);
        const std::string key = std::to_string(game.appid);

        if (j.contains(key) && j[key]["success"] == true) {
            for (const auto& g : j[key]["data"]["genres"])
                game.genres.push_back(g.value("description", ""));
        }
    } catch (const std::exception& e) {
        std::cerr << "[fetchGenres:" << game.appid << "] " << e.what() << "\n";
    }
}

void fetchAchievements(Game& game, const std::string& steamid) {
    const std::string url =
        "https://api.steampowered.com/ISteamUserStats/GetPlayerAchievements/v1/"
        "?key=" + API_KEY
        + "&steamid=" + steamid
        + "&appid="   + std::to_string(game.appid);

    auto resp = httpGet(url);
    if (!resp) return;

    try {
        auto j = json::parse(*resp);
        if (j["playerstats"].value("success", false) == false) return;

        for (const auto& a : j["playerstats"]["achievements"]) {
            Achievement ach;
            ach.apiname    = a.value("apiname",    "");
            ach.achieved   = (a.value("achieved",  0) == 1);
            ach.unlocktime = a.value("unlocktime", 0);
            game.achievements.push_back(std::move(ach));
        }
    } catch (const std::exception& e) {
        std::cerr << "[fetchAchievements:" << game.appid << "] " << e.what() << "\n";
    }
}

bool exportJson(const std::vector<UserProfile>& users, const std::string& filename) {
    json snapshot = json::array();

    for (const auto& u : users) {
        json ju;
        ju["steamid"]         = u.steamid;
        ju["name"]            = u.personaname;
        ju["avatar"]          = u.avatar;
        ju["total_hours"]     = u.total_hours();
        ju["achievement_pct"] = u.overall_achievement_pct();
        ju["categories"]      = u.hours_by_category();
        ju["games"]           = json::array();

        for (const auto& g : u.games) {
            if (g.excluded) continue;
            ju["games"].push_back({
                {"appid",    g.appid},
                {"name",     g.name},
                {"hours",    g.hours()},
                {"ach_pct",  g.achievement_pct()},
                {"genres",   g.genres},
            });
        }
        snapshot.push_back(ju);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[exportJson] Cannot open file: " << filename << "\n";
        return false;
    }
    file << snapshot.dump(2);
    return true;
}
