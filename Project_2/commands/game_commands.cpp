#include "game_commands.h"
#include <algorithm>
#include <iostream>


static Game* findGame(UserProfile& user, uint64_t appid) {
    for (auto& g : user.games)
        if (g.appid == appid) return &g;
    return nullptr;
}

ExcludeGameCommand::ExcludeGameCommand(UserProfile& user, uint64_t appid)
    : user_(user), appid_(appid)
{
    auto* g = findGame(user_, appid_);
    gameName_ = g ? g->name : std::to_string(appid_);
}

void ExcludeGameCommand::execute() {
    auto* g = findGame(user_, appid_);
    if (g) {
        g->excluded = true;
        std::cout << "[Command] Виключено: " << gameName_ << "\n";
    }
}

void ExcludeGameCommand::undo() {
    auto* g = findGame(user_, appid_);
    if (g) {
        g->excluded = false;
        std::cout << "[Command] Повернуто: " << gameName_ << "\n";
    }
}

std::string ExcludeGameCommand::description() const {
    return "Виключити гру: " + gameName_;
}


RestoreGameCommand::RestoreGameCommand(UserProfile& user, uint64_t appid)
    : user_(user), appid_(appid)
{
    auto* g = findGame(user_, appid_);
    gameName_ = g ? g->name : std::to_string(appid_);
}

void RestoreGameCommand::execute() {
    auto* g = findGame(user_, appid_);
    if (g) {
        g->excluded = false;
        std::cout << "[Command] Повернуто: " << gameName_ << "\n";
    }
}

void RestoreGameCommand::undo() {
    auto* g = findGame(user_, appid_);
    if (g) {
        g->excluded = true;
        std::cout << "[Command] Виключено: " << gameName_ << "\n";
    }
}

std::string RestoreGameCommand::description() const {
    return "Повернути гру: " + gameName_;
}

void CommandHistory::execute(std::unique_ptr<ICommand> cmd) {
    cmd->execute();
    history_.push_back(std::move(cmd));
}

bool CommandHistory::undo() {
    if (history_.empty()) return false;
    history_.back()->undo();
    history_.pop_back();
    return true;
}

std::string CommandHistory::lastDescription() const {
    if (history_.empty()) return "";
    return history_.back()->description();
}
