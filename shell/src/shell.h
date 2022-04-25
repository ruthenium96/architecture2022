#pragma once

#include "shell/src/streams.h"
#include "shell/src/parser.h"
#include "shell/src/commands/command.h"

#include <string>

namespace shell {

// Looper-class, very special type of Command. It is child of Command for subshell functionality.
class Shell : public Command {
public:
    Shell(const std::string& name, const std::string& description = "") : Command(name, description) {}

    std::optional<int> execute(const Arguments& args, State& state, IStreams& stream) override;
    ~Shell() override = default;
private:
    enum class ShellStatus {
        IDLE, RUNNING, FINISHED
    };

    void stop() { status_ = ShellStatus::FINISHED; }
    bool is_running() const {return status_ == ShellStatus::RUNNING; }

private:
    ShellStatus status_{ShellStatus::IDLE};
};

} // namespace shell
