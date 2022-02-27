#include "shell/src/commands/wc_command.h"
#include "shell/src/streams.h"
#include "shell/src/exception.h"

#include <fstream>

void shell::WcCommand::execute(const Arguments& args, State& state, IStreams& stream) {

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
            local_chars += line.size();
        }
        stream.get_out_stream() << local_lines << " " << local_words << " "
        << local_chars << " " << filename << std::endl;

        total_lines += local_lines;
        total_words += local_words;
        total_chars += local_chars;
    }
    stream.get_out_stream() << total_lines << " " << total_words << " "
                            << total_chars << " total" << std::endl;
    return;
}
