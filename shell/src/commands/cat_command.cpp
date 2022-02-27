#include "shell/src/commands/cat_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

void shell::CatCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    throw ShellException("cat command isn't implemented");
}
