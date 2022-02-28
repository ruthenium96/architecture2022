#include "shell/src/commands/pwd_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

std::optional<int> shell::PwdCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    stream.get_out_stream() << state.get_env().get_var("PWD") << std::endl;
    return 0;
}
