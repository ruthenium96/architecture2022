#include <memory>
#include "shell/src/commands/command.h"
#include "shell/src/commands/echo_cmd.h"

namespace shell {

void InitializeCommands(CommandManager& manager) {
    manager.RegisterCommand<EchoCommand>("echo", CommandParameters{});
}

CommandManager& CommandManager::get_instance() {
    static CommandManager manager;
    static bool initialized = false;
    if (!initialized) {
        InitializeCommands(manager);
        initialized = true;
    }

    return manager;
}

bool CommandManager::command_exist(const std::string &name) const {
    return commands_.find(name) != commands_.end();
}

std::shared_ptr<ICommand> CommandManager::get_command(const std::string &name) const {
    auto it = commands_.find(name);
    if (commands_.find(name) == commands_.end()) {
        return std::make_shared<Command>("empty"); // TODO: remove this line
    }
    return it->second;
}

}
