#pragma once

#include "shell/src/streams.h"
#include "shell/src/parser.h"
#include "shell/src/commands/command.h"

#include <string>

namespace shell {

class Shell {
public:
    Shell(const std::shared_ptr<IStreams>& streams) : streams_(streams) {}

    void start();

private:
    enum class ShellStatus {
        IDLE, RUNNING, FINISHED
    };

    void stop() { status_ = ShellStatus::FINISHED; }
    bool is_running() const {return status_ == ShellStatus::RUNNING; }

private:
    std::shared_ptr<IStreams> streams_;
    ShellStatus status_{ShellStatus::IDLE};
};

} // namespace shell
