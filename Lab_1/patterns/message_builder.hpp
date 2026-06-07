#pragma once
#include <string>

/**
 * @brief Builder for constructing complex outgoing messages.
 * Pattern: Builder (Creational)
 *
 * Problem solved: outgoing messages may optionally include
 * nick, timestamp, encryption flag, priority tag — building
 * them step-by-step is cleaner than a constructor with 5 params.
 */
class OutgoingMessage {
public:
    std::string nick;
    std::string body;
    bool        encrypted  = false;
    bool        withTime   = false;
    std::string tag;       // e.g. "#urgent"

    /// Final serialized form sent over the wire
    std::string serialize() const {
        std::string result;
        if (!tag.empty())      result += tag + " ";
        if (encrypted)         result += "[ENC] ";
        result += nick + ": " + body;
        return result;
    }
};

class OutgoingMessageBuilder {
    OutgoingMessage m_msg;
public:
    OutgoingMessageBuilder& setNick(const std::string& n)  { m_msg.nick = n;         return *this; }
    OutgoingMessageBuilder& setBody(const std::string& b)  { m_msg.body = b;         return *this; }
    OutgoingMessageBuilder& withEncryption()               { m_msg.encrypted = true;  return *this; }
    OutgoingMessageBuilder& withTimestamp()                { m_msg.withTime  = true;  return *this; }
    OutgoingMessageBuilder& withTag(const std::string& t)  { m_msg.tag = t;           return *this; }
    OutgoingMessage         build()                        { return m_msg; }
};