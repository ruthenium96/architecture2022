#pragma once

#include "shell/src/streams.h"
#include "shell/src/state.h"

#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>

namespace shell {

using Arguments = std::vector<std::string>;

class ICommand {
public:
    ICommand(const std::string& name, const std::string& description = "") : name_(name), description_(description) {}

    virtual void execute(const Arguments& args, IState& state, IStreams& stream) = 0;

    virtual ~ICommand() = default;

private:
    std::string name_;
    std::string description_;
};

class Command : public ICommand {
public:
    Command(const std::string& name, const std::string& description = "") : ICommand(name, description) {}
    void execute(const Arguments& args, IState& state, IStreams& stream) override {}
};


struct ArgumentDescription {
    enum class ArgumentType {
        REQUIRED,
        OPTIONAL
    };

    ArgumentType type;
    std::string description;
};

struct CommandParameters {
    std::string name;
    std::string description;
    std::vector<ArgumentDescription> args_description;
};

class CommandManager {
public:
    CommandManager(const CommandManager &) = delete;

    void operator=(const CommandManager &) = delete;

    static CommandManager &get_instance();

    template<class CommandType>
    void RegisterCommand(const std::string &name, const CommandParameters &parameters) {
        if (commands_.find(name) != commands_.end()) {
            throw; // TODO: ADD EXCEPTION
        }

        for (size_t index = 0; index + 1 < parameters.args_description.size(); ++index) {
            if (parameters.args_description[index].type == ArgumentDescription::ArgumentType::OPTIONAL &&
                parameters.args_description[index + 1].type == ArgumentDescription::ArgumentType::REQUIRED) {
                throw; // TODO: ADD EXCEPTION
            }
        }
        commands_[name] = std::make_shared<CommandType>(parameters.name, parameters.description);
    }

    bool command_exist(const std::string& name) const;
    std::shared_ptr<ICommand> get_command(const std::string &name) const;

private:
    CommandManager() = default;

    std::unordered_map<std::string, std::shared_ptr<ICommand>> commands_;
};

} // namespace shell
