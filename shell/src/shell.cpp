#include "shell/src/shell.h"
#include "shell/src/utils/substitute.h"

#include <filesystem>


namespace shell {

    std::optional<int> Shell::execute(const Arguments& args, State& state, IStreams& streams) {

    state.get_env().set_var("PWD", std::filesystem::current_path());

    status_ = ShellStatus::RUNNING;
    while (is_running()) {
        std::string cmd_line;
        // print invitation only if streams is global streams:
        if (dynamic_cast<StreamsGlobal*>(&streams)) {
            streams.get_out_stream() << state.get_env().get_var("PWD") << "$ ";
        }
        std::getline(streams.get_in_stream(), cmd_line);
        if (cmd_line.empty()) {
            this->stop();
            return 0;
        }

        parser::Parser cmd_parser{cmd_line};
        StreamsBuffered streams_local;

        auto tokens = cmd_parser.parse_tokens();
        if (!tokens.has_value()) {
            streams.get_err_stream() << "INCORRECT INPUT" << std::endl;
            continue;
        }
        while (!tokens.value().empty()) {
            streams_local.exchange_in_and_out();
            auto cmd_binding = shell::utils::construct_command_binding(tokens.value(), state, streams_local);
            if (cmd_binding.call() == std::nullopt) {
                this->stop();
                return 0;
            }
            tokens = cmd_parser.parse_tokens();
            if (!tokens.has_value()) {
                streams.get_err_stream() << "INCORRECT INPUT" << std::endl;
                continue;
            }
        }
        streams.get_out_stream() << streams_local.get_out_buffer();
        streams.get_err_stream() << streams_local.get_err_buffer();
    }
    return std::nullopt;
}
} // namespace shell
