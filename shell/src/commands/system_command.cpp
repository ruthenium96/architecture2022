#include "system_command.h"

#include "shell/src/streams.h"

#include <stdlib.h>
#include <fstream>
#include <filesystem>

std::optional<int> shell::SystemCommand::execute(const Arguments& args, State& state, IStreams& stream) {

    for (const auto& [key, value] : state.get_env()) {
        setenv(key.c_str(), value.c_str(), 1);
    }
    // create paths to temporary files
    std::string temp_path = std::filesystem::temp_directory_path();
    std::filesystem::path temp_in = temp_path;
    temp_in /= "in.txt";
    std::filesystem::path temp_out = temp_path;
    temp_out /= "out.txt";

    // write get_in_stream to file
    std::ofstream in_stream(temp_in.string());
    for (std::string line; std::getline(stream.get_in_stream(), line);) {
        in_stream << line << std::endl;
    }
    in_stream.close();

    // cat in.txt | 'COMMAND' ['ARGS'] > out.txt
    std::string external_command_string = "cat " + temp_in.string() + " | ";
    external_command_string += "'" + this->get_name() + "'";
    for (const auto& arg : args) {
        external_command_string += " '" + arg + "'";
    }
    external_command_string += "> " + temp_out.string();

    std::cout << external_command_string << std::endl;

    int exit_code = system(external_command_string.c_str());

    // write file to get_out_stream
    std::ifstream out_stream(temp_out.string());
    for (std::string line; std::getline(out_stream, line);) {
        stream.get_out_stream() << line << std::endl;
    }
    out_stream.close();

    std::filesystem::remove(temp_in.string());
    std::filesystem::remove(temp_out.string());

    return exit_code;
}