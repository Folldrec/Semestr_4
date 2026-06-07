#pragma once
#include "../inetwork.hpp"
#include "singleton_logger.hpp"
#include "observer.hpp"
#include <memory>

/**
 * @brief Proxy that wraps INetwork to add logging and event notification.
 * Pattern: Proxy (Structural) — specifically a "logging proxy"
 *
 * Problem solved: we want to log all network calls and fire Observer
 * events without modifying ChatNetwork or violating SRP.
 * The proxy is transparent — it satisfies the same INetwork interface.
 */
class NetworkProxy : public INetwork {
    std::unique_ptr<INetwork> m_real;
    ChatEventBus&             m_bus;
    Logger&                   m_log;

public:
    NetworkProxy(std::unique_ptr<INetwork> real, ChatEventBus& bus)
        : m_real(std::move(real)),
          m_bus(bus),
          m_log(Logger::instance()) {}

    void startServer(unsigned short port) override {
        m_log.log("NetworkProxy: startServer(" + std::to_string(port) + ")");
        m_real->startServer(port);
        m_bus.notify({ ChatEvent::ServerStarted, std::to_string(port) });
    }

    void connectToPeer(const std::string& ip, const std::string& port) override {
        m_log.log("NetworkProxy: connectToPeer(" + ip + ":" + port + ")");
        m_real->connectToPeer(ip, port);
        m_bus.notify({ ChatEvent::PeerConnected, ip + ":" + port });
    }

    void sendMessage(const std::string& nick, const std::string& msg) override {
        m_log.log("NetworkProxy: sendMessage from " + nick);
        m_real->sendMessage(nick, msg);
        m_bus.notify({ ChatEvent::MessageSent, nick + ": " + msg });
    }

    void setMessageCallback(MessageCallback cb) override {
        // Wrap callback to also fire Observer event
        m_real->setMessageCallback([this, cb](const std::string& raw) {
            m_log.log("NetworkProxy: received: " + raw);
            m_bus.notify({ ChatEvent::MessageReceived, raw });
            cb(raw);
        });
    }
};