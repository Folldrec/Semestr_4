#pragma once
#include "inetwork.hpp"
#include <string>
#include <functional>
#include <thread>
#include <atomic>


/**
 * @brief Concrete implementation of INetwork using TCP sockets.
 * Depends on INetwork interface — can be replaced by MockNetwork in tests.
 */
class ChatNetwork : public INetwork {
    MessageCallback m_callback;
    std::atomic<bool> m_running{false};
    std::thread m_thread;

public:
    ~ChatNetwork() override {
        m_running = false;
        if (m_thread.joinable())
            m_thread.join();
    }

    void setMessageCallback(MessageCallback cb) override {
        m_callback = std::move(cb);
    }

    void startServer(unsigned short port) override {
        m_running = true;
        m_thread = std::thread([this, port]() {
        });
    }

    void connectToPeer(const std::string& ip, const std::string& port) override {
    }

    void sendMessage(const std::string& nick, const std::string& msg) override {
    }
};