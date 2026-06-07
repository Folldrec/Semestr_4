#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include <shellapi.h>
  #pragma comment(lib, "shell32.lib")
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

#include <curl/curl.h>

#include "config.h"
#include "server.h"
#include "api_handler.h"
#include "providers/provider_registry.h"
#include "providers/steam_provider.h"

static std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void openBrowser(const std::string& url) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url.c_str(),
                  nullptr, nullptr, SW_SHOWNORMAL);
#elif __APPLE__
    system(("open " + url).c_str());
#else
    system(("xdg-open " + url + " &").c_str());
#endif
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    curl_global_init(CURL_GLOBAL_ALL);

    std::cout << "\n"
              << "  ====================================\n"
              << "    PROFILE ANALYZER \n"
              << "  ====================================\n\n";

    if (API_KEY == "YOUR_STEAM_API_KEY_HERE") {
        std::cerr << "  [!] Steam API key missing in config.h\n"
                  << "      Get one: https://steamcommunity.com/dev/apikey\n\n";
    } else {
        std::cout << "  [OK] Steam API configured\n";
    }

    int port = 8080;
    if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }

    std::string htmlContent = readFile("web/index.html");
    if (htmlContent.empty()) {
        std::cerr << "  [!] web/index.html not found!\n\n";
    }

    HttpServer server(port);

    server.route("GET", "/", [&htmlContent](const HttpRequest&) {
        return makeHtmlResponse(htmlContent);
    });
    server.route("GET", "/index.html", [&htmlContent](const HttpRequest&) {
        return makeHtmlResponse(htmlContent);
    });

    ProviderRegistry::instance().add(std::make_unique<SteamProvider>());

    registerApiRoutes(server);

    std::string url = "http://localhost:" + std::to_string(port);
    std::thread([url]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        openBrowser(url);
    }).detach();

    std::cout << "  Open: " << url << "\n"
              << "  Press Ctrl+C to stop.\n\n";

    server.run();

    curl_global_cleanup();
    return 0;
}
