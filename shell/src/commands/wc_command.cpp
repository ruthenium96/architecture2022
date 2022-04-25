#include "shell/src/commands/wc_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

#include <fstream>

std::optional<int> shell::WcCommand::execute(const Arguments& args, State& state, IStreams& stream) {
    if (args.size() == 0) {
        return execute_for_stdin(args, state, stream);
    } else {
        return execute_for_files(args, state, stream);
    }
}

std::optional<int> shell::WcCommand::execute_for_files(const Arguments& args, State& state, IStreams& stream) {

    unsigned long long total_lines = 0;
    unsigned long long total_words = 0;
    unsigned long long total_chars = 0;

    for (auto filename : args) {
        std::fstream filestream(filename);

        if (!filestream.is_open()) {
            stream.get_err_stream() << "Failed to open " << filename << std::endl;
            continue;
        }

        unsigned long long local_lines = 0;
        unsigned long long local_words = 0;
        unsigned long long local_chars = 0;

        for (std::string line; std::getline(filestream, line);) {
            local_lines++;
            local_words += std::count(line.cbegin(), line.cend(), ' ') + 1;
            local_chars += line.size() + 1;
        }
        stream.get_out_stream() << local_lines << " " << local_words << " "
        << local_chars << " " << filename << std::endl;

        total_lines += local_lines;
        total_words += local_words;
        total_chars += local_chars;
    }
    if (args.size() > 1) {
        stream.get_out_stream() << total_lines << " " << total_words << " "
        << total_chars << " total" << std::endl;
    }
    return 0;
}

std::optional<int> shell::WcCommand::execute_for_stdin(const Arguments& args, State& state, IStreams& stream) {

    unsigned long long local_lines = 0;
    unsigned long long local_words = 0;
    unsigned long long local_chars = 0;

    for (std::string line; std::getline(stream.get_in_stream(), line);) {
        local_lines++;
        local_words += std::count(line.cbegin(), line.cend(), ' ') + 1;
        local_chars += line.size() + 1;
    }

    stream.get_out_stream() << local_lines << " " << local_words << " "
    << local_chars << std::endl;
    return 0;
}