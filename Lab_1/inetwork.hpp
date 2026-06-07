#pragma once
#include <string>
#include <functional>

/**
 * @brief Abstract interface for network communication.
 * Enables dependency injection and mock testing.
 */
class INetwork {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    virtual ~INetwork() = default;
    virtual void startServer(unsigned short port) = 0;
    virtual void connectToPeer(const std::string& ip, const std::string& port) = 0;
    virtual void sendMessage(const std::string& nick, const std::string& msg) = 0;
    virtual void setMessageCallback(MessageCallback cb) = 0;
};