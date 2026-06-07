#include "server.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>

#ifdef _WIN32
  using SocketT = SOCKET;
  #define SOCK_INVALID  INVALID_SOCKET
  #define SOCK_ERR      SOCKET_ERROR
  #define closeSock(s)  closesocket(s)
  using socklen_t = int;
#else
  using SocketT = int;
  #define SOCK_INVALID  (-1)
  #define SOCK_ERR      (-1)
  #define closeSock(s)  close(s)
#endif


HttpResponse makeJsonResponse(const std::string& data, int code) {
    HttpResponse r;
    r.status       = code;
    r.content_type = "application/json";
    r.body         = data;
    return r;
}

HttpResponse makeHtmlResponse(const std::string& data) {
    HttpResponse r;
    r.status       = 200;
    r.content_type = "text/html; charset=utf-8";
    r.body         = data;
    return r;
}

HttpResponse makeErrorResponse(const std::string& msg, int code) {
    HttpResponse r;
    r.status       = code;
    r.content_type = "application/json";
    r.body         = "{\"error\":\"" + msg + "\"}";
    return r;
}


HttpServer::HttpServer(int port) : port_(port) {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

HttpServer::~HttpServer() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void HttpServer::route(const std::string& method,
                       const std::string& path,
                       RouteHandler handler)
{
    routes_[method + " " + path] = std::move(handler);
}

void HttpServer::stop() { running_ = false; }


void HttpServer::run() {
    SocketT srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv == SOCK_INVALID) {
        std::cerr << "[Server] socket() failed\n";
        return;
    }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(port_));

    if (bind(srv, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCK_ERR) {
        std::cerr << "[Server] bind() failed on port " << port_ << "\n";
        closeSock(srv);
        return;
    }
    listen(srv, 16);

    running_ = true;
    std::cout << "\n  Server: http://localhost:" << port_ << "\n\n";

    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t   clientLen = sizeof(clientAddr);
        SocketT client = accept(srv,
                                reinterpret_cast<sockaddr*>(&clientAddr),
                                &clientLen);
        if (client == SOCK_INVALID) continue;

        std::thread([this, client]() {
            handleClient(static_cast<int>(client));
        }).detach();
    }
    closeSock(srv);
}


void HttpServer::handleClient(int clientSock) {
    char buf[16384] = {};
    int received = recv(clientSock, buf, sizeof(buf) - 1, 0);

    std::string raw;
    if (received > 0) raw.assign(buf, static_cast<size_t>(received));
    if (raw.empty()) { closeSock(clientSock); return; }

    HttpRequest  req  = parseRequest(raw);
    HttpResponse resp;

    if (req.method == "GET" && req.path == "/")
        req.path = "/index.html";

    auto handler = findHandler(req.method, req.path);
    if (handler) {
        try { resp = handler(req); }
        catch (const std::exception& e) {
            resp = makeErrorResponse(e.what());
        }
    } else {
        resp = makeErrorResponse("Not Found", 404);
    }

    std::string response = buildResponse(resp);
    send(clientSock,
         response.c_str(),
         static_cast<int>(response.size()), 0);
    closeSock(clientSock);
}


HttpRequest HttpServer::parseRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream ss(raw);
    std::string line;
    std::getline(ss, line);

    std::istringstream ls(line);
    std::string fullPath;
    ls >> req.method >> fullPath;

    auto qpos = fullPath.find('?');
    if (qpos != std::string::npos) {
        req.params = parseQuery(fullPath.substr(qpos + 1));
        req.path   = fullPath.substr(0, qpos);
    } else {
        req.path = fullPath;
    }

    bool inBody = false;
    while (std::getline(ss, line)) {
        if (line == "\r" || line.empty()) { inBody = true; continue; }
        if (inBody) req.body += line + "\n";
    }
    return req;
}


std::string HttpServer::buildResponse(const HttpResponse& resp) {
    std::string statusText = "OK";
    if (resp.status == 404) statusText = "Not Found";
    if (resp.status == 500) statusText = "Internal Server Error";
    if (resp.status == 400) statusText = "Bad Request";

    std::ostringstream out;
    out << "HTTP/1.1 " << resp.status << " " << statusText << "\r\n"
        << "Content-Type: "   << resp.content_type << "\r\n"
        << "Content-Length: " << resp.body.size()  << "\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << resp.body;
    return out.str();
}


RouteHandler HttpServer::findHandler(const std::string& method,
                                      const std::string& path)
{
    auto key = method + " " + path;
    auto it = routes_.find(key);
    if (it != routes_.end()) return it->second;
    return nullptr;
}


std::map<std::string, std::string> HttpServer::parseQuery(const std::string& qs) {
    std::map<std::string, std::string> result;
    std::istringstream ss(qs);
    std::string token;
    while (std::getline(ss, token, '&')) {
        auto eq = token.find('=');
        if (eq != std::string::npos)
            result[urlDecode(token.substr(0, eq))] =
                urlDecode(token.substr(eq + 1));
    }
    return result;
}

std::string HttpServer::urlDecode(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '+') {
            out += ' ';
        } else if (s[i] == '%' && i + 2 < s.size()) {
            int val = 0;
            std::istringstream hex(s.substr(i + 1, 2));
            hex >> std::hex >> val;
            out += static_cast<char>(val);
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}
