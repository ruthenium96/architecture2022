#include "shell/src/shell.h"

namespace shell {

void Shell::start() {

    status_ = ShellStatus::RUNNING;
    State state;
    while (is_running()) {
        std::string cmd_line;
        std::getline(streams_->get_in_stream(), cmd_line);
        if (cmd_line.empty()) {
            this->stop();
            return;
        }

        parser::Parser cmd_parser{cmd_line};

        StreamsBuffered streams_local;

        while (true) {
            auto descriptor = cmd_parser.parse_command();
            if (descriptor == std::nullopt) {
                break;
            }

            /* TODO: command processor should be there*/

            descriptor.value().command->execute(descriptor.value().arguments, state, streams_local);

        }
        streams_->get_out_stream() << streams_local.get_out_buffer();
        streams_->get_err_stream() << streams_local.get_err_buffer();
    }
}
} // namespace shell
