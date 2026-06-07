#pragma once
#include <functional>
#include <vector>
#include <string>

/**
 * @brief Observer pattern for chat events.
 * Pattern: Observer (Behavioral)
 *
 * Problem solved: multiple UI components (chat history, status bar,
 * notification badge) need to react to the same events independently,
 * without tight coupling between event source and handlers.
 */
enum class ChatEvent {
    MessageReceived,
    MessageSent,
    PeerConnected,
    PeerDisconnected,
    ServerStarted
};

struct ChatEventData {
    ChatEvent   type;
    std::string payload;  
};

class IChatObserver {
public:
    virtual ~IChatObserver() = default;
    virtual void onEvent(const ChatEventData& e) = 0;
};

class ChatEventBus {
    std::vector<IChatObserver*> m_observers;
public:
    void subscribe(IChatObserver* obs) {
        m_observers.push_back(obs);
    }

    void unsubscribe(IChatObserver* obs) {
        m_observers.erase(
            std::remove(m_observers.begin(), m_observers.end(), obs),
            m_observers.end()
        );
    }

    void notify(const ChatEventData& e) {
        for (auto* obs : m_observers)
            obs->onEvent(e);
    }
};