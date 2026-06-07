#pragma once
/**
 * @file commands/i_command.h
 * @brief Патерн Command — інтерфейс команди.
 *
 * Інкапсулює операцію як об'єкт що дозволяє:
 * - Виконувати операцію (execute)
 * - Скасовувати операцію (undo)
 * - Зберігати та передавати операції
 *
 * @par Патерн: Command (Поведінковий)
 *
 * @see ExcludeGameCommand, RestoreGameCommand, CommandHistory
 */

#include <string>

/**
 * @brief Абстрактна команда з підтримкою Undo.
 *
 * @par Патерн: Command
 */
class ICommand {
public:
    virtual ~ICommand() = default;

    /// @brief Виконати команду
    virtual void execute() = 0;

    /// @brief Скасувати команду (зворотня дія)
    virtual void undo() = 0;

    /// @brief Опис команди для логів та UI
    virtual std::string description() const = 0;
};
