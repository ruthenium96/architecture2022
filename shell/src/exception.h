#pragma once

#include <exception>
#include <string>

namespace shell {

struct SubcommandParseException : public std::exception {
    char const* what() const noexcept override {
        return "Failed to parse command!";
    }
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

struct CmdErrorException : public std::exception {
    char const* what() const noexcept override {
        return "Error was raised during command execution!";
    }
};

struct CmdExitException : public std::exception {
    char const* what() const noexcept override {
        return "Execution finished using exit command!";
    }
};

} // namespace shell
