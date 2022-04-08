#include "shell/src/commands/echo_command.h"
#include "shell/src/streams.h"

std::optional<int> shell::EchoCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    auto& out_stream = stream.get_out_stream();
    for (size_t index = 0; index < args.size(); ++index) {
        out_stream << args[index] << " ";
    }
    out_stream << std::endl;
    return 0;
}
