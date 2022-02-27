#include "shell/src/shell.h"
#include "shell/src/utils/substitute.h"

namespace shell {

void Shell::execute(const Arguments& args, State& state, IStreams& streams) {

    status_ = ShellStatus::RUNNING;
    while (is_running()) {
        std::string cmd_line;
        std::getline(streams.get_in_stream(), cmd_line);
        if (cmd_line.empty()) {
            this->stop();
            return;
        }

        parser::Parser cmd_parser{cmd_line};
        StreamsBuffered streams_local;

        auto tokens = cmd_parser.parse_tokens();
        while (!tokens.empty()) {
            auto cmd_binding = shell::utils::construct_command_binding(tokens, state, streams_local);
            cmd_binding.call();
            tokens = cmd_parser.parse_tokens();
        }
        streams.get_out_stream() << streams_local.get_out_buffer();
        streams.get_err_stream() << streams_local.get_err_buffer();
    }
}
} // namespace shell
