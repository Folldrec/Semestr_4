#pragma once
#include "message_factory.hpp"
#include <chrono>
#include <ctime>

/**
 * @brief Decorators that wrap IMessage to add behavior.
 * Pattern: Decorator (Structural)
 *
 * Problem solved: adding timestamp, encryption marker, or read-receipt
 * to any message type without modifying existing classes or creating
 * a combinatorial explosion of subclasses.
 */
class MessageDecorator : public IMessage {
protected:
    std::unique_ptr<IMessage> m_wrapped;
public:
    explicit MessageDecorator(std::unique_ptr<IMessage> msg)
        : m_wrapped(std::move(msg)) {}
    std::string type() const override { return m_wrapped->type(); }
};

/// Adds a timestamp prefix to any message
class TimestampDecorator : public MessageDecorator {
public:
    explicit TimestampDecorator(std::unique_ptr<IMessage> msg)
        : MessageDecorator(std::move(msg)) {}

    std::string render() const override {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char buf[10];
        std::strftime(buf, sizeof(buf), "%H:%M", std::localtime(&t));
        return std::string("[") + buf + "] " + m_wrapped->render();
    }
};

/// Marks message as encrypted (visual indicator)
class EncryptedDecorator : public MessageDecorator {
public:
    explicit EncryptedDecorator(std::unique_ptr<IMessage> msg)
        : MessageDecorator(std::move(msg)) {}

    std::string render() const override {
        return "\xF0\x9F\x94\x92 " + m_wrapped->render();  // 🔒
    }
};