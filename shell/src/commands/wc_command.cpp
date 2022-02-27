#include "shell/src/commands/wc_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

void shell::WcCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    throw ShellException("wc command isn't implemented");
}
