#pragma once

#include <utility>

#include "shell/src/commands/command.h"

namespace shell {

class CatCommand : public Command {
public:
    CatCommand(const std::string& name, const std::string& description) : Command(name, description) {}

    std::optional<int> execute(const Arguments& args, State& state, IStreams& stream) override;
};

}
