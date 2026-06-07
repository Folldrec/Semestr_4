#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * @brief Command pattern for undoable chat actions.
 * Pattern: Command (Behavioral)
 *
 * Problem solved: "time machine" — user can undo sent messages
 * (remove from local history), replay history, or add new action
 * types without modifying ChatController.
 */
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo()    = 0;
    virtual std::string description() const = 0;
};

/// Command: append a line to chat history
class AppendMessageCommand : public ICommand {
    std::function<void(const std::string&)> m_append;
    std::function<void()>                   m_removeLast;
    std::string                             m_text;
public:
    AppendMessageCommand(
        std::function<void(const std::string&)> append,
        std::function<void()>                   removeLast,
        std::string                             text)
        : m_append(std::move(append)),
          m_removeLast(std::move(removeLast)),
          m_text(std::move(text)) {}

    void execute() override        { m_append(m_text); }
    void undo()    override        { m_removeLast(); }
    std::string description() const override { return "Append: " + m_text; }
};

/// Invoker — stores history and supports undo
class CommandHistory {
    std::vector<std::unique_ptr<ICommand>> m_done;
public:
    void execute(std::unique_ptr<ICommand> cmd) {
        cmd->execute();
        m_done.push_back(std::move(cmd));
    }

    void undo() {
        if (!m_done.empty()) {
            m_done.back()->undo();
            m_done.pop_back();
        }
    }

    bool canUndo() const { return !m_done.empty(); }
    size_t size()  const { return m_done.size(); }
};