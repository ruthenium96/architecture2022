#pragma once

#include <exception>
#include <string>

namespace shell {



class ShellException : public std::runtime_error {
public:
    explicit ShellException(const std::string& what) : std::runtime_error("Shell exception: " + std::string{what}) {}
};

struct ParsingError : public std::exception {
    char const* what() const noexcept override {
        return "Failed to parse tokens!";
    }
};

struct TokenizeError : public std::exception {
    char const* what() const noexcept override {
        return "Failed to tokenize line!";
    }
};

struct CmdNotFoundException : public std::exception {
    CmdNotFoundException(const std::string& error_cmd_line = {}) : error_cmd_line_{error_cmd_line} {}

    char const* what() const noexcept override {
        return "Command is not found!";
    }

    std::string get_error_command() const { return "Command: \"" + error_cmd_line_ + "\""; }

private:
    std::string error_cmd_line_;
};


} // namespace shell
