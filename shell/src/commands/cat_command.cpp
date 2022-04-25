#include "shell/src/commands/cat_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

#include <fstream>

std::optional<int> shell::CatCommand::execute(const Arguments& args, State& state, IStreams& stream) {

    if (args.empty()) {
        for (std::string line; std::getline(stream.get_in_stream(), line);) {
            stream.get_out_stream() << line << std::endl;
        }
        return 0;
    }

    for (const auto& filename : args) {
        std::fstream filestream(filename);

        if (!filestream.is_open()) {
            stream.get_err_stream() << "Failed to open " << filename << std::endl;
            continue;
        }

        for (std::string line; std::getline(filestream, line);) {
            stream.get_out_stream() << line << std::endl;
        }
    }
    return 0;
}
