#include "shell/src/shell.h"
#include "shell/src/utils/substitute.h"

#include <filesystem>


namespace shell {

    std::optional<int> Shell::execute(const Arguments& args, State& state, IStreams& streams) {

    state.get_env().set_var("PWD", std::filesystem::current_path());

    status_ = ShellStatus::RUNNING;
    while (is_running()) {
        std::string cmd_line;
        std::getline(streams.get_in_stream(), cmd_line);
        if (cmd_line.empty()) {
            this->stop();
            return 0;
        }

        parser::Parser cmd_parser{cmd_line};
        StreamsBuffered streams_local;

        auto tokens = cmd_parser.parse_tokens();
        while (!tokens.empty()) {
            streams_local.exchange_in_and_out();
            auto cmd_binding = shell::utils::construct_command_binding(tokens, state, streams_local);
            if (cmd_binding.call() == std::nullopt) {
                this->stop();
                return 0;
            }
            tokens = cmd_parser.parse_tokens();
        }
        streams.get_out_stream() << streams_local.get_out_buffer();
        streams.get_err_stream() << streams_local.get_err_buffer();
    }
    return std::nullopt;
}
} // namespace shell
