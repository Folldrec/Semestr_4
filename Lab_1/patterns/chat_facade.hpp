#pragma once
#include "../inetwork.hpp"
#include "../chat_logic.hpp"
#include "singleton_logger.hpp"
#include "observer.hpp"
#include "proxy_network.hpp"
#include "message_factory.hpp"
#include "message_decorator.hpp"
#include "command.hpp"
#include "message_builder.hpp"
#include <memory>
#include <string>

/**
 * @brief Facade that unifies the entire chat subsystem.
 * Pattern: Facade (Structural)
 *
 * Problem solved: ChatFrame previously had to know about ChatNetwork,
 * MessageFormatter, Logger, EventBus, etc. The Facade provides one
 * simple interface to the whole subsystem, reducing coupling dramatically.
 */
class ChatFacade {
    ChatEventBus                  m_bus;
    std::unique_ptr<INetwork>     m_network;
    CommandHistory                m_cmdHistory;
    SystemMessageFactory          m_sysFactory;
    bool                          m_useTimestamps = true;
    bool                          m_useEncryption = false;

    std::function<void(const std::string&)> m_uiAppend;
    std::function<void()>                   m_uiRemoveLast;

public:
    ChatFacade() {
        Logger::instance().setFile("chat.log");

        auto real  = std::make_unique<ChatNetwork>();
        m_network  = std::make_unique<NetworkProxy>(std::move(real), m_bus);

        m_network->setMessageCallback([this](const std::string& raw) {
            auto msg = m_sysFactory.create(raw);
            appendToUI(decorate(std::move(msg)));
        });
    }

    void setUICallbacks(
        std::function<void(const std::string&)> append,
        std::function<void()>                   removeLast)
    {
        m_uiAppend     = std::move(append);
        m_uiRemoveLast = std::move(removeLast);
    }

    void subscribeObserver(IChatObserver* obs) { m_bus.subscribe(obs); }

    void setTimestamps(bool on) { m_useTimestamps = on; }
    void setEncryption(bool on) { m_useEncryption = on; }

    // --- Actions ---
    void startServer(unsigned short port) {
        m_network->startServer(port);
        auto msg = m_sysFactory.create("Server started on port " + std::to_string(port));
        appendToUI(decorate(std::move(msg)));
    }

    void connectToPeer(const std::string& ip, const std::string& port) {
        m_network->connectToPeer(ip, port);
    }

    /// Build and send a message using Builder + Factory + Decorator + Command
    std::string sendMessage(const std::string& nick, const std::string& body,
                            const std::string& tag = "") {
        OutgoingMessage out = OutgoingMessageBuilder()
            .setNick(nick)
            .setBody(body)
            .withTag(tag)
            .build();

        TextMessageFactory factory(nick);
        auto msg = factory.create(body);

        auto decorated = decorate(std::move(msg));

        appendToUI(decorated);

        m_network->sendMessage(nick, out.serialize());

        return decorated;
    }

    void undoLastMessage() {
        m_cmdHistory.undo();
    }

    bool canUndo() const { return m_cmdHistory.canUndo(); }

private:
    std::string decorate(std::unique_ptr<IMessage> msg) {
        if (m_useEncryption)
            msg = std::make_unique<EncryptedDecorator>(std::move(msg));
        if (m_useTimestamps)
            msg = std::make_unique<TimestampDecorator>(std::move(msg));
        return msg->render();
    }

    void appendToUI(const std::string& text) {
        if (!m_uiAppend) return;
        auto cmd = std::make_unique<AppendMessageCommand>(
            m_uiAppend, m_uiRemoveLast, text);
        m_cmdHistory.execute(std::move(cmd));
    }
};