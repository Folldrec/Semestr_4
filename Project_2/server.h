#pragma once
/**
 * @file server.h
 * @brief Простий кросплатформний HTTP/1.1 сервер.
 *
 * Реалізований на сирих сокетах (Winsock2 на Windows, POSIX на Linux/macOS).
 * Підтримує реєстрацію маршрутів та обробку запитів у окремих потоках.
 *
 * @par Використання:
 * @code
 * HttpServer server(8080);
 * server.route("GET", "/api/hello", [](const HttpRequest& req) {
 *     return makeJsonResponse("{"ok":true}");
 * });
 * server.run(); // блокуючий виклик
 * @endcode
 */

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <unistd.h>
#endif

#include <string>
#include <functional>
#include <map>

/**
 * @brief HTTP запит від клієнта (браузера).
 */
struct HttpRequest {
    std::string method; ///< HTTP метод: "GET", "POST"
    std::string path;   ///< Шлях запиту: "/api/steam"
    std::string body;   ///< Тіло запиту (для POST)
    std::map<std::string, std::string> params; ///< Query параметри: ?key=value
};

/**
 * @brief HTTP відповідь сервера.
 */
struct HttpResponse {
    int         status       = 200;              ///< HTTP статус код
    std::string content_type = "application/json"; ///< Content-Type заголовок
    std::string body;                            ///< Тіло відповіді
};

/// @brief Створює JSON відповідь із заданим кодом
HttpResponse makeJsonResponse(const std::string& data, int code = 200);

/// @brief Створює HTML відповідь (для index.html)
HttpResponse makeHtmlResponse(const std::string& data);

/// @brief Створює JSON відповідь з повідомленням про помилку
HttpResponse makeErrorResponse(const std::string& msg, int code = 500);

/// @brief Тип обробника маршруту
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

/**
 * @brief HTTP/1.1 сервер на сирих сокетах.
 *
 * Кожен клієнт обробляється у окремому std::thread.
 * Реєстрація маршрутів через метод route().
 *
 * @note Частина патерну Facade — приховує деталі сокетів від api_handler.
 */
class HttpServer {
public:
    /**
     * @brief Конструктор.
     * @param port Порт прослуховування (за замовчуванням 8080)
     */
    explicit HttpServer(int port = 8080);
    ~HttpServer();

    /**
     * @brief Реєструє обробник для HTTP маршруту.
     * @param method  HTTP метод ("GET", "POST")
     * @param path    Шлях маршруту ("/api/steam")
     * @param handler Функція-обробник
     */
    void route(const std::string& method,
               const std::string& path,
               RouteHandler handler);

    /// @brief Запускає сервер (блокуючий виклик до stop())
    void run();

    /// @brief Зупиняє сервер
    void stop();

private:
    int  port_;
    bool running_ = false;
    std::map<std::string, RouteHandler> routes_;

    void        handleClient(int sock);
    HttpRequest parseRequest(const std::string& raw);
    std::string buildResponse(const HttpResponse& resp);
    RouteHandler findHandler(const std::string& method,
                              const std::string& path);

    static std::map<std::string, std::string> parseQuery(const std::string& qs);
    static std::string urlDecode(const std::string& s);
};
