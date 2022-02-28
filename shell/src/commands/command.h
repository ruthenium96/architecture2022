#pragma once

#include "shell/src/streams.h"
#include "shell/src/state.h"

#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace shell {

using Arguments = std::vector<std::string>;

class Command {
public:
    Command(const std::string& name, const std::string& description = "") : name_(name), description_(description) {}

    virtual std::optional<int> execute(const Arguments& args, State& state, IStreams& stream) = 0;

    virtual ~Command() = default;

private:
    std::string name_;
    std::string description_;
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
    void register_command(const std::string &name, const CommandParameters &parameters = {}) {
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

    void initialize_builtin_commands();

    bool command_exist(const std::string& name) const;
    std::shared_ptr<Command> get_command(const std::string &name) const;

private:
    CommandManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Command>> commands_;
};

class CommandBinding {
public:
    CommandBinding(std::shared_ptr<Command> command, const Arguments& arguments, State& state, IStreams& streams)
    : args_(arguments), command_(command), state_(state), streams_(streams) {}
    std::optional<int> call();
    std::shared_ptr<Command> get_command() const;
    Arguments get_arguments() const;
private:
    Arguments args_;
    std::shared_ptr<Command> command_;
    State& state_;
    IStreams& streams_;
};

} // namespace shell
