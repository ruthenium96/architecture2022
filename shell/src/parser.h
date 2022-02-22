#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <sstream>
#include <string_view>
#include <memory>
#include <optional>

#include "shell/src/exception.h"
#include "shell/src/commands/command.h"

namespace shell::parser {

class IToken {
public:
    explicit IToken(std::string value) : value_(std::move(value)) {}
    std::string GetValue() const {
        return value_;
    }
    virtual ~IToken() {}
private:
    std::string value_;
};


class StringToken : public IToken {
public:
    StringToken(std::string value) : IToken(std::move(value)) {}
    ~StringToken() override = default;
};

class RawStringToken : public IToken {
public:
    RawStringToken(std::string value) : IToken(std::move(value)) {}
    ~RawStringToken() override = default;
};


using Argument = std::string;

struct CommandDescriptor {
    std::shared_ptr<ICommand> command;
    std::vector<Argument> arguments;
};


class Parser {
public:
    explicit Parser(std::string  line) : cmd_manager_(CommandManager::get_instance()), line_(std::move(line)), line_view_(line_) {}

    std::optional<CommandDescriptor> parse_command();

    std::shared_ptr<IToken> get_next_token();
private:
    const CommandManager& cmd_manager_;
    std::string line_;
    std::string_view line_view_;
};

} // namespace shell::parser
