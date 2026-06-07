#include "api_handler.h"
#include "config.h"
#include "types.h"
#include "steam_api.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "profile_builder.h"
#include "strategies/sort_strategies.h"
#include "commands/game_commands.h"

using json = nlohmann::json;


static json gameToJson(const Game& g) {
    json j;
    j["appid"]     = g.appid;
    j["name"]      = g.name;
    j["hours"]     = g.hours();
    j["ach_pct"]   = g.achievement_pct();
    j["ach_done"]  = g.achieved_count();
    j["ach_total"] = g.total_achievements();
    j["genres"]    = g.genres;
    j["excluded"]  = g.excluded;
    return j;
}

static json userToJson(const UserProfile& u) {
    json games = json::array();
    for (const auto& g : u.games)
        games.push_back(gameToJson(g));

    json j;
    j["steamid"]     = u.steamid;
    j["name"]        = u.personaname;
    j["avatar"]      = u.avatar;
    j["total_hours"] = u.total_hours();
    j["ach_pct"]     = u.overall_achievement_pct();
    j["categories"]  = u.hours_by_category();
    j["games"]       = games;
    return j;
}


static HttpResponse handleStatus(const HttpRequest&) {
    json resp;
    resp["steam_configured"] = (API_KEY != "YOUR_STEAM_API_KEY_HERE");
    return makeJsonResponse(resp.dump());
}

static HttpResponse handleSteam(const HttpRequest& req) {
    auto it = req.params.find("steamid");
    if (it == req.params.end() || it->second.empty())
        return makeErrorResponse("steamid param required", 400);

    bool wantAch = req.params.count("achievements") &&
                   req.params.at("achievements") == "1";

    std::cout << "[API] Steam: " << it->second << "\n";

    ProfileBuilder builder;
    builder.steamId(it->second)
           .withGenres(MAX_GENRE_FETCH);

    if (wantAch)
        builder.withAchievements(MAX_ACHIEVEMENT_FETCH);

    auto userOpt = builder.build();
    if (!userOpt)
        return makeErrorResponse("Profile unavailable or private");

    UserProfile user = std::move(*userOpt);

    std::string sortBy = req.params.count("sort")
                       ? req.params.at("sort") : "hours";

    std::unique_ptr<ISortStrategy> strategy;
    if      (sortBy == "name")         strategy = std::make_unique<SortByName>();
    else if (sortBy == "achievements") strategy = std::make_unique<SortByAchievements>();
    else if (sortBy == "recent")       strategy = std::make_unique<SortByRecentPlay>();
    else                               strategy = std::make_unique<SortByHours>();

    strategy->sort(user.games);

    return makeJsonResponse(userToJson(user).dump());
}

static HttpResponse handleCompare(const HttpRequest& req) {
    auto it = req.params.find("steamids");
    if (it == req.params.end())
        return makeErrorResponse("steamids param required", 400);

    std::vector<std::string> ids;
    std::istringstream ss(it->second);
    std::string token;
    while (std::getline(ss, token, ','))
        if (!token.empty()) ids.push_back(token);

    if (ids.size() < 2)
        return makeErrorResponse("Need at least 2 steamids");

    json result = json::array();
    for (const auto& id : ids) {
        std::cout << "[Compare] Loading: " << id << "\n";

        UserProfile user;
        user.steamid = resolveSteamID(id);
        if (user.steamid.empty()) {
            std::cerr << "[Compare] SteamID not found: " << id << "\n";
            continue;
        }
        if (!fetchPlayerSummary(user)) {
            std::cerr << "[Compare] Profile unavailable: " << id << "\n";
            continue;
        }
        if (!fetchOwnedGames(user)) {
            std::cerr << "[Compare] Library unavailable: " << id << "\n";
            continue;
        }

        std::cout << "[Compare] Loaded " << user.personaname
                  << " - " << user.games.size() << " games\n";

        int gf = 0;
        for (auto& g : user.games) {
            if (gf++ >= 15) break;
            fetchGenres(g);
        }

        int af = 0;
        for (auto& g : user.games) {
            if (af++ >= 10) break;
            if (g.playtime_forever == 0) continue;
            fetchAchievements(g, user.steamid);
        }

        result.push_back(userToJson(user));
        std::cout << "[Compare] Done: " << user.personaname << "\n";
    }

    if (result.empty())
        return makeErrorResponse("No profiles loaded. Check SteamIDs and privacy settings.");

    return makeJsonResponse(result.dump());
}


void registerApiRoutes(HttpServer& server) {
    server.route("GET", "/api/status",  handleStatus);
    server.route("GET", "/api/steam",   handleSteam);
    server.route("GET", "/api/compare", handleCompare);
}
