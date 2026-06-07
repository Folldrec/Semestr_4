#pragma once
#include "inetwork.hpp"
#include "message_formatter.hpp"
#include <memory>

/**
 * @brief Mediator between UI layer and network/business logic.
 * Implements the Controller pattern (MVP).
 */
class ChatController {
    std::unique_ptr<INetwork> m_network;
    MessageFormatter m_formatter;
    INetwork::MessageCallback m_onMessage;

public:
    explicit ChatController(std::unique_ptr<INetwork> net)
        : m_network(std::move(net))
    {
        m_network->setMessageCallback([this](const std::string& msg) {
            if (m_onMessage) m_onMessage(msg);
        });
    }

    void setOnMessage(INetwork::MessageCallback cb) {
        m_onMessage = std::move(cb);
    }

    void startServer(unsigned short port) {
        m_network->startServer(port);
    }

    void connectToPeer(const std::string& ip, const std::string& port) {
        m_network->connectToPeer(ip, port);
    }

    /// Відправляє повідомлення та повертає відформатований рядок для відображення
    std::string sendMessage(const std::string& nick, const std::string& msg) {
        m_network->sendMessage(nick, msg);
        return m_formatter.format(nick, msg);
    }
};