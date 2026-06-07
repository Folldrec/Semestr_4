#pragma once
#include <string>
#include <memory>

/**
 * @brief Message types produced by Factory Method.
 * Pattern: Factory Method (Creational)
 *
 * Problem solved: different message types (text, system, error)
 * need different formatting/handling. Factory decouples creation
 * from usage — caller does not need to know concrete types.
 */
struct IMessage {
    virtual ~IMessage() = default;
    virtual std::string render() const = 0;
    virtual std::string type()   const = 0;
};

struct TextMessage : IMessage {
    std::string nick, text;
    TextMessage(std::string n, std::string t)
        : nick(std::move(n)), text(std::move(t)) {}
    std::string render() const override { return nick + ": " + text; }
    std::string type()   const override { return "text"; }
};

struct SystemMessage : IMessage {
    std::string text;
    explicit SystemMessage(std::string t) : text(std::move(t)) {}
    std::string render() const override { return "[System] " + text; }
    std::string type()   const override { return "system"; }
};

struct ErrorMessage : IMessage {
    std::string text;
    explicit ErrorMessage(std::string t) : text(std::move(t)) {}
    std::string render() const override { return "[Error] " + text; }
    std::string type()   const override { return "error"; }
};

/**
 * @brief Abstract factory method — subclasses decide which IMessage to create.
 */
class MessageFactory {
public:
    virtual ~MessageFactory() = default;
    virtual std::unique_ptr<IMessage> create(const std::string& payload) const = 0;
};

class TextMessageFactory : public MessageFactory {
    std::string m_nick;
public:
    explicit TextMessageFactory(std::string nick) : m_nick(std::move(nick)) {}
    std::unique_ptr<IMessage> create(const std::string& payload) const override {
        return std::make_unique<TextMessage>(m_nick, payload);
    }
};

class SystemMessageFactory : public MessageFactory {
public:
    std::unique_ptr<IMessage> create(const std::string& payload) const override {
        return std::make_unique<SystemMessage>(payload);
    }
};

class ErrorMessageFactory : public MessageFactory {
public:
    std::unique_ptr<IMessage> create(const std::string& payload) const override {
        return std::make_unique<ErrorMessage>(payload);
    }
};