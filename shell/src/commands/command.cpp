#include <memory>

#include "shell/src/exception.h"
#include "shell/src/commands/command.h"
#include "shell/src/commands/assign_command.h"
#include "shell/src/commands/echo_command.h"
#include "shell/src/commands/exit_command.h"
#include "shell/src/commands/cat_command.h"
#include "shell/src/commands/pwd_command.h"
#include "shell/src/commands/wc_command.h"
#include "shell/src/commands/system_command.h"


namespace shell {

CommandManager& CommandManager::get_instance() {
    static CommandManager manager;

    return manager;
}

bool CommandManager::command_exist(const std::string &name) const {
    return commands_.find(name) != commands_.end();
}

std::shared_ptr<Command> CommandManager::get_command(const std::string &name) const {
    auto it = commands_.find(name);
    if (commands_.find(name) == commands_.end()) {
        // REDO: move it to another place?
        if (std::count(name.cbegin(), name.cend(), '=') != 0) {
            auto ptr = std::make_shared<AssignCommand>(AssignCommand(name, ""));
            return ptr;
        }
        auto ptr = std::make_shared<SystemCommand>(SystemCommand(name, ""));
        return ptr;
    }
    return it->second;
}

void CommandManager::initialize_builtin_commands() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    register_command<EchoCommand>("echo");
    register_command<ExitCommand>("exit");
    register_command<CatCommand>("cat");
    register_command<WcCommand>("wc");
    register_command<PwdCommand>("pwd");

    initialized = true;
}

std::optional<int> CommandBinding::call() {
    return command_->execute(args_, state_, streams_);
}

std::shared_ptr<Command> CommandBinding::get_command() const {
    return command_;
}

Arguments CommandBinding::get_arguments() const {
    return args_;
}

}
