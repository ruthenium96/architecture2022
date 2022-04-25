#include "assign_command.h"

std::optional<int> shell::AssignCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    // If we have command with strange counts of '=':
    if (std::count(this->get_name().begin(), this->get_name().end(), '=') != 1) {
        return std::nullopt;
    }
    auto pos = this->get_name().find('=');
    std::string key, value;
    key = this->get_name().substr(0, pos);
    value = this->get_name().substr(pos + 1);

    state.get_env().set_var(std::move(key), std::move(value));

    return 0;
}