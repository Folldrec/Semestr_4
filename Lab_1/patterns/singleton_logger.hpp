#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>

/**
 * @brief Singleton logger. Single instance, accessible globally.
 * Pattern: Singleton (Creational)
 *
 * Problem solved: need one shared log accessible from UI, network,
 * controller — without passing references everywhere.
 */
class Logger {
public:
    /// Returns the single instance
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string entry = timestamp() + " " + msg;
        m_history.push_back(entry);
        if (m_file.is_open())
            m_file << entry << "\n";
    }

    void setFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file.open(path, std::ios::app);
    }

    const std::vector<std::string>& history() const { return m_history; }

    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;

    std::string timestamp() const {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
        return std::string("[") + buf + "]";
    }

    std::vector<std::string> m_history;
    std::ofstream            m_file;
    std::mutex               m_mutex;
};