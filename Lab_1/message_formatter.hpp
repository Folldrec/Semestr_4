#pragma once
#include <string>

/**
 * @brief Handles formatting of chat messages.
 * Separates presentation logic from UI and network layers.
 */
class MessageFormatter {
public:
    std::string format(const std::string& nick, const std::string& msg) const {
        return nick + ": " + msg;
    }

    std::string formatSystem(const std::string& msg) const {
        return "[System] " + msg;
    }
};