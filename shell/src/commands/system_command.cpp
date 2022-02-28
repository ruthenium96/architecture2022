#include "system_command.h"

#include "shell/src/streams.h"

#include <stdlib.h>

std::optional<int> shell::SystemCommand::execute(const Arguments& args, State& state, IStreams& stream) {

    for (const auto& [key, value] : state.get_env()) {
        setenv(key.c_str(), value.c_str(), 1);
    }
    return system("echo $azaza");
}