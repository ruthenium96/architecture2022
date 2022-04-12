#include "shell/src/commands/echo_command.h"
#include "shell/src/streams.h"

std::optional<int> shell::EchoCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    auto& out_stream = stream.get_out_stream();
    size_t index = 0;
    for (; index + 1 < args.size(); ++index) {
        out_stream << args[index] << " ";
    }
    if (args.size() != 0) {
        out_stream << args[index];
    }
    out_stream << std::endl;
    return 0;
}
