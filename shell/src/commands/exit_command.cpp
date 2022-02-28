#include "exit_command.h"

#include "shell/src/streams.h"

std::optional<int> shell::ExitCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    return std::nullopt;
}
