#pragma once
/**
 * @file commands/game_commands.h
 * @brief Конкретні команди для операцій над іграми та CommandHistory.
 *
 * @par Патерн: Command (Поведінковий)
 *
 * Реалізує:
 * - ExcludeGameCommand — виключає гру з підрахунку (soft-delete)
 * - RestoreGameCommand — повертає гру до підрахунку
 * - CommandHistory     — стек команд з підтримкою Undo
 *
 * @see ICommand, i_command.h
 */

#include "i_command.h"
#include "../types.h"

#include <vector>
#include <memory>
#include <cstdint>

/**
 * @brief Команда виключення гри з підрахунку.
 *
 * Встановлює game.excluded = true.
 * Undo: встановлює game.excluded = false.
 */
class ExcludeGameCommand final : public ICommand {
public:
    /**
     * @param user  Профіль гравця
     * @param appid ID гри для виключення
     */
    ExcludeGameCommand(UserProfile& user, uint64_t appid);

    void execute() override; ///< Виключає гру
    void undo()    override; ///< Повертає гру
    std::string description() const override;

private:
    UserProfile& user_;
    uint64_t     appid_;
    std::string  gameName_;
};

/**
 * @brief Команда повернення гри до підрахунку.
 *
 * Встановлює game.excluded = false.
 * Undo: встановлює game.excluded = true.
 */
class RestoreGameCommand final : public ICommand {
public:
    RestoreGameCommand(UserProfile& user, uint64_t appid);
    void execute() override;
    void undo()    override;
    std::string description() const override;

private:
    UserProfile& user_;
    uint64_t     appid_;
    std::string  gameName_;
};

/**
 * @brief Менеджер історії команд з підтримкою Undo.
 *
 * Зберігає стек виконаних команд та дозволяє їх скасувати.
 *
 * @par Патерн: Command (Command History / Invoker)
 */
class CommandHistory {
public:
    /**
     * @brief Виконує команду і додає до стеку.
     * @param cmd Команда для виконання
     */
    void execute(std::unique_ptr<ICommand> cmd);

    /**
     * @brief Скасовує останню виконану команду.
     * @return true якщо команда була скасована, false якщо стек порожній
     */
    bool undo();

    /// @brief Кількість команд в стеку
    size_t size() const { return history_.size(); }

    /// @brief true якщо є команди для скасування
    bool canUndo() const { return !history_.empty(); }

    /// @brief Опис останньої команди в стеку
    std::string lastDescription() const;

    /// @brief Очищає весь стек команд
    void clear() { history_.clear(); }

private:
    std::vector<std::unique_ptr<ICommand>> history_; ///< Стек виконаних команд
};
