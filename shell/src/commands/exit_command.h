#ifndef ARCHITECTURE_EXIT_H
#define ARCHITECTURE_EXIT_H

#include <utility>

#include "shell/src/commands/command.h"

namespace shell {

    class ExitCommand : public Command {
    public:
        ExitCommand(const std::string& name, const std::string& description) : Command(name, description) {}

        std::optional<int> execute(const Arguments& args, State& state, IStreams& stream) override;
    };

}


#endif //ARCHITECTURE_EXIT_H
