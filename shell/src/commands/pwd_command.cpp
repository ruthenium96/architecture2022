#include "shell/src/commands/pwd_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

void shell::PwdCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    throw ShellException("pwd command isn't implemented");
}
