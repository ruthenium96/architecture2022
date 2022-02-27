#pragma once

#include <utility>

#include "shell/src/commands/command.h"

namespace shell {

class PwdCommand : public Command {
public:
    PwdCommand(const std::string& name, const std::string& description) : Command(name, description) {}

    void execute(const Arguments& args, State& state, IStreams& stream) override;
};

}
